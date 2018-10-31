//
// Created by crbaniak on 10/17/18.
//

#include "ass4.h"

void getProcessDuty();

int main(int argc, char* argv[]){

    //init shared mem
    sharedMemoryConfig();


    //check if scheduled


    //get duty of process to perform
    getProcessDuty();

    // do work

    // update pcd with end times
    PCBshmPtr[0]->endCPUTimeNanoseconds = sysClockshmPtr->nanoseconds;
    PCBshmPtr[0]->endCPUTimeSeconds = sysClockshmPtr->seconds;


    // send back to queue


    // flag for process complete
    PCBshmPtr[0]->complete = 1;


    // flip semaphore/round robin back to oss


    // clean shared mem
    shmdt(sysClockshmPtr);
    shmdt(PCBshmPtr);
    return 0;
}

void getProcessDuty(){

    int processDuty = 0;
    char str[1] = {'0'};
    time_t t;

    // init clock the clock
    srand((unsigned) time(&t));

    //create a random number between 0-3
    //  0: process terminates
    //  1: process terminates at time quantum
    //  2: process waits for event taking rr.ss seconds
    //  3: process runs for random number P for P percent of time quantum
    processDuty = rand() % (3 + 1 - 0) + 0;
    // convert int to string for switch
    // sprintf(str, "%d", processDuty);


    switch(str[0])
    {
        case '0':
            printf("Child has duty 0\n");

            break;

        case '1':
            printf("Child has duty 1\n");
            break;

        case '2':
            printf("Child has duty 2\n");
            break;

        case '3':
            printf("Child has duty 3\n");
            break;

            // operator is doesn't match any case constant (+, -, *, /)
        default:
            printf("Error! operator is not correct");
    }
};
