#include "headers.h"

/* Modify this file as needed*/
int shmid;

// clean up resource and terminate 
void cleanup(int signum){
     shmctl(shmid, IPC_RMID, NULL);
     exit(0);
}
//semaphore union 
union Semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};
// lock semaphore
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
// release semaphore
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

// get semaphore
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

int main(int agrc, char * argv[])
{   
   
    initClk();
    // get shared memory
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
    //get semaphore
    int sem = getSem();

    // total time of process 
    int remain = atoi(argv[1]); 

    // exit when proecess is finished 
    while(remain > 0){
        int last = getClk(); 
        while ( *shmaddr > 0)
        {
            int now = getClk();
            // if more than one cycle 
            if(now-last > 1)
                last = now;
            
            // if one cycle 
            if(now-last == 1){
                // lock sem
                down(sem); 

                // decrease total by one 
                remain--; 

                // decrease quantum by one 
                *shmaddr -=1;

                last = now;
                // release sem 
                up(sem); 
            }
            if(*shmaddr == 0){
                //notify scheduler process has finished quantum 
                kill(getppid(),SIGUSR1);
            }
        }
    }
    printf("process %d terminated\n",getpid());
    //release resources 
    shmdt(shmaddr);
    destroyClk(false); 
    return 0;
}
