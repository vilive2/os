#include "common.h"

int main() {
    for(int i = 0 ; i < 10 ; i++) {
        pid_t pid = fork();
        if(pid == 0) {
            printf("CHILD = %d\n", getpid());
            exit(0);
        }
    }
    return 0;
}