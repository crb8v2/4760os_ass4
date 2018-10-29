//
// Created by crbaniak on 10/17/18.
//

#include "ass4.h"

int main(int argc, char* argv[]){

    sharedMemoryConfig();

//    FILE *fp = fopen("log.txt", "a+");
////    fputs("Child: ", fp);
//    fclose(fp);

    printf("Hello world\n");

    shmdt(sysClockshmPtr);
    shmdt(PCBshmPtr);
    return 0;
}
