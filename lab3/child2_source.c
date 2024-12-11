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

int main(int argsc, char **args)
{
   
    sem_t *semaphore_c1c2;
    semaphore_c1c2 = sem_open(SEM_NAME_C1C2, O_EXCL);  
    int shared_fd = open("temp.txt", O_RDWR | O_CREAT, 0);
    char* data = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
    sem_wait(semaphore_c1c2);
    // char buf[4096];
    // size_t bytes;
    // while (bytes = read(STDIN_FILENO, buf, sizeof(buf)))
    // {
        // if (buf[0] == '\n'){
        //     int written = write(STDOUT_FILENO, &buf[0], 1);
        //     if (written != 1)
        //     {
        //         const char msg[] = "error: failed to write to pipe\n";
        //         write(STDERR_FILENO, msg, sizeof(msg));
        //         exit(EXIT_FAILURE);
        //     }
        //     _exit(0);
        // }
        // if (bytes < 0)
        // {
        //     const char msg[] = "error: failed to read from stdin\n";
        //     write(STDERR_FILENO, msg, sizeof(msg));
        //     exit(EXIT_FAILURE);
        // }
        // fprintf(stderr, "c2read %s", buf);
        // buf[bytes] = '\0';
        int n = strlen(data);
        char res[4096];
        strcpy(res, "");

        // fprintf(stderr,"%s", buf);
        for (int i = 0; i < n - 1; i++)
        {
            if (data[i] == ' ' && data[i + 1] == ' ')
            {
                i++;
            }
            else
            {
                strncat(res, &data[i], 1);
            }
        }
        strncat(res, &data[n - 1], 1);
        //  fprintf(stderr, "c2");
        // fprintf(stderr, "%ld", bytes);
        // fprintf(stderr,"2 %s", res);
        
        sem_t *semaphore_c2p;
        semaphore_c2p = sem_open(SEM_NAME_C2P, O_CREAT, 0666, 0);
        if (semaphore_c2p == SEM_FAILED) {
            write(STDERR_FILENO, "Error connecting to semaphore.\n", 32);
        } 
        strcpy(data, res);
        // fprintf(stderr, "%s\n", res);
        // fprintf(stderr, "%s\n", data);
        sem_post(semaphore_c2p);
    // }

    return 0;
}