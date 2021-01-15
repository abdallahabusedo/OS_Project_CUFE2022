#pragma once 
#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <math.h>

typedef short bool;
#define true 1
#define false 0
#define SHKEY 300

// some defines 
#define HPF 1 
#define SRTN 2
#define RR 3
#define G_MSG_TYPE 7
#define G_MSG_KEY 77
#define P_SHM_TYPE 8
#define P_SHM_KEY 88
#define RUNNING 1
#define WAITING 2
#define READY 3
#define SEM_KEY 66


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



int getClk()
{
    return *shmaddr;
}

// process data structure 
struct Process{
    int id; 
    int arrive; 
    int runtime; 
    int priority; 
    int remain;
    int state;
    int execTime; 
    int wait;
    int stopTime;
    int pid; 
}; 

// message buff structure 
struct msgbuff
{
    long mtype; 
    struct Process p; // process 
    bool isLast;    // is last one with save arrival 
};



/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
