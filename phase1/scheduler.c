#include "headers.h"
#include "min_heap.h"
#define RR_PERIOD 2

Heap* queue;
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
};
int first = 1; 
int shmid,msgq_id;
running tracker; 
void cleanup(int signum){
     shmctl(shmid, IPC_RMID, NULL);
     msgctl(msgq_id,IPC_RMID,NULL); 
     exit(0);
}
int main(int argc, char * argv[])
{   
    signal(SIGINT, cleanup);
    selAlgo = *argv[1]; 
    queue =  CreateHeap(selAlgo);
    key_t key_id;
    int rec_val;
    key_id = ftok("keyfile", G_MSG_KEY);               //create unique key
    msgq_id = msgget(key_id, 0666);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    struct msgbuff message;
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
        if(read)
            rec_val = msgrcv(msgq_id, &message, sizeof(message.p), G_MSG_TYPE, IPC_NOWAIT);
        if (rec_val != -1){
            message.p.state = READY; 
            if(message.p.arrive == getClk()){
                printf("%d",getClk());
                insert(message.p);
                read = true;
                continue;  
            }else{
                read = false; 
            }
            //printf("scheduler: process received with arrival: %d   ========== \n\n", message.p.arrive);
        }
        if(queue->count > 0 && (*tracker.shmaddr == 0 || (selAlgo==SRTN && front(queue).remain < *tracker.shmaddr ))){
            save_state(); 
            //*tracker.curr_process = dequeue(queue);
            forkProcess();
        }
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
    if(first != 1){
        tracker.curr_process.remain -= (tracker.quantum  - *tracker.shmaddr); 
        if(tracker.curr_process.remain > 0){
            kill(tracker.curr_process.pid,SIGSTOP);  
            tracker.curr_process.state = WAITING; 
            printf("PID = %d  stopped at time %d with remain = %d ***********************\n"
                    ,tracker.curr_process.id,getClk(),tracker.curr_process.remain);
            insert(tracker.curr_process);  
        }else 
            printf("PID %d finished at time %d \n",tracker.curr_process.id,getClk());
    }
    first = 0; //set not first
}

void forkProcess(){
    
    tracker.curr_process = dequeue(queue);
    // printf("forking: id = %d , arrive  = %d, ",tracker.curr_process->id,tracker.curr_process->arrive); 
    switch (selAlgo)
    {
        case RR: 
            if ( tracker.curr_process.remain < RR_PERIOD)
                tracker.quantum = tracker.curr_process.remain;
            else{
                tracker.curr_process.remain  = RR_PERIOD; 
            }
            break;
        default:
            tracker.quantum = tracker.curr_process.remain; 
            break;
    }
    *tracker.shmaddr = tracker.quantum; 
     if(tracker.curr_process.state == WAITING){
        printf("PID = %d , arrival = %d, resumed at time %d for : %d ***********************\n"
            ,tracker.curr_process.id,tracker.curr_process.arrive,getClk(),tracker.quantum);
        kill(tracker.curr_process.pid,SIGCONT);
    } else {// ready
        int pid = fork(),stat_loc;
        tracker.curr_process.pid = pid; 
        if (pid == -1)
            perror("error in fork");
        else if (pid == 0)
        {   char sid[10] ; 
            sprintf(sid,"%i",tracker.curr_process.id);
            execl("process.out", "process.out",sid, NULL); 
        }
    }
    tracker.curr_process.state = RUNNING;
}
   

