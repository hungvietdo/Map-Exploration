#include <iostream>
#include <vector>
#include <stdio.h>
#include <limits.h>
#include "runningcar.h"
using namespace std;

std::vector<Position> vectorPositions; //array to store all postions neeed to check
void map_exploration();
void remove_pos(Position v_pos);
bool not_in_the_list(Position v_pos);

int main(int argc, char**argv)
{

 map_exploration();

 return 0;

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


void map_exploration()
{
    Position t_pos;
    Position leftPos, rightPos, backPos,frontPos;
    char sensor_info;

    //initiate the ball_array; -1 mean this pos is not check yet
   dataInitialize();

    //add the first pos to the list
    Position v_currPos ;
    v_currPos.Direction ='S';
    v_currPos.horizontal = 1;
    v_currPos.vertical = 1;
    vectorPositions.push_back(v_currPos);

    while (vectorPositions.size()>0)
    {
            leftPos = getPos(v_currPos,'L');
            rightPos = getPos(v_currPos,'R');
            backPos = getPos(v_currPos,'B');
            frontPos = getPos(v_currPos,'F');

            if (not_in_the_list(frontPos))
            {
                //cout<<"o day";
                if (checking_pos(frontPos))
                {
                    //cout << frontPos.horizontal<<frontPos.vertical << "\n";
                    vectorPositions.push_back(frontPos);
                }
            }

//            //Check right position
//            if (not_in_the_list(rightPos))
//                {
//                    //Turn car to the right
//                    TurnTheCar(&v_currPos,'R');
//
//
//                    if (checking_pos(rightPos))
//                    vectorPositions.push_back(rightPos);
//                }
//
            //Check left position
            if (not_in_the_list(leftPos))
                {
                    //Turn car to the left
                    //Physical Turn
                    Command_Data("L",1000);

                    //Logical turn (data)
                    TurnTheCar(&v_currPos,'L');

                    if (checking_pos(leftPos))
                    vectorPositions.push_back(leftPos);
                }

            //Which position to check next
            //Find closest position from the list
            t_pos = get_to_closest_pos(vectorPositions,v_currPos);
            if (t_pos.horizontal !=0)
            {
                //travel to that position
                travel_from_pos_to_pos(v_currPos,&t_pos);
            }


            //remove v_currentPos out of the list;
            remove_pos(v_currPos);

            //update currentPos;
            v_currPos = t_pos;

    }
    return;
}
