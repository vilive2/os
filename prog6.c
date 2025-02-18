#include "common.h"

int main() {

    int n = 5;
    int status;

    for(int i = 0 ; i < n ; i++) {
        pid_t pid = fork();
        if(pid == 0) {
            printf("CHILD : %d Terminated\n", getpid());
            exit(0);
        }
    }

    for(int i = 0 ; i < n ; i++) {
        pid_t pid = wait(&status);
        printf("Zombie Process %d cleaned up.\n", pid);
    }

    return 0;
}