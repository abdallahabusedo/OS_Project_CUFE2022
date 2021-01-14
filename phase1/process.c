#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int shmid;
void cleanup(int signum){
     shmctl(shmid, IPC_RMID, NULL);
     exit(0);
}
union Semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};
void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}
int getSem(){
    union Semun semun;
    key_t sem_key_id;
    sem_key_id = ftok("keyfile", SEM_KEY);
    int m = semget(sem_key_id, 1, 0666);
    if (m == -1) {
        printf("Please run server first\n");
        exit(-1);
    }
    return m; 
}
void con(){
    printf("received continue signal ************************\n\n");
}
int main(int agrc, char * argv[])
{   
    signal(SIGINT, cleanup);
    // signal(SIGCONT,con); 
    initClk();
    // printf("remain = %s, started at time %d ***********************\n",argv[1],getClk());
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
    int sem = getSem();
    int remain = atoi(argv[1]); 
    int last = getClk(); 
    // printf("my pid is %d and my remain is %d\n",getpid(),remain);
    while(remain > 0){
        //  printf("process: enter first loop \n");
        while ( *shmaddr > 0)
        {
            // printf("process: enter second loop \n");
            int now = getClk(); 
            if(now-last == 1){
                // printf("remain in process-------%d\n",remain);
                down(sem); 
                remain--; 
                *shmaddr -=1;
                // printf("%d\n",*shmaddr);
                last = now;
                up(sem); 
            }
            if(*shmaddr == 0){
                // printf("process sent finish signal &&&&   %d \n",*shmaddr);
                kill(getppid(),SIGUSR1);
                // printf("process sent finish signal &&&&   %d \n",*shmaddr);
            }

        }
       
    }
    printf("process %d terminated\n",getpid());

    shmdt(shmaddr);
    destroyClk(false); 
    return 0;
}
