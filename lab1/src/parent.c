#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <signal.h>

int main()
{
    char path1[4096] = "child1";
    char path2[4096] = "child2";
    int p1[2];
    int p2[2];
    int p_childrens[2];
    if (pipe(p1) != 0){
        perror("FAiled to open pipe\n");
        exit(EXIT_FAILURE);
    } // [1] - запись [0] - чтение
    if (pipe(p2) != 0){
        perror("FAiled to open pipe\n");
        exit(EXIT_FAILURE);
    } 
    if (pipe(p_childrens) != 0){
        perror("FAiled to open pipe\n");
        exit(EXIT_FAILURE);
    } 

    pid_t child1 = fork();
    int status;
    int res_status;
    if (child1 == -1)
    {
        perror("Failed to fork\n");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0)
    {
        // child
        if (dup2(p1[0], STDIN_FILENO) == -1){
            perror("Failed to dup\n");
            exit(EXIT_FAILURE);
        }
        if (dup2(p_childrens[1], STDOUT_FILENO) == -1){
            perror("Failed to dup\n");
            exit(EXIT_FAILURE);
        }
        close(p1[0]);
        close(p_childrens[1]);

        char *args[] = {NULL};
        status = execv(path1, args);

        if (status == -1)
        {
            const char msg[] = "error: failed to exec into new exectuable image\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        //
        // parent
        pid_t child2 = fork();
        if (child2 == -1)
        {
            kill(child1, SIGKILL);
            exit(EXIT_FAILURE);
        }
        if (child2 == 0)
        {
            if (dup2(p_childrens[0], STDIN_FILENO) == -1)
            {
                kill(child1, SIGKILL);
                perror("Failed to dup\n");
                exit(EXIT_FAILURE);
            }
            if (dup2(p2[1], STDOUT_FILENO) == -1)
            {
                kill(child1, SIGKILL);
                perror("Failed to dup\n");
                exit(EXIT_FAILURE);
            }
           close(p_childrens[0]);
           close(p2[1]);
           
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
            int n_inp = read(STDIN_FILENO, input_string, sizeof(input_string));
            int n_out;
            while (input_string[0] != '\n' && input_string[0] != EOF)
            {

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

                // fprintf(stderr, "p %s", input_string);
                input_string[n_inp - 1] = '\0';
                int really_written = write(p1[1], input_string, n_inp);

                if (really_written != n_inp){
                    perror("Failed to write to pipe\n");
                    kill(child1, SIGKILL);
                    kill(child2, SIGKILL);
                    exit(EXIT_FAILURE);
                }
                // fprintf(stderr, "wait");
                n_out = read(p2[0], output_string, sizeof(output_string));
                // fprintf(stderr, "parent read %s end", output_string);
                really_written = write(STDOUT_FILENO, output_string, n_out);
                char temp = '\n';

                if (really_written != n_out ||  write(STDOUT_FILENO, &temp, 1) != 1)
                {
                    const char msg[] = "error: failed to write to stdout\n";
                    write(STDERR_FILENO, msg, sizeof(msg));
                    kill(child1, SIGKILL);
                    kill(child2, SIGKILL);
                    exit(EXIT_FAILURE);
                }                
               
                n_inp = read(STDIN_FILENO, input_string, sizeof(input_string));
            }
            // fprintf(stderr, "exit");
            char temp = '\n';
            if(write(p1[1], &temp, 1) != 1){
                const char msg[] = "error: failed to write to pipe\n";
                write(STDERR_FILENO, msg, sizeof(msg));
                kill(child1, SIGKILL);
                kill(child2, SIGKILL);
                exit(EXIT_FAILURE);
            }
            wait(&res_status);
        }
    }
    return res_status;
}