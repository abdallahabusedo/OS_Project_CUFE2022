#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int shmid;
void cleanup(int signum){
     shmctl(shmid, IPC_RMID, NULL);
     exit(0);
}
int main(int agrc, char * argv[])
{
    signal(SIGINT, cleanup);
    initClk();
    printf("PID = %s, started at time %d ***********************\n",argv[1],getClk());
    int pid;
    key_t key_id;
    key_id = ftok("keyfile", P_SHM_KEY);
    shmid = shmget(key_id, sizeof(int), IPC_CREAT | 0666);
    int *shmaddr = (int*) shmat(shmid, (void *)0, 0);
    if (*shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }   

    int last = getClk(); 
    while (*shmaddr > 0)
    {
        int now = getClk(); 
        if(now-last == 1){
            *shmaddr -=1;
            last = now;
        }
    }

 
    shmdt(shmaddr);
    destroyClk(false); 
    return 0;
}
