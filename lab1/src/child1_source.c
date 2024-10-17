#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argsc, char** args){
    char buf[4096];
    size_t bytes;
    while(bytes = read(STDIN_FILENO, buf, sizeof(buf))){
        if (buf[0] == '\n')
        {
            int written = write(STDOUT_FILENO, &buf[0], 1);
            if (written != 1)
            {
                const char msg[] = "error: failed to write to pipe\n";
                write(STDERR_FILENO, msg, sizeof(msg));
                exit(EXIT_FAILURE);
            }
            _exit(0);
        }
        if (bytes < 0) {
			const char msg[] = "error: failed to read from stdin\n";
			write(STDERR_FILENO, msg, sizeof(msg));
			exit(EXIT_FAILURE);
		}
        
        // fprintf(stderr, "%s\n", buf);
        buf[bytes - 1] = '\0';
        int n = strlen(buf);
        for (int i = 0; i < n; i++){
            buf[i] = toupper(buf[i]);
        }

        int written = write(STDOUT_FILENO, buf, strlen(buf));
        // fprintf(stderr, "c1write");
        // int written = bytes;
        if (written != strlen(buf)){
            const char msg[] = "error: failed to write to pipe\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }
    }
    
    return 0;
}