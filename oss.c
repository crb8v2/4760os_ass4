#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMKEY 123123

void sharedMemory();

int main() {

    //#############################
    // ### BLOCK FOR SHARED MEM ###
    //#############################
    //shared memory (key, size, permissions)
    int exampleSize[3];
    int shmid = shmget ( SHMKEY, sizeof(exampleSize[3]), 0775 | IPC_CREAT );
    //get pointer to memory block
    char * paddr = ( char * )( shmat ( shmid, NULL, 0 ) );
    int * pint = ( int * )( paddr );
    // init vars
    pint[0] = 0;
    pint[1] = 0;
    pint[2] = 0;

    //cries, very hard



    printf("Hello, World!\n");
    return 0;
}