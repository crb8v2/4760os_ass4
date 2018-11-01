//
// Created by crbaniak on 10/26/18.
//

#include "ass4.h"

void sigint(int);
void ossClock();
void forkIfSecondPassed();
void initForkToPCB(pid_t);
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

    clockIncrement = (unsigned int) (rand() % 600000000 + 1);

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
    if(oneSecHappened == 1) {
        pid_t holder;
        //init forks to PCB and writes to log
        if ((holder = fork()) == 0){
            execl("./user", "user", NULL);
        }

        PCBshmPtr[0]->pidHolder = holder;

        initForkToPCB(holder);
        oneSecHappened--;
    }
}

//init forks to PCB and writes to log
void initForkToPCB(pid_t holder){

    // random creation time between 0-2 seconds
    PCBshmPtr[0]->timeCreateSeconds = rand() % 2;
    PCBshmPtr[0]->timeCreateNanoseconds = (rand() % 99) * 10000000;

    FILE *fp = fopen("log.txt", "a+");
    fprintf(fp, "Generating process with PID %d (priority),"
                "and putting it in queue (number) at time "
                "%d : %d \n", holder, sysClockshmPtr->seconds, sysClockshmPtr[0].nanoseconds);
    fclose(fp);

}

//round robin scheduler
void roundRobinSchedule(){

    if(sysClockshmPtr->seconds == 3){
        msgQueuePtr->pid = PCBshmPtr[0]->pidHolder;
    }

}

// if process completes, write data to log
void writeResultsToLog(){

    int rollover; //for clock

    if(PCBshmPtr[0]->complete == 1) {

        printf("were right here %d", PCBshmPtr[0]->timeCreateNanoseconds);
        printf("were right here %d", PCBshmPtr[0]->timeWorkingNanoseconds);


        PCBshmPtr[0]->totalCPUTimeSeconds = PCBshmPtr[0]->timeCreateSeconds + PCBshmPtr[0]->timeWorkingSeconds;
        PCBshmPtr[0]->totalCPUTimeNanoseconds = PCBshmPtr[0]->timeCreateNanoseconds + PCBshmPtr[0]->timeWorkingNanoseconds;

        printf("were right here %d", PCBshmPtr[0]->totalCPUTimeNanoseconds);

        if((sysClockshmPtr->nanoseconds + PCBshmPtr[0]->timeWorkingNanoseconds) > 999999999){
            rollover = (sysClockshmPtr->nanoseconds + PCBshmPtr[0]->timeWorkingNanoseconds) - 999999999;
            sysClockshmPtr->seconds += 1;
            sysClockshmPtr->nanoseconds = rollover;
            oneSecHappened = 1;
        } else {
            sysClockshmPtr->nanoseconds += PCBshmPtr[0]->timeWorkingNanoseconds;
        }

        sysClockshmPtr->seconds += PCBshmPtr[0]->totalCPUTimeSeconds;


//        PCBshmPtr[0]->totalCPUTimeSeconds = PCBshmPtr[0]->endCPUTimeSeconds - PCBshmPtr[0]->totalCPUTimeSeconds;
        PCBshmPtr[0]->complete = 0;
        FILE *fp = fopen("log.txt", "a+");
        fprintf(fp, "Finished process with PID %d at time %d:%d taking %d:%d amount of time\n",
                PCBshmPtr[0]->pidHolder, sysClockshmPtr->seconds, sysClockshmPtr->nanoseconds,
                PCBshmPtr[0]->totalCPUTimeSeconds, PCBshmPtr[0]->totalCPUTimeNanoseconds);
        fclose(fp);
    }
}

