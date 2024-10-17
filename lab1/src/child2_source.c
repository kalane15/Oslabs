#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argsc, char **args)
{
    char buf[4096];
    size_t bytes;
    while (bytes = read(STDIN_FILENO, buf, sizeof(buf)))
    {
        if (buf[0] == '\n'){
            int written = write(STDOUT_FILENO, &buf[0], 1);
            if (written != 1)
            {
                const char msg[] = "error: failed to write to pipe\n";
                write(STDERR_FILENO, msg, sizeof(msg));
                exit(EXIT_FAILURE);
            }
            exit(0);
        }
        if (bytes < 0)
        {
            const char msg[] = "error: failed to read from stdin\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }
        // fprintf(stderr, "c2read %s", buf);
        buf[bytes] = '\0';
        int n = strlen(buf);
        char res[4096];
        strcpy(res, "");

        // fprintf(stderr,"%s", buf);
        for (int i = 0; i < n - 1; i++)
        {
            if (buf[i] == ' ' && buf[i + 1] == ' ')
            {
                i++;
            }
            else
            {
                strncat(res, &buf[i], 1);
            }
        }
        strncat(res, &buf[n - 1], 1);
        // fprintf(stderr, "%ld", bytes);
        // fprintf(stderr,"2 %s", res);

        int written = write(STDOUT_FILENO, res, strlen(res));
        // fprintf(stderr, "c2write %s end", res);
        if (written != strlen(res))
        {
            const char msg[] = "error: failed to write to pipe\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}