#include "headers.h"
#include "struct.h"
#include "memory.h"

int RR_QUANTUM = 2; // RR quantum time 
Dstruct* readyQueue; // ready queue 
Dstruct* waitingQueue; // allocation waiting queue 
char selAlgo; //selected algorithm 


int n;  // number of processes in input file
int finishedProcessesNumber = 0; // #process has finished
float sumWTA = 0; // summation of weighted turn around time
int sumWaiting = 0; // summation of waiting time for each process in ready queue
float* wtaArray; // array holds wta time for each process
int usefulTime = 0; // summation of time processes was being running 

void forkProcess();
void initiate_shared_memory(int shmid);
void save_state();
pair allocateProcess(struct Process p ); 
// isert process in ready queue
void insert(struct Process p){
    // struct Process *pp = (struct Process *) malloc(sizeof(struct Process )); 
    // *pp = p ; 
    enqueue(readyQueue,p);
}

// struct that holds info about running process
typedef struct RunningState running; 
struct RunningState{
    struct Process curr_process;
    int quantum ;
    int * shmaddr;
    int isRunning; 
};
// resources ids
int shmid,msgq_id;
int sem;
// pointer to log file 
FILE* logptr;
FILE* memfileptr;
//tracker to current running prcocess
running tracker; 

// clear up resrouces and terminates 
void cleanup(){
    shmctl(shmid, IPC_RMID, NULL);
    semctl(sem, 0, IPC_RMID);
    fclose(logptr);
    fclose(memfileptr); 
    free(wtaArray);
    //  msgctl(msgq_id,IPC_RMID,NULL); 
    printf("scheduler is terminating\n");
    exit(0);
}
// semaphore union 
union Semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

//semaphore down 
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
// semaphore up
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
// initialize semaphore as up 
// return semaphore id 
int initiateSem(){
    union Semun semun;
    int sem_id = ftok("keyfile", SEM_KEY);
    int m = semget(sem_id, 1, 0666 | IPC_CREAT);
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

// add process to queue of waiting process that needs to be allocated
void addToMemAllocatingQueue(struct Process p){
    enqueue(waitingQueue,p); 
}
// get front of Memory waiting queue without dequeue
struct Process frontOfMemAllocQueue(){ 
    return front(waitingQueue); 
}
// deqeue from Memory waiting queue
struct Process dequeueOfMemAllocQueue(){
    return dequeue(waitingQueue); 
}
int main(int argc, char * argv[])
{   

    //seletected algorithm 
    selAlgo = atoi(argv[1]); 

    //numver of process in input file
    n = atoi(argv[2]);

    //quantum in case of RR
    RR_QUANTUM = atoi(argv[3]); 

    //hanlde interrupt 
    signal(SIGINT, cleanup);

    //handle finished process
    signal(SIGUSR1, save_state);

    wtaArray = (float *) malloc(n * sizeof(float));

    // create data structure based on selected algorithm 
    readyQueue =  CreateStruct(selAlgo);
    waitingQueue = CreateStruct(SMF); 
    InitializeMemory(); 

    // get message queue resource
    key_t key_id;
    int rec_val;
    key_id = ftok("keyfile", G_MSG_KEY);        //create unique key
    msgq_id = msgget(key_id, 0666);
    if (msgq_id == -1)
    {
        perror("Error in create\n");
        exit(-1);
    }
    
    //initialize semaphore 
    sem = initiateSem();

    struct msgbuff message;
    
    //set tracker not running
    tracker.isRunning = false; 
    
    // initialize clock 
    initClk();

    // create shared memory to track if done
    int pid;
    key_t shmid_key_id;
    shmid_key_id = ftok("keyfile", P_SHM_KEY);
    shmid = shmget(shmid_key_id,  sizeof(int), IPC_CREAT | 0666);

    // initaite shared memory with zero
    initiate_shared_memory(shmid); 

    // open log file 
    logptr = fopen("scheduler.log","w");
    memfileptr = fopen("memory.log","w");
    fprintf(logptr, "#At time x process y state arr w total z remain y wait k\n");
    fprintf(memfileptr, "#At time x allocated y bytes for process z from i to j\n");
    // loop until all processes are finished 
    while (true)
    {   
        do{
            // check if theres is a process in the message queue 
            rec_val = msgrcv(msgq_id, &message, sizeof(message.p)+sizeof(bool), G_MSG_TYPE, IPC_NOWAIT);
            if (rec_val != -1){
                //set process stae to ready 
                message.p.state = READY;
                message.p.isAllocated = false; 
                // add it to ready queue
              
                enqueue(readyQueue,message.p); 
        }
        }while(rec_val!= -1 && !message.isLast); // exit when process is last (there're no more processes arrived at same time )
        
        // if selected algorithm is SRTN and there's a process 
        //with less remain time than the running one
        if(selAlgo==SRTN && getcount(readyQueue)> 0 &&
        front(readyQueue).remain < *tracker.shmaddr ){
            //send signal to save info of current running 
            printf("isnert process queue");
            raise(SIGUSR1);
        }
        //if tracker has no running process fork new porcess
        if(!tracker.isRunning && getcount(readyQueue)/*count of proccesses*/ > 0){
            // get process to fork 
            tracker.curr_process = dequeue(readyQueue);
            
            // if process not allocated yet (not forked yet)
            if(!tracker.curr_process.isAllocated)
            { 
                // if no space for this process to be allocated
                pair mem; 
                mem =  allocateProcess(tracker.curr_process); 
                if(mem.end != 0){
                    tracker.curr_process.mem = mem; 
                    
                    fprintf(memfileptr,"At time %d allocated %d bytes for process %d from %d to %d\n",getClk()
                    ,tracker.curr_process.memsize,tracker.curr_process.id
                    ,tracker.curr_process.mem.start,tracker.curr_process.mem.end);
                    
                    printf("At time %d allocated %d bytes for process %d from %d to %d\n",getClk()
                    ,tracker.curr_process.memsize,tracker.curr_process.id
                    ,tracker.curr_process.mem.start,tracker.curr_process.mem.end); 
                    tracker.curr_process.isAllocated = true; 
                }
                else{
                    continue; 
                }
            }
            forkProcess();
        }
    }
    // destroyClk(true);
}

// initialize shared memory to store the time 
//that running process should take before context switching 
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

// save info of current running process 
// handle if is last one in input file --> save log data
// if process not finished enqueue it again in ready queue
void save_state(){
    // wait until process finishes using shared memroy 
    Pdown(sem);
    tracker.curr_process.stopTime = getClk();

    // calculate remainging time of last running process 
    tracker.curr_process.remain -= (tracker.quantum  - *tracker.shmaddr);

    // add running rime to userfulTime
    usefulTime += (tracker.quantum  - *tracker.shmaddr);
    if(tracker.curr_process.remain > 0){
        // send signal to running process to stop 
        kill(tracker.curr_process.pid,SIGSTOP);  
        tracker.curr_process.state = WAITING;
        fprintf(logptr, "At time %d Process %d stopped arr %d total %d remain %d wait %d\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait);

        printf("At time %d Process %d stopped arr %d total %d remain %d wait %d\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait);
        enqueue(readyQueue,tracker.curr_process); 
        // insert(tracker.curr_process);  
    }else {
        
        // process has finished calculate and write its data to the log file 
        int TA = getClk() - tracker.curr_process.arrive;
        float WTA = (float)TA/tracker.curr_process.runtime;
        WTA = (int)(WTA * 100.0 + .5)/100.0;
        sumWTA += WTA;
        sumWaiting += tracker.curr_process.wait;
        wtaArray[finishedProcessesNumber] = WTA;
        finishedProcessesNumber += 1;
        fprintf(logptr, "At time %d Process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait, TA, WTA
        );

        printf("At time %d Process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait, TA, WTA
        );
        // if last process in input file 
        if(finishedProcessesNumber == n){
            FILE* fptr = fopen("scheduler.perf","w");

            // calculate statistics 
            float utilization = 100.0*usefulTime/getClk();
            float avgWTA = sumWTA / n;
            float avgWaiting = (float)sumWaiting / n;
            float sumOfDifferenceSquared = 0;
            for (int i = 0; i < n; i++) {
                sumOfDifferenceSquared += ((wtaArray[i] - avgWTA)*(wtaArray[i] - avgWTA));
            }
            float stdWTA = sqrt(sumOfDifferenceSquared/n);

            fprintf(fptr, "CPU utilization %.2f%%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f\n", utilization, avgWTA, avgWaiting, stdWTA);
            printf("CPU utilization %.2f%%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f\n", utilization,avgWTA, avgWaiting, stdWTA);
            
            fclose(fptr);
            
            fprintf(memfileptr,"At time %d freed %d bytes for process %d from %d to %d\n",getClk()
            ,tracker.curr_process.memsize,tracker.curr_process.id,tracker.curr_process.mem.start,
                tracker.curr_process.mem.end);

            printf("At time %d freed %d bytes for process %d from %d to %d\n",getClk()
            ,tracker.curr_process.memsize,tracker.curr_process.id,tracker.curr_process.mem.start,
                tracker.curr_process.mem.end); 
            deallocate(tracker.curr_process.mem);

            // terminate all 
            destroyClk(true);
        }

        fprintf(memfileptr,"At time %d freed %d bytes for process %d from %d to %d\n",getClk()
        ,tracker.curr_process.memsize,tracker.curr_process.id,
        tracker.curr_process.mem.start,tracker.curr_process.mem.end);

        printf("At time %d freed %d bytes for process %d from %d to %d\n",getClk()
        ,tracker.curr_process.memsize,tracker.curr_process.id,
        tracker.curr_process.mem.start,tracker.curr_process.mem.end); 
        deallocate(tracker.curr_process.mem); 
        if(getcount(waitingQueue) > 0 ){
            //front is allocated remove front 
            // printf("waiting queue is has %d processe\n",getcount(waitingQueue));
            pair mem; 
            mem =  allocateProcess(frontOfMemAllocQueue()); 
            while(mem.end != 0){
                tracker.curr_process =  dequeueOfMemAllocQueue(); 
                tracker.curr_process.mem = mem; 
                
                fprintf(memfileptr, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk()
                ,tracker.curr_process.memsize,tracker.curr_process.id
                ,tracker.curr_process.mem.start,tracker.curr_process.mem.end);
                
                printf("At time %d allocated %d bytes for process %d from %d to %d\n",getClk()
                ,tracker.curr_process.memsize,tracker.curr_process.id
                ,tracker.curr_process.mem.start,tracker.curr_process.mem.end); 
                tracker.curr_process.isAllocated = true; 
                enqueue(readyQueue, tracker.curr_process);
                if(getcount(waitingQueue)> 0){
                    mem =  allocateProcess(frontOfMemAllocQueue()); 
                }else 
                    break; 
            }
        } 
    }
    tracker.isRunning = false; 
    // free shared memory
    Pup(sem); 
}

// allocate process in memory 
// return pair of memory allocated
pair allocateProcess(struct Process p ){
    // allocate process --> return 0 if not allocated
    pair pair1  = allocate(p.memsize); 
    // if couldn't allocate 
    if(pair1.end == 0){
        // add process to waiting queue
        printf("process with id %d is waiting to be allocated \n",p.id); 
        addToMemAllocatingQueue(p);
    }
    return pair1; 
}

// fork process in tracker
// or resume if forked before 
void forkProcess(){
    // get quantum based on algo
    switch (selAlgo)
    {
        case RR:
            // if remain less thant quantum 
            // set quantum to remain
            if ( tracker.curr_process.remain < RR_QUANTUM)
                tracker.quantum = tracker.curr_process.remain;
            else{
                tracker.quantum  = RR_QUANTUM; 
            }
            break;
        default:
            tracker.quantum = tracker.curr_process.remain; 
            break;
    }

    //set quantum of next process 
    *tracker.shmaddr = tracker.quantum;
   
    // if process was stopped, continue
     if(tracker.curr_process.state == WAITING){
        tracker.curr_process.wait += (getClk() - tracker.curr_process.stopTime);
        
        fprintf(logptr, "At time %d Process %d resumed arr %d total %d remain %d wait %d\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait);

        printf("At time %d Process %d resumed arr %d total %d remain %d wait %d\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait);
       // send continue signal to process with pid 
        int result = kill(tracker.curr_process.pid,SIGCONT);
    } else {// process is new 
        //calc waiting time of process 
        tracker.curr_process.wait += (getClk() - tracker.curr_process.arrive);
        
        fprintf(logptr, "At time %d Process %d started arr %d total %d remain %d wait %d\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait);

        printf("At time %d Process %d started arr %d total %d remain %d wait %d\n"
            ,getClk(),tracker.curr_process.id,tracker.curr_process.arrive
            ,tracker.curr_process.runtime,tracker.curr_process.remain
            ,tracker.curr_process.wait);
        
        //for process 
        int pid = fork(),stat_loc;
        tracker.curr_process.pid = pid; 
        if (pid == -1)
            perror("error in fork");
        else if (pid == 0)
        {    
            char remain[10] ; 
            sprintf(remain,"%i",tracker.curr_process.runtime);
            // replace child code to process.out file 
            execl("process.out", "process.out",remain, NULL); 
        }
    }
    //set state of process to running 
    tracker.curr_process.state = RUNNING;
    tracker.isRunning = true; 
}
   

