#include "../common.h"
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {
    void *shm;
    int *shmarr;
    int shmid;
    int pid, i;
    shmid = shmget(IPC_PRIVATE, 1024, 0600);
    pid = fork();
    shm = shmat(shmid, NULL, 0);
    shmarr = (int *)shm;
    
    if(pid != 0) {
        shmctl(shmid, IPC_RMID, NULL);
        printf("PARENT : ");
        for(i = 0 ; i < 10 ; i++) printf("%d, ", shmarr[i]);
        printf("\n");

        for(i = 0 ; i < 10 ; i++) 
            shmarr[i] = i;
    } else {
        sleep(2);
        printf("CHILD : ");
        for(i = 0 ; i < 10 ; i++) printf("%d, ", shmarr[i]);
        printf("\n");
    }

    shmdt(shm);
    return 0;
}