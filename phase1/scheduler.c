#include "headers.h"
#include "struct.h"
#define RR_PERIOD 2

Dstruct* queue;
int size = 0;
char selAlgo; 


void forkProcess();
void initiate_shared_memory(int shmid);
void save_state();

void insert(struct Process p){
    // struct Process *pp = (struct Process *) malloc(sizeof(struct Process )); 
    // *pp = p ; 
    enqueue(queue,p);
}
typedef struct RunningState running; 
struct RunningState{
    struct Process curr_process;
    int quantum ;
    int * shmaddr;
    int isRunning; 
};
int first = 1; 
int shmid,msgq_id;
running tracker; 
void cleanup(int signum){
     shmctl(shmid, IPC_RMID, NULL);
     msgctl(msgq_id,IPC_RMID,NULL); 
     exit(0);
}
union Semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};
void Pdown(int sem)
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

void Pup(int sem)
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
int initiateSem(){
    union Semun semun;
    key_t sem_key_id;
    sem_key_id = ftok("keyfile", SEM_KEY);
    int m = semget(sem_key_id, 1, 0666 | IPC_CREAT);
    if (m == -1) {
        perror("Error in create sem");
        exit(-1);
    }

    semun.val = 1;
    if (semctl(m, 0, SETVAL, semun) == -1) {
        perror("Error in semctl, this");
        exit(-1);
    }
    return m; 

}
 int sem; 
int main(int argc, char * argv[])
{   
    signal(SIGINT, cleanup);
    signal(SIGUSR1, save_state);
    selAlgo = atoi(argv[1]); 

    queue =  CreateStruct(selAlgo);
    key_t key_id;
    int rec_val;
    key_id = ftok("keyfile", G_MSG_KEY);        //create unique key
    msgq_id = msgget(key_id, 0666);
    if (msgq_id == -1)
    {
        perror("Error in create\n");
        exit(-1);
    }
    sem = initiateSem();
    // printf("start schedular\n\n");
    struct msgbuff message;
    tracker.isRunning = false; 
    initClk();
    // create shared memory to track if done
    int pid;
    key_t shmid_key_id;
    shmid_key_id = ftok("keyfile", P_SHM_KEY);
    shmid = shmget(shmid_key_id,  sizeof(int), IPC_CREAT | 0666);
    initiate_shared_memory(shmid); // initaite with zero
    bool read = true; 
    while (true)
    {   
        do{
            rec_val = msgrcv(msgq_id, &message, sizeof(message.p)+sizeof(bool), G_MSG_TYPE, IPC_NOWAIT);
        
            if (rec_val != -1){
                message.p.state = READY; 
                insert(message.p);
                // printf("scheduler: process received with arrival: %d   ========== \n\n", message.p.arrive);
            }
        }while(rec_val!= -1 && !message.isLast); 
        
        if(selAlgo==SRTN && getcount(queue)> 0 &&
        front(queue).remain < *tracker.shmaddr ){
            raise(SIGUSR1);
        }
        if(!tracker.isRunning && getcount(queue)/*count of proccesses*/ > 0){
            forkProcess();
        }
    
        // if((getcount(queue)/*count of proccesses*/ > 0 )
        // && (*tracker.shmaddr == 0 || (selAlgo==SRTN 
        // && front(queue).remain < *tracker.shmaddr ))){
        //     Pdown(sem);
        //     save_state(); 
        //     Pup(sem);
        //     forkProcess();
        // }
    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    destroyClk(true);
    
}
void initiate_shared_memory(int shmid){
    
    if (shmid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    int *shmaddr = (int *) shmat(shmid, (void *)0, 0);
    if (*shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
    (*shmaddr) = 0 ;
    tracker.shmaddr = shmaddr; 
}
void save_state(){
    Pdown(sem);
    tracker.curr_process.remain -= (tracker.quantum  - *tracker.shmaddr); 
    if(tracker.curr_process.remain > 0){
        // sleep(5); 
        kill(tracker.curr_process.pid,SIGSTOP);  
        tracker.curr_process.state = WAITING; 
        printf("At time %d Process %d stopped arr %d total %d remain %d wait\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain);
        insert(tracker.curr_process);  
    }else 
        printf("At time %d Process %d finished arr %d total %d remain %d wait\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain);
    tracker.isRunning = false; 
    Pup(sem); 
}

void forkProcess(){
    
    // printf("before dequeue\n");
    // printf("%d\n",getcount(queue));
    // displayS(queue);
    tracker.curr_process = dequeue(queue);
    // printf("after dequeue\n");
    
    // printf("forking: id = %d , arrive  = %d, ",tracker.curr_process.id,tracker.curr_process.arrive); 
    switch (selAlgo)
    {
        case RR: 
            if ( tracker.curr_process.remain < RR_PERIOD)
                tracker.quantum = tracker.curr_process.remain;
            else{
                tracker.quantum  = RR_PERIOD; 
            }
            break;
        default:
            // printf("process with id = %d and remain is %d\n",tracker.curr_process.id,tracker.curr_process.remain);
            tracker.quantum = tracker.curr_process.remain; 
            break;
    }
    *tracker.shmaddr = tracker.quantum;
    // printf("shared memory %d\n",*tracker.shmaddr);
    // printf("saved  quantum = %d\n",*tracker.shmaddr);
     if(tracker.curr_process.state == WAITING){
        printf("At time %d Process %d resumed arr %d total %d remain %d wait\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain);
        // printf("this is pid ========== %d\n",tracker.curr_process.pid);
        int result = kill(tracker.curr_process.pid,SIGCONT);
      //  if(result == -1){
        // printf("result to resume %d \n\n",result);
       // }
    } else {// ready
        printf("At time %d Process %d started arr %d total %d remain %d wait\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain);
        int pid = fork(),stat_loc;
        tracker.curr_process.pid = pid; 
        if (pid == -1)
            perror("error in fork");
        else if (pid == 0)
        {    
            char remain[10] ; 
            // printf("start child\n");
            sprintf(remain,"%i",tracker.curr_process.remain);
            execl("process.out", "process.out",remain, NULL); 
            // printf("execl failedddddddddddddddddddddddddd\n\n");
        }
    }
    tracker.curr_process.state = RUNNING;
    tracker.isRunning = true; 
}
   

