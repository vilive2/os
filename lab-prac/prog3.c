#include "common.h"

int main() {

    pid_t pid = fork();

    if(pid == 0) {
        int ret_val = execv("/bin/ls", NULL);
        if(ret_val < 0) 
            printf("ls didn't run.\n");
    } 
    if (pid > 0) {
        printf("CHILD pid = %d\n", pid);
        exit(0);
    }

    return 0;
}