#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <bits/mman-linux.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include "parent.h"

int main()
{
    
    char path1[4096] = "/media/sf_univer/OSlabs/lab3/child1";
    char path2[4096] = "/media/sf_univer/OSlabs/lab3/child2";
    
    int shared_fd = open("temp.txt", O_RDWR | O_CREAT, 0);
    if (shared_fd == -1){
        exit(EXIT_FAILURE);
    }
    
    char* data = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
    data[10] = '4';
   
    pid_t child1 = fork();
    int status;
    int res_status;
    if (child1 == -1)
    {
        exit(EXIT_FAILURE);
    }
   
    
    if (child1 == 0)
    {
        char *args[] = {NULL};
        status = execv(path1, args);
        if (status == -1)
        {
            const char msg[] = "error: failed to exec into new exectuable image\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }
    }

    // write(shared_fd, "aaaaa", 6);
    // sem_post(semaphore1);
    // wait(NULL);
    // printf("%s", data);
    // return 0;
    else
    {
        //
        // parent

        pid_t child2 = fork();
        if (child2 == -1)
        {
            exit(EXIT_FAILURE);
        }
        if (child2 == 0)
        {
            char *args[] = {NULL};
            status = execv(path2, args);

            if (status == -1)
            {
                const char msg[] = "error: failed to exec into new exectuable image\n";
                write(STDERR_FILENO, msg, sizeof(msg));
                kill(child1, SIGKILL);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            char input_string[4096];
            char output_string[4096];
            sem_t *semaphore_p_c1;
            semaphore_p_c1 = sem_open(SEM_NAME_PC1, 0, 0666, 0);
            int a;
            sem_init(semaphore_p_c1, 1, 0);
            int n_inp = read(STDIN_FILENO, input_string, sizeof(input_string));
            int n_out;
            sem_t *semaphore_c2p;
            semaphore_c2p = sem_open(SEM_NAME_C2P, O_EXCL);
            sem_init(semaphore_c2p, 1, 0);
            while (input_string[0] != '\n' && input_string[0] != EOF)
            {

               
                // fprintf(stderr, "p %s", input_string);
                input_string[n_inp - 1] = '\0';                            
                write(shared_fd, input_string, n_inp);
                // sem_post(semaphore_p_c1);
                // fprintf(stderr, "wait");
                // n_out = read(p2[0], output_string, sizeof(output_string));
                // fprintf(stderr, "parent read %s end", output_string);
                // int really_written = write(STDOUT_FILENO, output_string, n_out);
                char temp = '\n';
                sem_post(semaphore_p_c1);
                sem_wait(semaphore_c2p);
                write(STDOUT_FILENO, data, strlen(data));
                // if (really_written != n_out ||  write(STDOUT_FILENO, &temp, 1) != 1)
                // {
                //     const char msg[] = "error: failed to write to stdout\n";
                //     write(STDERR_FILENO, msg, sizeof(msg));
                //     kill(child1, SIGKILL);
                //     kill(child2, SIGKILL);
                //     exit(EXIT_FAILURE);
                // }                
               
                // n_inp = read(STDIN_FILENO, input_string, sizeof(input_string));
            }
            // fprintf(stderr, "exit");
            char temp = '\n';
            // if(write(p1[1], &temp, 1) != 1){
            //     const char msg[] = "error: failed to write to pipe\n";
            //     write(STDERR_FILENO, msg, sizeof(msg));
            //     kill(child1, SIGKILL);
            //     kill(child2, SIGKILL);
            //     exit(EXIT_FAILURE);
            // }
            wait(&res_status);
        }
        int status1, status2;
                waitpid(child1, &status1, WNOHANG);
                waitpid(child2, &status2, WNOHANG);
                if (status1 != 0 || status2 != 0)
                {
                    if (status1 == 0){
                        kill(child1, SIGKILL);
                    } else {
                        kill(child2, SIGKILL);
                    }
                }
    }  

    return res_status;
}