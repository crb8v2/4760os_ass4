//
// Created by crbaniak on 10/17/18.
//

#include "ass4.h"

int main(int argc, char* argv[]){

    sharedMemoryConfig();

//    sysClockshmPtr->nanoseconds = 3;
    printf("%d\n", sysClockshmPtr->nanoseconds);
//    PCBshmPtr->bitVector = 4;
    printf("%d\n", PCBshmPtr->bitVector);

    shmdt(sysClockshmPtr);
    return 0;
}
