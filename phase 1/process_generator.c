#include "headers.h"
#include <stdio.h>
#include <string.h>
#define HPF 1
#define SRTN 2
#define RR 3

void clearResources(int);
const char* PROCESSES_FILE_NAME = "phase 1/processes.txt";

struct Process{
    int id; 
    int arrive; 
    int runtime; 
    int priority; 
}; 

struct Process ** readProcesses(int * count){
    FILE * file = fopen(PROCESSES_FILE_NAME,"r"); 
    if(!file){
        printf("not able to read processes"); 
        exit(-1);                                   // what should we do here ? 
    }
    char * line; 
    struct Process* processes[__INT8_MAX__];
    int i = 0; 
    size_t buf_size = 0; 
    while(getline(&line,&buf_size,file) > 0){
        if(line[0]=='#') continue; 
        processes[i] = (struct Process *) malloc(sizeof(struct Process*));
        processes[i]->id = line[0]-'0'; 
        processes[i]->arrive = line[4]-'0'; 
        processes[i]->runtime = line[8]-'0'; 
        processes[i]->priority = line[12]-'0'; 
        i++; 
        // loop through the string to extract all other tokens
       
    }
    count = i; 
    fclose(file); 
    return processes; 
}


int readAlgoNum()
{
    int selected_algorithm_Number;
    printf("Please select one of these algorithms: \n");
    printf("1) Non-preemptive Highest Priority First (HPF).\n");
    printf("2) Shortest Remaining time Next (SRTN).\n");
    printf("3) Round Robin (RR).\n");
    scanf("%d", &selected_algorithm_Number);
    if (selected_algorithm_Number != 1 && selected_algorithm_Number != 2 && selected_algorithm_Number != 3)
    {
        printf("Please select one of the following algorithms numbers 1 , 2 or 3\n");
        readAlgoNum();
    }
    else
    {
        return selected_algorithm_Number;
    }
}
void writeInClk(int shmid)
{
    int *shmaddr = (int *) shmat(shmid, (void *)0, 0);
    if ((long)shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
   // printf("\nWriter: Shared memory attached at address %x\n", *shmaddr);
    (*shmaddr) = 0;
    //printf("\nWriter Detaching...");
    shmdt(shmaddr);
}

void initializeClk(){
    int shmid = shmget(SHKEY, 4, IPC_CREAT | 0666);
    if(shmid == -1){
        printf("Erroer creating shared memory for clock");
        exit(-1);
    }
    writeInClk(shmid);
}

void startHPF_schedular(struct Process ** processes,int P_N){
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    // To get time use this
    while(1){
    int x = getClk();
    printf("current time is %d\n", x);
    sleep(1);
    }
}
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    int P_N = 0; 
    struct Process ** processes = readProcesses(&P_N); 
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int selAlgo = readAlgoNum();
    // 3. Initiate and create the scheduler and clock processes.
    initializeClk();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // TODO Generation Main Loop
    switch (selAlgo)
    {
    case SRTN: 
        startSRTN_schedular(processes,P_N); 
        break;
    case HPF: 
        startHPF_schedular(processes,P_N); 
        break;
    default:
        startRR_schedular(processes,P_N); 
        break;
    }
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    
}
