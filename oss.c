//
// Created by crbaniak on 10/26/18.
//

#include "ass4.h"

void sigint(int);
static void ALARMhandler();
void ossClock();
void forkIfSecondPassed();
void initForkToPCB(pid_t);
void roundRobinSchedule();
void writeResultsToLog();
void finalStatsToLog();

int main(int argc, char* argv[]){

    int clockStop = 0;      // flag for stopping OS clock

    // init clock to random
    time_t t;
    srand((unsigned) time(&t));

    //signal handling config - ctrl-c
    signal(SIGINT, sigint);

    // configure shared memory
    sharedMemoryConfig();

    //alarm times out if forks all do not return in 2 seconds
    signal(SIGALRM, ALARMhandler);
    alarm(4);


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

    // kill open forks
    for(int ii = 0; ii < sizeof(pids)/sizeof(int); ii++){
        if(PCBshmPtr[ii]->pidHolder != 0){
            signal(SIGQUIT, SIG_IGN);
            kill(PCBshmPtr[ii]->pidHolder, SIGQUIT);
        }
    }

    finalStatsToLog();

    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(PCBshmPtr);
    shmctl(PCBshmid, IPC_RMID, NULL);

    exit(0);
}

// alarm magic
static void ALARMhandler() {

    // kill open forks
    for(int ii = 0; ii < sizeof(pids)/sizeof(int); ii++){
        if(PCBshmPtr[ii]->pidHolder != 0){
            signal(SIGQUIT, SIG_IGN);
            kill(PCBshmPtr[ii]->pidHolder, SIGQUIT);
        }
    }

    finalStatsToLog();

    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(PCBshmPtr);
    shmctl(PCBshmid, IPC_RMID, NULL);

    printf("Timed out after 4 seconds.\n");
    exit(EXIT_SUCCESS);
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

//    printf("%d\n", sysClockshmPtr->seconds);
//    printf("%d\n", sysClockshmPtr->nanoseconds);

    usleep(200000);
}

//if one second happens, for a child
void forkIfSecondPassed(){

    if(oneSecHappened == 1) {
        for(int ii = 0; ii < sizeof(pids)/sizeof(int); ii++){
            if(pids[ii] == 0){
                if ((pids[ii] = fork()) == 0){
                    execl("./user", "user", NULL);
                }

                PCBshmPtr[ii]->pidHolder = pids[ii];

                initForkToPCB(pids[ii]);

                //set complete flag not complete
                PCBshmPtr[ii]->complete = 0;

                break;
            }
        }
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

//    for(int ii = 0; ii < sizeof(pids)/sizeof(int); ii++) {
//        if (pids[ii] != 0) {
//                msgQueuePtr->pid = PCBshmPtr[0]->pidHolder;
//        }
//    }

    if(sysClockshmPtr->seconds == 2){
        msgQueuePtr->pid = PCBshmPtr[0]->pidHolder;
    }

    if(sysClockshmPtr->seconds == 4){
        msgQueuePtr->pid = PCBshmPtr[2]->pidHolder;
    }


}

// if process completes, write data to log
void writeResultsToLog(){

    int rollover; //for clock

    for(int ii = 0; ii < sizeof(pids)/sizeof(int); ii++) {
        if (PCBshmPtr[ii]->complete == 1) {

            PCBshmPtr[0]->totalCPUTimeSeconds = PCBshmPtr[0]->timeCreateSeconds + PCBshmPtr[0]->timeWorkingSeconds;
            PCBshmPtr[0]->totalCPUTimeNanoseconds =
                    PCBshmPtr[0]->timeCreateNanoseconds + PCBshmPtr[0]->timeWorkingNanoseconds;

            if ((sysClockshmPtr->nanoseconds + PCBshmPtr[0]->timeWorkingNanoseconds) > 999999999) {
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

}

void finalStatsToLog(){
    FILE *fp = fopen("log.txt", "a+");
    fprintf(fp, "**************************************\n");
    fprintf(fp, "*******Final Clock Time***************\n");
    fprintf(fp, "**********%d:%d*****************\n", sysClockshmPtr->seconds, sysClockshmPtr-> nanoseconds);
    fprintf(fp, "**************************************\n");

    fclose(fp);
}

