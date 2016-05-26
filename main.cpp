#include <iostream>
#include <vector>
#include <stdio.h>
#include <limits.h>
#include "runningcar.h"
using namespace std;
Serial* SP = new Serial("\\\\.\\COM5");    // adjust as needed

std::vector<Position> vectorPositions; //array to store all postions neeed to check
std::vector<Position> CheckedPositions; //
std::vector<Position> FreePositions; //

 int travelPath[ChessBoardWidth+2][ChessBoardWidth+2];
void travel_from_pos_to_pos(Position FromPos,Position *ToPos);
void map_exploration();
void remove_pos(Position v_pos);
bool not_in_the_list(Position v_pos);
void PrintBoard();

int main(int argc, char**argv)
{

 map_exploration();

 return 0;

}



void Print_list(char *text,std::vector<Position> * v)
{
    std::cout<< text << std::endl;
    Position it;

     for ( int i =0; i<v->size(); ++i)
    {
        it = v->at(i);
        std::cout << "(" << it.horizontal << "," << it.vertical << ");";
    }

}
Position getPos(Position v_Pos,char v_side)
{
    string str;
    std::stringstream ss;

    Position tmpPos;
    tmpPos = v_Pos;

    ss << v_Pos.Direction << v_side;
    str = ss.str();
    //std::cout << v_Pos.Direction << v_side << "\n";
    if ((str == "EL") || (str == "WR") || (str == "SB") || (str == "NF"))
        tmpPos.horizontal = tmpPos.horizontal-1;

    if ((str == "ER") || (str == "WL") || (str == "SF") || (str == "NB"))
        tmpPos.horizontal = tmpPos.horizontal+1;

    if ((str == "EB") || (str == "WF") || (str == "SR") || (str == "NL"))
        tmpPos.vertical = tmpPos.vertical-1;

    if ((str == "EF") || (str == "WB") || (str == "SL") || (str == "NR"))
    {
        tmpPos.vertical = tmpPos.vertical+1;
    }


    return tmpPos;
}
char Command_Sensor(char *ch)
{
    Sleep(500);
    clock_t begin, end;
    double time_spent;
    int running_time;

    bool ii = SP->WriteData(ch,1);
    begin = clock();
    int readResult = 1;
    char incomingData[MAX_LEN] = "";

         do {
            memset(incomingData, '\0', MAX_LEN);
            readResult = SP->ReadData(incomingData,10);
           if ((strncmp(incomingData,"1",1)==0) || (strncmp(incomingData,"0",1)==0) )
            {
                 printf("Received sensor info from car:%s\n",incomingData);

                 break;
            }
            end = clock();
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        } while ((time_spent<3) ); //waiting for 2 seconds

   //two seconds for the led off (receiving led)
    printf("time spent:%f\n",time_spent );
    if (strncmp(incomingData,"0",1)==0) return '0';
    if (strncmp(incomingData,"1",1)==0) return '1';

}

void Command_Data (char *ch, int sleeptime)
{
    Sleep(500);
    clock_t begin, end;
    double time_spent;
    int running_time;

    bool ii = SP->WriteData(ch,1);
    begin = clock();

    int readResult = 1;
    char incomingData[MAX_LEN] = "";

        do {
            memset(incomingData, '\0', MAX_LEN);
            readResult = SP->ReadData(incomingData,10);
            if (!strncmp(ch,incomingData,1))
            {
                 printf("Received complete message from car:%s\n",incomingData);

                 break;
            }
            end = clock();
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        } while ((time_spent<10) ); //waiting for 10 seconds


   //two seconds for the led off (receiving led)
    printf("time spent:%f\n",time_spent );

    if (time_spent>=10)
    {
        printf("Warning: no response from car.\n");
        //exit (0);
    }
}

void remove_pos(Position v_pos)
{

  //  for (int i=1; i<=5; i++) myvector.push_back(i);
    Position it;
    for ( int i =0; i<vectorPositions.size(); ++i)
    {
        it = vectorPositions.at(i);
        if ((it.horizontal == v_pos.horizontal) && (it.vertical == v_pos.vertical)) //check if input position is the one in iterator
            {
                vectorPositions.erase(vectorPositions.begin()+i); //remove this pos out of the list
                return;
            }
    }

    //if no position to delete print a warning message
    std::cout << "Warning: The position [" << v_pos.horizontal << "," << v_pos.vertical << "] is not in the list!";

}

bool not_in_the_list(Position v_pos,std::vector<Position> * v)
{
    Position it;
     for ( int i =0; i<v->size(); ++i)
    {
        it = v->at(i);
        if ((it.horizontal == v_pos.horizontal) && (it.vertical == v_pos.vertical)) //check if input position is the one in iterator
            {
                return false; //the v_pos is in the list
            }
    }
    return true; //v_pos is not in the list
}

bool checkBoundary(Position v_Pos)
{
    //Check if position is outside of the board. will return false
    if ((v_Pos.horizontal<=0) || (v_Pos.vertical <=0) || (v_Pos.horizontal>ChessBoardWidth) || (v_Pos.vertical>ChessBoardWidth))
        return false;

    return true;
}

void TurnTheCar(Position *v_currPos, char v_turn)
{
    std::string str;
    std::stringstream ss;

    ss << v_currPos->Direction << v_turn;
    str = ss.str();
    //std::cout << "turn to direction" << str <<std::endl;
    if ((str == "EL") || (str == "WR") || (str == "SB") || (str == "NF"))
        v_currPos->Direction ='N';

    if ((str == "ER") || (str == "WL") || (str == "SF") || (str == "NB"))
        v_currPos->Direction ='S';

    if ((str == "EB") || (str == "WF") || (str == "SR") || (str == "NL"))
        v_currPos->Direction ='W';

    if ((str == "EF") || (str == "WB") || (str == "SL") || (str == "NR"))
    {
        v_currPos->Direction ='E';
    }
    //std::cout << "turn to " << v_currPos->Direction <<std::endl;
}

//Check if there is a ball in this position
bool checking_pos(Position v_pos)
{


    if (checkBoundary(v_pos))
            {
                //cout << v_pos.horizontal<<v_pos.vertical << "\n";
                //Send a sensor signal to the car. Return value is a char with 0 or 1
                char sensor_info = Command_Sensor("S");

                if ((sensor_info == '0')) //check sensor and update the Ball table
                    {
                        //add v_pos to the free list
                        FreePositions.push_back(v_pos);
                        return true;
                    }
                else
                {
                        return false;
                }
            }

    return false;
}
void map_exploration()
{
    Position t_pos;
    Position leftPos, rightPos, backPos,frontPos;
    char sensor_info;

    //initiate the ball_array; -1 mean this pos is not check yet
   //dataInitialize();

    //data init
    vectorPositions.clear();
    CheckedPositions.clear();
    FreePositions.clear();

     if (SP->IsConnected())
		cout << "Connected successfully to the xbee.\n";

    //add the first pos to the list
    Position v_currPos ;
    v_currPos.Direction ='S';
    v_currPos.horizontal = 1;
    v_currPos.vertical = 1;
    vectorPositions.push_back(v_currPos);
    CheckedPositions.push_back(v_currPos);
    FreePositions.push_back(v_currPos);

    while (vectorPositions.size()>0)
    {



            leftPos = getPos(v_currPos,'L');
            rightPos = getPos(v_currPos,'R');
            backPos = getPos(v_currPos,'B');
            frontPos = getPos(v_currPos,'F');

            if (not_in_the_list(frontPos,&vectorPositions) && not_in_the_list(frontPos,&CheckedPositions) &&checkBoundary(frontPos))
            {

                if (checking_pos(frontPos))
                {

                    vectorPositions.push_back(frontPos);
                    //return;


                }

                //add to checked pos list
                CheckedPositions.push_back(frontPos);
            }


            //Check right position
            if (not_in_the_list(rightPos,&vectorPositions) && not_in_the_list(rightPos,&CheckedPositions) &&checkBoundary(rightPos))
                {
                    //Physical Turn
                    Command_Data("R",1000);

                    //Logically turn car to the right
                    TurnTheCar(&v_currPos,'R');

                    if (checking_pos(rightPos))
                    vectorPositions.push_back(rightPos);

                    //add to checked pos list
                    CheckedPositions.push_back(rightPos);
                }

            //Check left position
            if (not_in_the_list(leftPos,&vectorPositions) && not_in_the_list(leftPos,&CheckedPositions) &&checkBoundary(leftPos))
                {
                    //Turn car to the left
                    //Physical Turn
                    Command_Data("L",1000);

                    //Logical turn (data)
                    TurnTheCar(&v_currPos,'L');

                    if (checking_pos(leftPos))
                    vectorPositions.push_back(leftPos);

                    //add to checked pos list
                    CheckedPositions.push_back(leftPos);
                }

            //Which position to check next
            //Find closest position from the list
//            t_pos = get_to_closest_pos(vectorPositions,v_currPos);
//            if (t_pos.horizontal !=0)
//            {
//                //travel to that position
//                travel_from_pos_to_pos(v_currPos,&t_pos);
//            }
//
//
//            //remove v_currentPos out of the list;

             Print_list("Vector list",&vectorPositions);

             remove_pos(v_currPos);
             //vectorPositions.erase (vectorPositions.begin());

             if (vectorPositions.size()>0)
             {

                 travel_from_pos_to_pos(v_currPos,&vectorPositions.back());
                 v_currPos = vectorPositions.back();
                 std::cout<< "Current pos:" << v_currPos.horizontal << "," << v_currPos.vertical << std::endl;


             }



//
//            //update currentPos;
//            v_currPos = t_pos;

    }
    return;
}

bool free_pos_in_list(int hor, int ver,std::vector<Position> * v)
{
    Position it;
     for ( int i =0; i<v->size(); ++i)
    {
        it = v->at(i);
        if ((it.horizontal == hor) && (it.vertical == ver)) //check if input position is the one in iterator
            {
                return true; //the v_pos is in the list
            }
    }
    return false; //v_pos is not in the list
}

void travel_from_pos_to_pos(Position FromPos,Position *ToPos)
{


    Position v_toPos = *ToPos;

    for (int i=0;i<ChessBoardWidth+1;i++)
        for (int j=0;j<ChessBoardWidth+1;j++)
        {
            travelPath[i][j] = SHRT_MAX;
        }

    travelPath[ToPos->horizontal][ToPos->vertical] = 0;

    bool v_check = true;

    //Set value to the matrix
        do {
        v_check = false;

        for (int i = ChessBoardWidth;i>0;i--)
            for (int j = ChessBoardWidth; j>0; j--)
            {

                if (free_pos_in_list(i,j,&FreePositions))
                    {
                        if ((j+1<=ChessBoardWidth) && (free_pos_in_list(i,j+1,&FreePositions)) && (travelPath[i][j+1]>travelPath[i][j]+1))
                        {

                              v_check = true;
                             travelPath[i][j+1] = travelPath[i][j]+1 ;
                        }
                        if ((j-1>0) && (free_pos_in_list(i,j-1,&FreePositions)) && (travelPath[i][j-1]>travelPath[i][j]+1))
                        {

                              v_check = true;
                             travelPath[i][j-1] = travelPath[i][j]+1 ;
                        }

                        if ((i-1>0) && (free_pos_in_list(i-1,j,&FreePositions)) && (travelPath[i-1][j]>travelPath[i][j]+1))
                        {

                              v_check = true;
                             travelPath[i-1][j] = travelPath[i][j]+1 ;
                        }
                        if ((i+1<=ChessBoardWidth) && (free_pos_in_list(i+1,j,&FreePositions)) && (travelPath[i+1][j]>travelPath[i][j]+1))
                        {

                              v_check = true;
                             travelPath[i+1][j] = travelPath[i][j]+1 ;
                        }
                    }

            }
    } while (v_check);



    PrintBoard();

    //Find the way from the matrix

    Position v_pos,leftPos,rightPos,frontPos;
    v_pos = FromPos;
    int current_step = travelPath[v_pos.horizontal][v_pos.vertical];
    do
    {
            leftPos = getPos(v_pos,'L');
            rightPos = getPos(v_pos,'R');
            frontPos = getPos(v_pos,'F');
        //Go strait
        if (checkBoundary(frontPos) && (travelPath[v_pos.horizontal][v_pos.vertical]-1 == travelPath[frontPos.horizontal][frontPos.vertical]))
        {
            //TurnTheCar(&v_currPos,'L');
            Command_Data("F",500);
            frontPos.Direction = v_pos.Direction;
            v_pos= frontPos;

            continue;
        }
        //Go left
        if (checkBoundary(leftPos) && (travelPath[v_pos.horizontal][v_pos.vertical]-1 == travelPath[leftPos.horizontal][leftPos.vertical]))
        {
            TurnTheCar(&v_pos,'L');
            leftPos.Direction = v_pos.Direction;
            Command_Data("L",1000);
            Command_Data("F",500);
            v_pos= leftPos;
            continue;
        }

        //Go right
         if (checkBoundary(rightPos) && (travelPath[v_pos.horizontal][v_pos.vertical]-1 == travelPath[rightPos.horizontal][rightPos.vertical]))
        {
            TurnTheCar(&v_pos,'R');
            rightPos.Direction = v_pos.Direction;
            Command_Data("R",1000);
            Command_Data("F",500);
            v_pos= rightPos;
            continue;
        }

    }
    while ((v_pos.horizontal != ToPos->horizontal) || (v_pos.vertical != ToPos->vertical));

    //Set ToPos Direction
    ToPos->Direction = v_pos.Direction;
}

void PrintBoard()
{
     int i, j;
        for (i=1;i<ChessBoardWidth+1;i++)
        {
            cout << endl;

            for (j=1;j<ChessBoardWidth+1;j++)
                {
                    if (travelPath[i][j]<0) cout << "0 ";
                    else
                    cout << travelPath[i][j] << " ";
                }
        }
        cout << endl;
}
