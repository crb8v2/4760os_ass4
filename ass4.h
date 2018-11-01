//
// Created by crbaniak on 10/26/18.
//

#ifndef ASS4_ASS4_H
#define ASS4_ASS4_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>



//#################################
//##### SHARED MEMORY SEGMENT #####
//#################################

//shared mem keys, probs not secure in a header
#define CLOCK_SHMKEY 123123
#define PCB_SHMKEY 123124
#define MSG_QUEUE 123125

#define TIME_QUANTUM 10

//shared mem structures
typedef struct {
    pid_t pidHolder;

    int timeCreateSeconds;
    int timeCreateNanoseconds;

    int timeWorkingSeconds;
    int timeWorkingNanoseconds;

    int totalCPUTimeSeconds;
    int totalCPUTimeNanoseconds;

    int priorityValue;
    int bitVector;
    int complete;
}  PCB_t[20];

typedef struct {
    unsigned int seconds;
    unsigned int nanoseconds;
} systemClock_t;

typedef struct {
    pid_t pid;
    int timeQuantum;
} msgQueue_t;

// globals for accessing pointers to shared memory
int sysClockshmid;              //holds the shared memory segment id
int PCBshmid;
int msgQueueshmid;

systemClock_t *sysClockshmPtr;   //points to the data structure
PCB_t *PCBshmPtr;
msgQueue_t *msgQueuePtr;

// for OSS
int oneSecHappened = 0;
int pids[20] = {0};

// allocates shared mem
void sharedMemoryConfig(){

    //shared mem for sysClock
    sysClockshmid = shmget(CLOCK_SHMKEY, sizeof(systemClock_t), IPC_CREAT|0777);
    if(sysClockshmid < 0)
    {
        perror("sysClock shmget error in master \n");
        exit(errno);
    }
    sysClockshmPtr = shmat(sysClockshmid, NULL, 0);
    if(sysClockshmPtr < 0){
        perror("sysClock shmat error in oss\n");
        exit(errno);
    }

    // shared mem for PCB
    PCBshmid = shmget(PCB_SHMKEY, sizeof(PCB_t), IPC_CREAT|0777);
    if(PCBshmid < 0)
    {
        perror("PCB shmget error in master \n");
        exit(errno);
    }
    PCBshmPtr = shmat(PCBshmid, NULL, 0);
    if(PCBshmPtr < 0){
        perror("PCB shmat error in oss\n");
        exit(errno);
    }

    // shared mem for message queue
    msgQueueshmid = shmget(MSG_QUEUE, sizeof(msgQueue_t), IPC_CREAT|0777);
    if(msgQueueshmid < 0)
    {
        perror("PCB shmget error in master \n");
        exit(errno);
    }
    msgQueuePtr = shmat(msgQueueshmid, NULL, 0);
    if(msgQueuePtr < 0){
        perror("PCB shmat error in oss\n");
        exit(errno);
    }

    //tests for shared mem
//    sysClockshmPtr->nanoseconds = 3;
//    printf("%d\n", sysClockshmPtr->nanoseconds);

//    sysClockshmPtr->nanoseconds = 0;
//    sysClockshmPtr->seconds = 0;
}

#endif //ASS4_H
