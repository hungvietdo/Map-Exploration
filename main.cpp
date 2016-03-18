#include "runningcar.h"
using namespace std;

Serial* SP = new Serial("\\\\.\\COM3");    // adjust as needed

char sensor_info;
int step_count =0;

void step2_shortpath();
void step1_getmapinfo(Position CurrPosCar);
void Command_Data (char *ch, int sleeptime);
char Command_Sensor(char *ch);

int main(int argc, char**argv)
{
    //init data
    dataInitialize();

    if (SP->IsConnected())
		cout << "Connected successfully to the xbee.\n";

    //set a start position
    Position CurrPosCar;
    CurrPosCar.Direction = StartDir;
    CurrPosCar.horizontal = StartX;
    CurrPosCar.vertical = StartY;

    int input;
    for(;;)
    {
        printf("\n\n");
        printf("===============================================\n");
        printf( "1. Find a path.\n" );
        printf( "2. Use the shortest path to the destination.\n" );
        printf( "3. Learn more about the map.\n" );
        printf( "4. Exit\n" );
        printf("===============================================\n");
        printf( "Get the car ready and make a selection: " );
        scanf( "%d", &input );
        switch ( input ) {
            case 1:            /* Note the colon, not a semicolon */
                step1_getmapinfo(CurrPosCar);
                break;
            case 2:
                step2_shortpath();
                break;
            case 3:
                printf( "Function is not available yet!\n" );
                break;
            case 4:
                printf( "Thanks for riding!\n" );
                return 0;
            default:
                printf( "Bad input, quitting!\n" );
                break;
        }
        getchar();
    }
}

void step1_getmapinfo(Position CurrPosCar)
{
    char v_PosNotChecked, v_MinPos;
    Position fromPos;

    //set the first position as a visited
    setVisit(&CurrPosCar);

        while (!checkFinishPos(CurrPosCar))
        {
            PrintBoard();
            //Get a position that has not been checked before value = -1
            v_PosNotChecked = GetPosNotChecked(CurrPosCar); //F,L, R, B,
            if (v_PosNotChecked != 'O') //there is a position near-by have not been visited
            {
                fromPos = CurrPosCar;
                TurnTheCar(&CurrPosCar,v_PosNotChecked);

                //**************Car control************************
                //If not 'Forward' command we have to turn the car in {R, or L, or B}
                if (v_PosNotChecked != 'F')
                    {
                        //Physical turn the car
                        char *pChar = &v_PosNotChecked;
                        Command_Data(pChar,500);
                    }

                //Send a sensor signal to the car. Return value is a char with 0 or 1
                sensor_info = Command_Sensor("S");

                //**************Car control************************
                if (!checkBall(CurrPosCar,sensor_info)) //False, no ball, free go
                    {
                        GoForward(&CurrPosCar);
                        //**************Car control************************
                        //Physical: go forward
                        Command_Data("F",1000);
                        //**************end Car control************************
                    }
                else //has a ball in front
                    {
                        //tmpPos = getPos(CurrPosCar,'F');
                        blockPosition(getPos(CurrPosCar,'F')); //set blocked to the front position
                    }
            }
        else //near-by positions have been visited. Now select a lowest visit
            {

                    v_MinPos = GetMinValue (CurrPosCar); //L, R, F, B
                    fromPos = CurrPosCar;
                    TurnTheCar(&CurrPosCar,v_MinPos);
                    //**************Car control************************
                    //Physical: Turn the car if needed
                    if (v_MinPos != 'F')
                    {
                        char *pChar = &v_MinPos;
                         Command_Data(pChar,500);  //turn the car

                    }
                    GoForward(&CurrPosCar);
                    //Physical: Go forward
                    Command_Data("F",500);
                    step_count++;
                    cout << "done go forward for " << step_count << " steps." << endl;
            }
        }

        std::cout << "Reach destination.";
    //**************end Car control************************
}

void step2_shortpath()
{
    PrintBoard();

    GetShortestPath();



    Position TravelPos;
    char v_turn;
    string str;

    int v_steps;

    int v_travelpos = 1;

    TravelPos.Direction = StartDir; //start direction
    TravelPos.horizontal = StartX;
    TravelPos.vertical = StartY;

     while (!checkFinishPos(TravelPos))
     //   for (int i=1;i<10;i++)
        {
        //turn the car
        v_turn = get_next_direction(v_travelpos,TravelPos.Direction);

        cout << "turn the car " << v_turn << endl;

        if (v_turn != 'F')
            {
                //Physical turn the car
                char *pChar = &v_turn;
                Command_Data(pChar,500);
            }

         v_steps = GetNumberOfSteps(v_travelpos);
        //run the car v_steps
        cout << "Number of steps: " << v_steps << endl;

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
