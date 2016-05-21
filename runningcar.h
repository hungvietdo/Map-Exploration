#include <iostream>
#include <tchar.h>
#include <time.h>
#include <string>
#include <sstream>
#include "Serial.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#define MAX_LEN 255

#define  carWidth 30
#define  carLength  50
#define  chessWidth 90

#define SleepTime 50

#define ChessBoardWidth 5
#define FinishX 5
#define FinishY 5
#define StartX 1
#define StartY 1
#define StartDir 'S'



 struct Position
{
    int vertical;
    int horizontal;
    char Ver_Hor;
    char Direction;
};
 struct Direction
{
    //North, South, East, West
    char direction;

};



void GetShortestPath();
int GetNumberOfSteps (int v_travelPos);
char get_next_direction (int v_pos, char v_dir);
void setCurrentTravel(Position *v_currentPos,int i);
void PrintBoard();
void GoForward(Position *v_currPos);
char GetMinValue (Position v_currPos);


int CalDistance(Position v_Pos,Position v_FinishPos);
void setVisit (Position *v_Pos);
char GetPosNotChecked(Position v_currPos,Direction v_dir);
void TurnTheCar(Position *v_currPos, char v_turn); //turn_to = 'L', 'R','B'
bool checkBall(Position v_currPos, char v_sensor_data);
int checkNoBallinFrontForTest(Position v_currPos);
void blockPosition(Position v_Pos); //set lock the this position
void dataInitialize();
Position getPos(Position v_Pos,char v_side);
bool checkBoundary(Position v_Pos);
char GetPosNotChecked(Position v_currPos);
bool checkFinishPos (Position v_Pos);
bool checkBlockPosition(Position v_Pos);



void Command_Data (char *ch, int sleeptime);
char Command_Sensor(char *ch);



bool checking_pos(Position frontPos);
int get_min_step(Position from_pos,Position to_pos);
Position get_to_closest_pos(std::vector<Position> vectorPositions,Position start_pos);
void travel_from_pos_to_pos(Position from_pos,Position to_pos);

