#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <bits/mman-linux.h>
#include <semaphore.h>
#include "parent.h"

int main(int argsc, char** args){
    int shared_fd = open("temp.txt", O_RDWR | O_CREAT, 0);
    if (shared_fd == -1){
        exit(EXIT_FAILURE);
    }
    sem_t *semaphore_pc1;
    semaphore_pc1 = sem_open(SEM_NAME_PC1, O_EXCL);
    if (semaphore_pc1 == SEM_FAILED) {
        write(STDERR_FILENO, "Error connecting to semaphore.\n", 32);
    }
    sem_t *semaphore_c1c2;
    semaphore_c1c2 = sem_open(SEM_NAME_C1C2, O_CREAT, 0666, 0);
    if (semaphore_c1c2 == SEM_FAILED) {
        write(STDERR_FILENO, "Error connecting to semaphore.\n", 32);
    } 
    sem_init(semaphore_c1c2, 1, 0);
    char* data = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
    
    sem_wait(semaphore_pc1);
    // fprintf(stderr, "go");
    size_t bytes = strlen(data);
    // while(data[0] != '\n'){
        // fprintf(stderr, "%s\n", buf);
        int n = strlen(data);
        for (int i = 0; i < n; i++){
            data[i] = toupper(data[i]);
            // fprintf(stderr, "%c", data[i]);
        }
        sem_post(semaphore_c1c2);    
    // }

    return 0;
}