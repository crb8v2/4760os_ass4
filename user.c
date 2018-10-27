//
// Created by crbaniak on 10/17/18.
//

#include "ass4.h"

int main(int argc, char* argv[]){

    sharedMemoryConfig();

    printf("Hello world\n");

    shmdt(sysClockshmPtr);
    shmdt(PCBshmPtr);
    return 0;
}
