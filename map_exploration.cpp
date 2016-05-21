#include <iostream>
#include <vector>
#include <stdio.h>
#include <limits.h>
#include "runningcar.h"
using namespace std;

std::vector<Position> vectorPositions; //array to store all postions neeed to check
int BallExploration[ChessBoardWidth][ChessBoardWidth];
int travelPath[ChessBoardWidth][ChessBoardWidth];
int tmpShortPath[ChessBoardWidth][ChessBoardWidth];
Serial* SP = new Serial("\\\\.\\COM3");    // adjust as needed
void Command_Data (char *ch, int sleeptime);
char Command_Sensor(char *ch);


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

bool not_in_the_list(Position v_pos)
{
    Position it;
     for ( int i =0; i<vectorPositions.size(); ++i)
    {
        it = vectorPositions.at(i);
        if ((it.horizontal == v_pos.horizontal) && (it.vertical == v_pos.vertical)) //check if input position is the one in iterator
            {
                return false; //the v_pos is in the list
            }
    }
    return true; //v_pos is not in the list
}

void checking_pos(Position frontPos)
{
    if (checkBoundary(frontPos))
            {
                //Send a sensor signal to the car. Return value is a char with 0 or 1
                char sensor_info = Command_Sensor("S");

                if ((sensor_info == '1') || (BallExploration[frontPos.horizontal][frontPos.vertical] ==1)) //check sensor and data before
                    {
                    //add frontPos to the list
                        vectorPositions.push_back(frontPos);
                        BallExploration[frontPos.horizontal][frontPos.vertical] = 0; //free position
                    }
                else
                {
                    BallExploration[frontPos.horizontal][frontPos.vertical] = 1; //blocked position
                }
            }

}

int get_min_step(Position from_pos,Position to_pos)
{
    int i,j;
    bool v_check;


    for (i=1;i<ChessBoardWidth+1;i++)
        for (j=1;j<ChessBoardWidth+1;j++)
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
                        if ((j+1<ChessBoardWidth) && (BallExploration[i][j+1] == 0) && (tmpShortPath[i][j+1]>tmpShortPath[i][j]+1))
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
                        if ((i+1<ChessBoardWidth) && (BallExploration[i+1][j] == 0) && (tmpShortPath[i+1][j]>tmpShortPath[i][j]+1))
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

Position get_to_closest_pos(Position start_pos)
{

    int v_steps,min_steps;
    Position v_pos;

    min_steps = SHRT_MAX;

    Position finish_pos;
    for ( int i =0; i<vectorPositions.size(); ++i)
    {
        finish_pos = vectorPositions.at(i);
        v_steps = get_min_step(finish_pos,start_pos); //***** Very important: in the reverse order finish_pos and start_pos *****

        if (v_steps<min_steps)
        {
            min_steps = v_steps;
            v_pos = finish_pos;

            //remember travel path
            for (int i=1;i<ChessBoardWidth+1;i++)
                for (int j=1;j<ChessBoardWidth+1;j++)
                    {
                        travelPath[i][j] = tmpShortPath[i][j];
                    }
        }
    }
    return v_pos; //the closest position

}

void travel_from_pos_to_pos(Position from_pos,Position to_pos)
{
    //Get travel path

    int StepsCount = travelPath[from_pos.horizontal] [ from_pos.vertical ];
    int v_v, v_h;
    Position travelPos = from_pos;
    int i = 1;
    Position TravelPos;
    char v_turn;
    int v_travelpos = 1;
    int v_steps;
    string str;

    while (StepsCount+1 > i)
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
    TravelPos.Direction = from_pos.Direction; //start direction
    TravelPos.horizontal = from_pos.horizontal;
    TravelPos.vertical = from_pos.vertical;

     while ((TravelPos.horizontal != to_pos.horizontal) || (TravelPos.horizontal != to_pos.horizontal))
     //   for (int i=1;i<10;i++)
        {
        //turn the car
        v_turn = get_next_direction(v_travelpos,TravelPos.Direction);

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
            ss << TravelPos.Direction << v_turn;
            str = ss.str();

            //cout << " before dir: " << TravelPos.Direction << " str: " << str << endl;

            if ((str == "EL") || (str == "WR")) TravelPos.Direction = 'N';
            if ((str == "WL") || (str == "ER")) TravelPos.Direction = 'S';
            if ((str == "SL") || (str == "NR")) TravelPos.Direction = 'E';
            if ((str == "NL") || (str == "SR")) TravelPos.Direction = 'W';

               //Run the car v_steps

            char c_steps[10];
            snprintf(c_steps, sizeof c_steps, "%d", v_steps);
			puts(c_steps);

            cout << c_steps;
            Command_Data(c_steps,500);
            setCurrentTravel(&TravelPos,v_travelpos + v_steps);
            v_travelpos = v_travelpos + v_steps ;

            cout << "current position" << TravelPos.horizontal << "," << TravelPos.vertical << endl ;


        }


}

void map_exploration()
{
    Position t_pos;
    Position leftPos, rightPos, backPos,frontPos;
    char sensor_info;

    //Connect to the COM port
     if (SP->IsConnected())
		cout << "Connected successfully to the xbee.\n";

    //initiate the ball_array; -1 mean this pos is not check yet
     for (int i=1;i<ChessBoardWidth+1;i++)
        for (int j=1;j<ChessBoardWidth+1;j++)
        {
            BallExploration[i][j] = -1;
        }

    //add the first pos to the list
    Position v_currPos {1,1,'S'};
    vectorPositions.push_back(v_currPos);

    while (vectorPositions.size()>0)
    {
            leftPos = getPos(v_currPos,'L');
            rightPos = getPos(v_currPos,'R');
            backPos = getPos(v_currPos,'B');
            frontPos = getPos(v_currPos,'F');

            //Check front
            if (not_in_the_list(frontPos))
            {
                checking_pos(frontPos);
            }

            //Check right position
            if (not_in_the_list(rightPos))
                {
                    //Turn car to the right
                    TurnTheCar(&v_currPos,'R');
                    checking_pos(rightPos);
                }

            //Check left position
            if (not_in_the_list(leftPos))
                {
                    //Turn car to the left
                    TurnTheCar(&v_currPos,'L');
                    checking_pos(leftPos);
                }

            //Which position to check next
            //Find closest position from the list
            t_pos = get_to_closest_pos(v_currPos);

            //travel to that position
            travel_from_pos_to_pos(v_currPos,t_pos);

            //remove v_currentPos out of the list;
            remove_pos(v_currPos);

            //update currentPos;
            v_currPos = t_pos;

    }
    return;
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
