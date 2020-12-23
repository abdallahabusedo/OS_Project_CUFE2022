#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main() {
     key_t buffer_key_id, buffer_size_key_id;
    int bufferId, bufferSizeId;
    buffer_key_id = ftok("buffer_memory_key", 1);
    buffer_size_key_id = ftok("buffer_size_key", 2);
    
    int bufferSize;

    bufferSizeId = shmget(buffer_size_key_id, sizeof(int), IPC_EXCL | IPC_CREAT | 0666);
    if(bufferSizeId == -1){
        if (EEXIST == errno) {
            bufferSizeId = shmget(buffer_size_key_id, sizeof(int), IPC_CREAT | 0666);
            int *shmaddr = shmat(bufferSizeId, (void *)0, SHM_RDONLY);
            printf("buffer size Id = %d\n", bufferSizeId);
            if (shmaddr == (int *)-1)
            {
                perror("Error in attach in reader");
                exit(-1);
            }

            bufferSize = *shmaddr;
        }

        else {
            perror("msgget() failed");
        }
    }
    else{
        shmctl(bufferSizeId, IPC_RMID, (struct shmid_ds *)0);
        printf("No buffer available\n");
    }
}
