//
// Created by crbaniak on 10/17/18.
//

#include "ass4.h"

void getProcessDuty();

int main(int argc, char* argv[]){

    //init shared mem
    sharedMemoryConfig();

    //check if scheduled
    while(msgQueuePtr->pid != getpid()){}

    //get duty of process to perform
    getProcessDuty();

    // do work

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
            PCBshmPtr[0]->timeWorkingNanoseconds = 0;
            break;

        case '1':
            printf("Child has duty 1\n");
            PCBshmPtr[0]->timeWorkingNanoseconds = TIME_QUANTUM;
            break;

        case '2':
            printf("Child has duty 2\n");
            PCBshmPtr[0]->timeWorkingSeconds = rand() % 5;
            PCBshmPtr[0]->timeWorkingNanoseconds = (rand() % 1000) * 1000000;
            break;

        case '3':
            printf("Child has duty 3\n");
            float percent = (rand() % 100);
            percent /= 100;
            PCBshmPtr[0]->timeWorkingNanoseconds = percent * TIME_QUANTUM;
            break;

            // operator is doesn't match any case constan
        default:
            printf("Error! operator is not correct");
    }
};
