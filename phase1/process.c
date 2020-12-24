#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

void set_remain_for_RR(int remain)
{
    
}
int main(int agrc, char * argv[])
{
    initClk();
    int shmid, pid;
    shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);
    int *shmaddr = (int*) shmat(shmid, (void *)0, 0);
    if (*shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }   

    printf("process started at time %d ***********************\n",getClk());
    while (*shmaddr > 0)
    {
        sleep(1); 
        *shmaddr -=1;
    }
    printf("process ended at time %d  \n\n",getClk());
    shmdt(shmaddr);
    destroyClk(false); 
    return 0;
}
