//
// Created by crbaniak on 10/26/18.
//

#include "ass4.h"

void ossClock();
void sigint(int);
void initForkToPCB();

int main(int argc, char* argv[]){

    int clockStop = 0;      // flag for stopping OS clock
    time_t t;               // for init random
    int forkMax = 0;

    // init clock the clock
    srand((unsigned) time(&t));

    //signal handling config - ctrl-c
    signal(SIGINT, sigint);

    // configure shared memory
    sharedMemoryConfig();

    // os clock
    while(clockStop == 0){

        //increments clock and adjusts sec:nanos
        ossClock();

        //fork a child
        if(forkMax == 0){
//            initForkToPCB();
            if ((PCBshmPtr[0]->pidHolder = fork()) == 0)
                execl("./user", "user", NULL);
                initForkToPCB();
            forkMax++;
        }
    }
    // clean shared memory
    shmdt(sysClockshmPtr);
    return 0;
}

// signal handles ctrl-c
void sigint(int a)
{
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
    } else {
        sysClockshmPtr->nanoseconds += clockIncrement;
    }

    printf("%d\n", sysClockshmPtr->seconds);
    printf("%d\n", sysClockshmPtr->nanoseconds);

    sleep(1);
}

void initForkToPCB(){
    FILE *fp = fopen("log.txt", "a+");
    fprintf(fp, "Generating process with PID %d, at time %d  %d \n", PCBshmPtr[0]->pidHolder, sysClockshmPtr->seconds, sysClockshmPtr->nanoseconds);
    fclose(fp);
}

