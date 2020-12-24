#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

void set_remain_for_RR(int remain)
{
    int shmid, pid;
    shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);
    int *shmaddr = (int*) shmat(shmid, (void *)0, 0);
    if (*shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    else
    {
        *shmaddr = remain; 
    } 
    shmdt(shmaddr);
}
int main(int agrc, char * argv[])
{
    initClk();
    remainingtime = atoi(argv[1]); 
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    if(agrc > 2){
       int remain = atoi(argv[2]); 
       if(remain < remainingtime){
           remainingtime = remainingtime; 
       }
       remain = remain - remainingtime; 
        set_remain_for_RR(remain);
    }
    printf("process started at time %d \n",getClk());
    while (remainingtime > 0)
    {
        remainingtime =-1;
        sleep(1); 
    }
    printf("process ended at time %d  \n\n",getClk());
    destroyClk(false);
    kill(getppid(),SIGCHLD); 
    return 0;
}
