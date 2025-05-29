#include "common.h"
#include <sys/types.h>
#include <sys/wait.h>


#define BUFSIZE 1024
#define MAX_ARGS 1024
#define DEBUG 0


static int parse_command_line(char *, char **);


int main(int ac, char *av[])
{
    char inbuf[BUFSIZE], *args[MAX_ARGS];
    unsigned int size = BUFSIZE-1;
    int status, num_args;
    pid_t pid;


    while (1) {
        printf("tiny shell prompt$ ");
        if (NULL == fgets(inbuf, size, stdin))
            break;
        num_args = parse_command_line(inbuf, args);
        if (num_args < 0)
            ERR_MESG("failed to parse command line");
        if (num_args == 0)
            continue;
#if DEBUG
        int i;
        for (i = 0; i < num_args; i++)
            printf("%d %s\n", i, args[i]);
        continue;                
#endif
        if (0 > (pid = fork()))
            ERR_MESG("fork failed");
        if (pid == 0) {
            /* child */
            fprintf(stderr, "Child %d running\n", getpid());
            if (0 > execvp(args[0], args))
                fprintf(stderr, "Failed to execute command\n");
        }
        else {
            if (pid != wait(&status))
                ERR_MESG("error in wait");
            fprintf(stderr, "Child %d exited with status %d\n", pid, status);
        }


    }        


    return 0;
}




static int parse_command_line(char *inbuf, char **argv)
{
    unsigned int num_args = 1;


    if (NULL == (argv[0] = strtok(inbuf, " \t\n")))
        return 0;
    while (num_args < MAX_ARGS - 1 &&
           NULL != (argv[num_args++] = strtok(NULL, " \t\n")));
    argv[num_args] = (char *) NULL;
    return num_args;
}
