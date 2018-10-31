//
// Created by crbaniak on 10/26/18.
//

#include "ass4.h"

void sigint(int);
void ossClock();
void forkIfSecondPassed();
void initForkToPCB();
void roundRobinSchedule();
void writeResultsToLog();

int oneSecHappened = 0;

int main(int argc, char* argv[]){

    int clockStop = 0;      // flag for stopping OS clock

    // init clock to random
    time_t t;
    srand((unsigned) time(&t));

    //signal handling config - ctrl-c
    signal(SIGINT, sigint);

    // configure shared memory
    sharedMemoryConfig();

    // os clock
    while(clockStop == 0){

        //increments clock and adjusts sec:nanos
        ossClock();

        //if one second happens, for a child
        forkIfSecondPassed();

        //round robin scheduler
        roundRobinSchedule();

        // if process completes, write data to log
        writeResultsToLog();
    }

    // clean shared memory
    shmdt(sysClockshmPtr);
    return 0;
}

// signal handles ctrl-c
void sigint(int a) {
    printf("^C caught\n");
    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(PCBshmPtr);
    shmctl(PCBshmid, IPC_RMID, NULL);
    exit(0);
}

//increments the clock random value and adjust sec:nanos
void ossClock(){

    int clockIncrement;     // random number to increment time
    int rollover;           // rollover nanos

    clockIncrement = (unsigned int) (rand() % 100000000 + 1);

    if ((sysClockshmPtr->nanoseconds + clockIncrement) > 999999999){
        rollover = (sysClockshmPtr->nanoseconds + clockIncrement) - 999999999;
        sysClockshmPtr->seconds += 1;
        sysClockshmPtr->nanoseconds = rollover;
        oneSecHappened = 1;
    } else {
        sysClockshmPtr->nanoseconds += clockIncrement;
    }

    printf("%d\n", sysClockshmPtr->seconds);
    printf("%d\n", sysClockshmPtr->nanoseconds);

    sleep(1);
}

//if one second happens, for a child
void forkIfSecondPassed(){
    if(oneSecHappened == 1){
        //init forks to PCB
        initForkToPCB();
        if ((PCBshmPtr[0]->pidHolder = fork()) == 0)
            execl("./user", "user", NULL);
        oneSecHappened--;
    }
}

//init forks to PCB
void initForkToPCB(){

    PCBshmPtr[0]->startCPUTimeNanoseconds = sysClockshmPtr->nanoseconds;
    PCBshmPtr[0]->startCPUTimeSeconds = sysClockshmPtr->seconds;

    FILE *fp = fopen("log.txt", "a+");
    fprintf(fp, "Generating process with PID %d (priority),"
                "and putting it in queue (number) at time "
                "%d  %d \n", PCBshmPtr[0]->pidHolder, PCBshmPtr[0]->startCPUTimeSeconds, PCBshmPtr[0]->startCPUTimeNanoseconds);
    fclose(fp);
}

//round robin scheduler
void roundRobinSchedule(){

}

// if process completes, write data to log
void writeResultsToLog(){
    if(PCBshmPtr[0]->complete == 1) {
        PCBshmPtr[0]->totalCPUTimeSeconds = PCBshmPtr[0]->endCPUTimeSeconds - PCBshmPtr[0]->totalCPUTimeSeconds;
        PCBshmPtr[0]->complete = 0;
        FILE *fp = fopen("log.txt", "a+");
        fprintf(fp, "Finished process with PID %d at time %d %d\n",
                PCBshmPtr[0]->pidHolder, PCBshmPtr[0]->totalCPUTimeSeconds, PCBshmPtr[0]->totalCPUTimeSeconds);
        fclose(fp);
    }
}

