#include "common.h"


int main() {
    int pid, x = 10;

    printf("Parent process before fork\n");
    printf("My pid is %d\n", getpid());

    pid = fork();

    if (pid == 0) {
        printf("CHILD: pid = %d\n", getpid());
        x++;
        printf("CHILD: returned pid value = %d; x = %d\n", pid, x);
        printf("CHILD: Current parent = %d\n", getppid());
        sleep(5);
        printf("CHILD: Updated(?) parent = %d\n", getppid());        
    }
    if (pid > 0) {
        printf("PARENT: pid = %d\n", getpid());
        printf("PARENT: my child's pid is %d\n", pid);
        printf("PARENT: x = %d\n", x);
    }

    return 0;
}
