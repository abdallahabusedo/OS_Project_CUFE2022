#include "headers.h"
#include "priority_queue.h"
#define RR_PERIOD 2

struct Process ** queue;
int size = 0;
char selAlgo; 


void forkProcess();
void switchProcess();

void insert(struct Process p){
    struct Process *pp = (struct Process *) malloc(sizeof(struct Process )); 
    *pp = p ; 
    enqueue(queue,pp,&size,selAlgo);
}
typedef struct RunningState running; 
struct RunningState{
    struct Process *curr_process;
    int quantum ;
    int * shmaddr
};
running tracker; 
int main(int argc, char * argv[])
{   
 
    signal(SIGCHLD,switchProcess);
    queue = (struct Process **) malloc(sizeof(struct Process*));
    selAlgo = *argv[1]; 
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
        shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);
        int * shmaddr = initiate_shared_memory(shmid); // initaite with zero
    while (true)
    { 
        rec_val = msgrcv(msgq_id, &message, sizeof(message.p), G_MSG_TYPE, IPC_NOWAIT);
        if (rec_val != -1){
            printf("scheduler: process received with arrival: %d   ========== \n\n", message.p.arrive);
            insert(message.p); 
        }
        if(size > 0 &&  *shmaddr == 0){
            save_state(); 
            forkProcess();
        }

    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    destroyClk(true);
    
}
int * initiate_shared_memory(int shmid)
{
    
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
        if(selAlgo == RR && tracker.quantum > tracker.curr_process->remain) {
            tracker.quantum = tracker.curr_process->remain; 
        }
        tracker.curr_process->remain = tracker.curr_process->remain - tracker.quantum; 
        if(tracker.curr_process->remain > 0){
            //TODOsend signal to stop
            insert(*tracker.curr_process);  
        }
    }
}

void forkProcess(){
 
    tracker.curr_process  = dequeue(queue,&size,selAlgo); 
    tracker.curr_process->runtime = true; 
    switch (selAlgo)
    {
        case RR: 
            tracker.quantum = RR_PERIOD;
            break;
        default:
            tracker.quantum = tracker.curr_process->remain; 
            break;
    }
    *tracker.shmaddr = tracker.quantum; 
    printf("start process with arrival = %d and remain = %d at time %d \n\n",tracker.curr_process->arrive,tracker.curr_process->remain,getClk());
    int sch_pid = fork(),sch_stat_loc;
    if (sch_pid == -1)
        perror("error in fork");
    else if (sch_pid == 0)
    {   printf("\nI am the child, my pid = %d and my parent's pid = %d\n\n", getpid(), getppid());
        if(RR == selAlgo)
            execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/p.o", "p.o",tracker.curr_process->remain, NULL);
        else if(HPF == selAlgo){
            execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/clk.op.o", "p.o", NULL);
        }
    }
}
   

