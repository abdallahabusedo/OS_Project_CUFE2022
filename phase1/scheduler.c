#include "headers.h"
#include "min_heap.h"
#define RR_PERIOD 2

Heap* queue;
int size = 0;
char selAlgo; 


void forkProcess();
int * initiate_shared_memory(int shmid);
void save_state();

void insert(struct Process p){
    // struct Process *pp = (struct Process *) malloc(sizeof(struct Process )); 
    // *pp = p ; 
    enqueue(queue,p);
}
typedef struct RunningState running; 
struct RunningState{
    struct Process *curr_process;
    int quantum ;
    int * shmaddr;
};
running tracker; 
int main(int argc, char * argv[])
{   
 

    selAlgo = *argv[1]; 
    queue =  CreateHeap(selAlgo);
    key_t key_id;
    int rec_val, msgq_id;
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
    int shmid, pid;
    key_t shmid_key_id;
    shmid_key_id = ftok("keyfile", P_SHM_KEY);
    shmid = shmget(shmid_key_id,  sizeof(int), IPC_CREAT | 0644);
    int * shmaddr = initiate_shared_memory(shmid); // initaite with zero
    while (true)
    { 
        rec_val = msgrcv(msgq_id, &message, sizeof(message.p), G_MSG_TYPE, IPC_NOWAIT);
        if (rec_val != -1){
            printf("scheduler: process received with arrival: %d   ========== \n\n", message.p.arrive);
            message.p.state = READY; 
            insert(message.p); 
            
        }
        if(queue->count > 0 && *shmaddr == 0 ){//|| (selAlgo==SRTN && front(queue).remain < *shmaddr))){
            /*save_state();*/ 
            printf("forking ++++++++++++++++");
            forkProcess();
        }

    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    destroyClk(true);
    
}
int * initiate_shared_memory(int shmid){
    
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
   return shmaddr; 
}
void save_state(){
    if(tracker.curr_process != NULL){
        tracker.curr_process->remain = tracker.curr_process->remain - tracker.quantum; 
        if(tracker.curr_process->remain > 0){
            kill(tracker.curr_process->pid,SIGSTOP);  
            tracker.curr_process->state = WAITING; 
            insert(*tracker.curr_process);  
        }
    }
}

void forkProcess(){
 
    *tracker.curr_process = dequeue(queue);
    print("forking: id = %d , arrive  = %d, ",tracker.curr_process->id,tracker.curr_process->arrive); 
    //tracker.curr_process->state = true; 
    switch (selAlgo)
    {
        case RR: 
            if ( tracker.curr_process->remain < RR_PERIOD)
                tracker.quantum = tracker.curr_process->remain;
            else{
                tracker.curr_process->remain  = RR_PERIOD; 
            }
            break;
        default:
            tracker.quantum = tracker.curr_process->remain; 
            break;
    }
    *tracker.shmaddr = tracker.quantum; 
     if(tracker.curr_process->state == WAITING){
        kill(tracker.curr_process->pid,SIGCONT);
    } else {// ready
        printf("start process with arrival = %d and remain = %d at time %d \n\n",tracker.curr_process->arrive,tracker.curr_process->remain,getClk());
        int pid = fork(),stat_loc;
        tracker.curr_process->pid = pid; 
        if (pid == -1)
            perror("error in fork");
        else if (pid == 0)
        {   printf("\nI am the child, my pid = %d and my parent's pid = %d\n\n", getpid(), getppid());
            if(RR == selAlgo)
                execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/p.o", "p.o", NULL);
            else if(HPF == selAlgo){
                execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/clk.op.o", "p.o", NULL);
            }
        }
    }
    tracker.curr_process->state = RUNNING;
}
   

