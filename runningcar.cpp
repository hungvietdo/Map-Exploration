#include "runningcar.h"
#include <limits.h>
using namespace std;
Serial* SP = new Serial("\\\\.\\COM5");    // adjust as needed

int BallsArray[ChessBoardWidth][ChessBoardWidth];
int VisitsArray[ChessBoardWidth][ChessBoardWidth];
int VisitsArrayTime[ChessBoardWidth][ChessBoardWidth];
int ShortPath[ChessBoardWidth][ChessBoardWidth];
int travelPath[ChessBoardWidth+2][ChessBoardWidth+2];
int tmpShortPath[ChessBoardWidth+2][ChessBoardWidth+2];
int BallExploration[ChessBoardWidth+2][ChessBoardWidth+2];

int VisitTimeCount;

//position BlockArray[6] = {{1,4},{2,3},{4,2},{6,1},{4,6},{5,5}};
//Position TravelArray[200];

Position TravelArray[200];

Position FinishPos = {FinishX,FinishY};
Position StartPos = {StartX,StartY};


bool checking_pos(Position frontPos)
{


    if (checkBoundary(frontPos))
            {
                //cout << frontPos.horizontal<<frontPos.vertical << "\n";
                //Send a sensor signal to the car. Return value is a char with 0 or 1
                char sensor_info = Command_Sensor("S");

                if ((sensor_info == '0') || (BallExploration[frontPos.horizontal][frontPos.vertical] ==1)) //check sensor and data before
                    {
                    //add frontPos to the list

                        BallExploration[frontPos.horizontal][frontPos.vertical] = 0; //free position
                        return true;
                    }
                else
                {
                    BallExploration[frontPos.horizontal][frontPos.vertical] = 1; //blocked position
                    return false;
                }
            }

    return false;
}

int get_min_step(Position from_pos,Position to_pos)
{
    int i,j;
    bool v_check;


    for (i=0;i<ChessBoardWidth+1;i++)
        for (j=0;j<ChessBoardWidth+1;j++)
        {
            //BallExploration[i][j] = -1;

            tmpShortPath[i][j] = SHRT_MAX;//
        }

    //Init the first pos
    tmpShortPath[from_pos.horizontal][from_pos.vertical] = 0;

    //Calculate the shortest matrix
    do {
        v_check = false;

        for (i = ChessBoardWidth;i>0;i--)
            for (j = ChessBoardWidth; j>0; j--)
            {
                if (BallExploration[i][j] == 0)
                    {
                        if ((j+1<=ChessBoardWidth) && (BallExploration[i][j+1] == 0) && (tmpShortPath[i][j+1]>tmpShortPath[i][j]+1))
                        {

                              v_check = true;
                             tmpShortPath[i][j+1] = tmpShortPath[i][j]+1 ;
                        }
                        if ((j-1>0) && (BallExploration[i][j-1] == 0) && (tmpShortPath[i][j-1]>tmpShortPath[i][j]+1))
                        {

                              v_check = true;
                             tmpShortPath[i][j-1] = tmpShortPath[i][j]+1 ;
                        }

                        if ((i-1>0) && (BallExploration[i-1][j] == 0) && (tmpShortPath[i-1][j]>tmpShortPath[i][j]+1))
                        {

                              v_check = true;
                             tmpShortPath[i-1][j] = tmpShortPath[i][j]+1 ;
                        }
                        if ((i+1<=ChessBoardWidth) && (BallExploration[i+1][j] == 0) && (tmpShortPath[i+1][j]>tmpShortPath[i][j]+1))
                        {

                              v_check = true;
                             tmpShortPath[i+1][j] = tmpShortPath[i][j]+1 ;
                        }
                    }

            }
    } while (v_check);


    //Return steps need to take from_pos to to_pos
    return tmpShortPath[to_pos.horizontal][to_pos.vertical];
}

Position get_to_closest_pos(std::vector<Position> vectorPositions,Position start_pos)
{

    int v_steps,min_steps;
    Position v_pos;

    min_steps = SHRT_MAX;

    Position finish_pos;

    v_pos.horizontal = 0;
    v_pos.vertical = 0;

    for ( int i =0; i<vectorPositions.size(); i++)
    {
        finish_pos = vectorPositions.at(i);
        if ((finish_pos.horizontal != start_pos.horizontal) || (finish_pos.vertical != start_pos.vertical))
        {
                v_steps = get_min_step(finish_pos,start_pos); //***** Very important: in the reverse order finish_pos and start_pos *****

                if (v_steps<min_steps)
                {
                    min_steps = v_steps;
                    v_pos = finish_pos;

                    //remember travel path
                    for (int i=0;i<ChessBoardWidth+1;i++)
                        for (int j=0;j<ChessBoardWidth+1;j++)
                            {
                                travelPath[i][j] = tmpShortPath[i][j];
                            }
                }
        }
    }
    return v_pos; //the closest position

}

void travel_from_pos_to_pos(Position from_pos,Position *to_pos)
{
    //Get travel path

//    cout << from_pos.horizontal<<from_pos.vertical << "from_pos \n";
//    cout << to_pos.horizontal<<to_pos.vertical << "to_pos \n";


    int v_v, v_h;
    Position travelPos = from_pos;
    int i = 1;
    int j;



//     for (i=1;i<ChessBoardWidth+1;i++)
//        {
//            cout << endl;
//
//            for (j=1;j<ChessBoardWidth+1;j++)
//                {
//                    if (travelPath[i][j]<0) cout << "0 ";
//                    else
//                    cout << travelPath[i][j] << " ";
//                }
//        }




    int StepsCount = travelPath[from_pos.horizontal] [from_pos.vertical];
    i = 1;
    while (StepsCount+1 >= i)
        {
           TravelArray[i] =  travelPos;
           v_v = travelPos.vertical;
           v_h = travelPos.horizontal;
           if (travelPath[v_h][v_v+1] == travelPath[v_h][v_v]-1)
                travelPos.vertical++;
                else if (travelPath[v_h][v_v-1] == travelPath[v_h][v_v]-1)
                        travelPos.vertical--;
                        else if (travelPath[v_h+1][v_v] == travelPath[v_h][v_v]-1)
                                travelPos.horizontal++;
                                else if (travelPath[v_h-1][v_v] == travelPath[v_h][v_v]-1)
                                        travelPos.horizontal--;
           i++;
        }






    //Make the actual run from the TravelArray info
    travelPos.Direction = from_pos.Direction; //start direction
    travelPos.horizontal = from_pos.horizontal;
    travelPos.vertical = from_pos.vertical;

    int v_travelpos = 1; //from the first item in the TravelArray
     char v_turn;
    int v_steps;
    string str;

     while ((travelPos.horizontal != to_pos->horizontal) || (travelPos.vertical != to_pos->vertical))
     //   for (int i=1;i<10;i++)
        {
        //turn the car
        v_turn = get_next_direction(v_travelpos,travelPos.Direction);

        std::cout << "turn the car " << v_turn << endl;

        //Physical turn the car
        if (v_turn != 'F')
            {
                char *pChar = &v_turn;
                Command_Data(pChar,500);
            }

         v_steps = GetNumberOfSteps(v_travelpos);


        //run the car v_steps
        std::cout << "Number of steps: " << v_steps << endl;

            std::stringstream ss;
            ss << travelPos.Direction << v_turn;
            str = ss.str();

            //cout << " before dir: " << travelPos.Direction << " str: " << str << endl;

            if ((str == "EL") || (str == "WR")) travelPos.Direction = 'N';
            if ((str == "WL") || (str == "ER")) travelPos.Direction = 'S';
            if ((str == "SL") || (str == "NR")) travelPos.Direction = 'E';
            if ((str == "NL") || (str == "SR")) travelPos.Direction = 'W';

               //Run the car v_steps

            char c_steps[10];
            snprintf(c_steps, sizeof c_steps, "%d", v_steps);
			puts(c_steps);



            cout << c_steps;
            Command_Data(c_steps,500);
            setCurrentTravel(&travelPos,v_travelpos + v_steps);
            v_travelpos = v_travelpos + v_steps ;

            cout << "current position" << travelPos.horizontal << "," << travelPos.vertical << endl ;


        }

         to_pos->Direction = travelPos.Direction; //set to_pos direction

}
int GetNumberOfSteps ( int v_travelPos)
{
    int i =1;
    Position v_pos = TravelArray[v_travelPos];

    if (v_pos.horizontal == TravelArray[v_travelPos+i].horizontal)
    {
        while (v_pos.horizontal == TravelArray[v_travelPos+i].horizontal) {i++;};
    }
    else
    {
        while (v_pos.vertical == TravelArray[v_travelPos+i].vertical) {i++;};
    }

    return i-1;

}

char get_next_direction (int v_pos, char v_dir)
{
     Position leftPos,rightPos;

     TravelArray[v_pos].Direction = v_dir;
     leftPos = getPos(TravelArray[v_pos],'L');
     rightPos = getPos(TravelArray[v_pos],'R');

             if ((leftPos.horizontal== TravelArray[v_pos+1].horizontal )
                && (leftPos.vertical== TravelArray[v_pos+1].vertical ))
                return 'L';

             if ((rightPos.horizontal== TravelArray[v_pos+1].horizontal )
                && (rightPos.vertical== TravelArray[v_pos+1].vertical ))
                return 'R';
return 'F';
}

void setCurrentTravel(Position *v_currentPos,int i)
{
    TravelArray[i].Direction = v_currentPos->Direction;
    v_currentPos->horizontal = TravelArray[i].horizontal;
    v_currentPos->vertical = TravelArray[i].vertical;
}


void GetShortestPath()
{
    int i, j;
    bool v_check;

//set values for destination. no-ball and value of path is 1
ShortPath[ChessBoardWidth][ChessBoardWidth] = 1;
BallsArray[ChessBoardWidth][ChessBoardWidth] = 0;


//Calculate the shortest matrix
    do {
        v_check = false;

        for (i = ChessBoardWidth;i>0;i--)
            for (j = ChessBoardWidth; j>0; j--)
            {
                if (BallsArray[i][j] == 0)
                    {
                        if ((j+1<ChessBoardWidth) && (BallsArray[i][j+1] == 0) && (ShortPath[i][j+1]>ShortPath[i][j]+1))
                        {

                              v_check = true;
                             ShortPath[i][j+1] = ShortPath[i][j]+1 ;
                        }
                        if ((j-1>0) && (BallsArray[i][j-1] == 0) && (ShortPath[i][j-1]>ShortPath[i][j]+1))
                        {

                              v_check = true;
                             ShortPath[i][j-1] = ShortPath[i][j]+1 ;
                        }

                        if ((i-1>0) && (BallsArray[i-1][j] == 0) && (ShortPath[i-1][j]>ShortPath[i][j]+1))
                        {

                              v_check = true;
                             ShortPath[i-1][j] = ShortPath[i][j]+1 ;
                        }
                        if ((i+1<ChessBoardWidth) && (BallsArray[i+1][j] == 0) && (ShortPath[i+1][j]>ShortPath[i][j]+1))
                        {

                              v_check = true;
                             ShortPath[i+1][j] = ShortPath[i][j]+1 ;
                        }
                    }

            }
    } while (v_check);


 // get the travel array

int StepsCount = ShortPath[StartPos.horizontal] [ StartPos.vertical ];
int v_v, v_h;
Position travelPos = StartPos;
i = 1;

while (StepsCount+1 > i)
    {
       TravelArray[i] =  travelPos;
       v_v = travelPos.vertical;
       v_h = travelPos.horizontal;
       if (ShortPath[v_h][v_v+1] == ShortPath[v_h][v_v]-1)
            travelPos.vertical++;
            else if (ShortPath[v_h][v_v-1] == ShortPath[v_h][v_v]-1)
                    travelPos.vertical--;
                    else if (ShortPath[v_h+1][v_v] == ShortPath[v_h][v_v]-1)
                            travelPos.horizontal++;
                            else if (ShortPath[v_h-1][v_v] == ShortPath[v_h][v_v]-1)
                                    travelPos.horizontal--;
       i++;
    }

    //Print out list of positions
    for (i=1;i<=StepsCount;i++)
        {
            cout << "[" << TravelArray[i].horizontal << "," << TravelArray[i].vertical << "]" <<endl;
        }
}

void PrintBoard()
{
     int i, j;
        for (i=1;i<ChessBoardWidth+1;i++)
        {
            cout << endl;

            for (j=1;j<ChessBoardWidth+1;j++)
                {
                    if (BallsArray[i][j]<0) cout << "0 ";
                    else
                    cout << BallsArray[i][j] << " ";
                }
        }
        cout << endl;
}


void dataInitialize()
{

      //Connect to the COM port
     if (SP->IsConnected())
		cout << "Connected successfully to the xbee.\n";


      for (int i=0;i<ChessBoardWidth+1;i++)
        for (int j=0;j<ChessBoardWidth+1;j++)
        {
            BallExploration[i][j] = -1;
        }

    //set the first position avaiable
    BallExploration[1][1]=0;
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
bool checkBoundary(Position v_Pos)
{
    //Check if position is outside of the board. will return false
    if ((v_Pos.horizontal<=0) || (v_Pos.vertical <=0) || (v_Pos.horizontal>ChessBoardWidth) || (v_Pos.vertical>ChessBoardWidth))
        return false;

    return true;
}


bool checkBall(Position v_currPos, char v_sensor_data) //-1 not check;1 block; 0 free to go
{
    Position frontPos;
    frontPos = getPos(v_currPos,'F');
    if (v_sensor_data == '1')
    {

        return true;
    }
    //has a ball in front
    if (BallsArray [frontPos.horizontal][frontPos.vertical]==1)
        return true;


    //Free to go
    return false;

}

int checkNoBallinFrontForTest(Position v_currPos) //-1 not check;1 block; 0 free to go
{
    Position frontPos;
    frontPos = getPos(v_currPos,'F');
    if (BallsArray [frontPos.horizontal][frontPos.vertical] == -1)
    {
        BallsArray [frontPos.horizontal][frontPos.vertical] = 0; //testing purpose
    }
    return BallsArray [frontPos.horizontal][frontPos.vertical];
}

void blockPosition(Position v_Pos)
{
    BallsArray[v_Pos.horizontal][v_Pos.vertical] = 1; //set v_pos as a block position
}

bool checkBlockPosition(Position v_Pos)
{
    return BallsArray[v_Pos.horizontal][v_Pos.vertical] == 1;
}

char GetMinValue (Position v_currPos)
{
    int v_min = 36000; //number of visits
    int v_min_time = 36000;
    char v_dir;
    Position leftPos, rightPos, backPos,frontPos;

    leftPos = getPos(v_currPos,'L');
    rightPos = getPos(v_currPos,'R');
    backPos = getPos(v_currPos,'B');
    frontPos = getPos(v_currPos,'F');

    //std::cout << "back block: "<< checkBlockPosition(backPos)<< std::endl;
    //std::cout << "back boundary: "<< checkBoundary(backPos)<< std::endl;
    // std::cout << "value: "<< VisitsArray[backPos.horizontal][backPos.vertical]<< std::endl;

    if ((!checkBlockPosition(backPos)) && (checkBoundary(backPos)))
    {
        if (VisitsArray[backPos.horizontal][backPos.vertical] < v_min)
            {

            v_min_time = VisitsArrayTime[backPos.horizontal][backPos.vertical];
            v_min = VisitsArray[backPos.horizontal][backPos.vertical];
            v_dir = 'B';
            }
        else
            if
             ((VisitsArray[backPos.horizontal][backPos.vertical] == v_min)
             && (VisitsArrayTime[backPos.horizontal][backPos.vertical] < v_min_time))
             {
                    v_min = VisitsArray[backPos.horizontal][backPos.vertical];
                    v_min_time = VisitsArrayTime[backPos.horizontal][backPos.vertical];
                    v_dir = 'B';
             }
    }

    if ((!checkBlockPosition(rightPos)) &&(checkBoundary(rightPos)))
            {
                if (VisitsArray[rightPos.horizontal][rightPos.vertical] < v_min)
                {
                        v_min_time = VisitsArrayTime[rightPos.horizontal][rightPos.vertical];
                        v_min = VisitsArray[rightPos.horizontal][rightPos.vertical];
                        v_dir = 'R';
                }
                else
                    if
                     ((VisitsArray[rightPos.horizontal][rightPos.vertical] == v_min)
                    && (VisitsArrayTime[rightPos.horizontal][rightPos.vertical] < v_min_time))
                {
                        v_min_time = VisitsArrayTime[rightPos.horizontal][rightPos.vertical];
                        v_min = VisitsArray[rightPos.horizontal][rightPos.vertical];
                        v_dir = 'R';
                }
            }

    if ((!checkBlockPosition(leftPos)) && (checkBoundary(leftPos)))

            {
                if (VisitsArray[leftPos.horizontal][leftPos.vertical] < v_min)
                {
                        v_min_time = VisitsArrayTime[leftPos.horizontal][leftPos.vertical];
                        v_min = VisitsArray[leftPos.horizontal][leftPos.vertical];
                        v_dir = 'L';
                }
                else
                    if
                     ((VisitsArray[leftPos.horizontal][leftPos.vertical] == v_min)
                    && (VisitsArrayTime[leftPos.horizontal][leftPos.vertical] < v_min_time))
                {
                        v_min_time = VisitsArrayTime[leftPos.horizontal][leftPos.vertical];
                        v_min = VisitsArray[leftPos.horizontal][leftPos.vertical];
                        v_dir = 'L';
                }

            }

    if ((!checkBlockPosition(frontPos)) &&(checkBoundary(frontPos)))
        {

          if (VisitsArray[frontPos.horizontal][frontPos.vertical] < v_min)
                {
                        v_min_time = VisitsArrayTime[frontPos.horizontal][frontPos.vertical];
                        v_min = VisitsArray[frontPos.horizontal][frontPos.vertical];
                        v_dir = 'F';
                }
                else
                    if
                     ((VisitsArray[frontPos.horizontal][frontPos.vertical] == v_min)
                    && (VisitsArrayTime[frontPos.horizontal][frontPos.vertical] < v_min_time))
                {
                        v_min_time = VisitsArrayTime[frontPos.horizontal][frontPos.vertical];
                        v_min = VisitsArray[frontPos.horizontal][frontPos.vertical];
                        v_dir = 'F';
                }
        }

    return v_dir;
}

void setVisit (Position *v_Pos)
{
 BallsArray[v_Pos->horizontal][v_Pos->vertical] = 0;
}
void GoForward(Position *v_currPos)
{
    VisitsArray[v_currPos->horizontal][v_currPos->vertical]++;
    BallsArray[v_currPos->horizontal][v_currPos->vertical] = 0;
    VisitTimeCount ++;
    VisitsArrayTime[v_currPos->horizontal][v_currPos->vertical] = VisitTimeCount;
    Position tmpPos;

    tmpPos.Direction = v_currPos->Direction;
    tmpPos.vertical = v_currPos->vertical;
    tmpPos.horizontal = v_currPos->horizontal;

    tmpPos = getPos(tmpPos,'F');

    v_currPos->Direction = tmpPos.Direction;
    v_currPos->horizontal = tmpPos.horizontal;
    v_currPos->vertical = tmpPos.vertical;
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


