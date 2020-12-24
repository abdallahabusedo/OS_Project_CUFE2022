#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    int shmid, pid;
    key_t key_id;
    key_id = ftok("keyfile", P_SHM_KEY);
    shmid = shmget(key_id, sizeof(int), IPC_CREAT | 0644);
    int *shmaddr = (int*) shmat(shmid, (void *)0, 0);
    if (*shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }   

    printf("process started at time %d ***********************\n",getClk());
    int last = getClk(); 
    while (*shmaddr > 0)
    {
        int now = getClk(); 
        if(now-last == 1){
            *shmaddr -=1;
            last = now;
        }
    }
    printf("process ended at time %d  \n\n",getClk());
    shmdt(shmaddr);
    destroyClk(false); 
    return 0;
}
