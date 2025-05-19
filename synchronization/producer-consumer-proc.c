#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/unistd.h>
#include <fcntl.h>

#define N 10
#define BUF_LEN 3

int *buf;
int in, out;
sem_t *full, *empty, *mutex;

void producer() {
    printf("PRODUCER STARTED\n");

    int a = 0, b = 1;
    for(int i = 0 ; i < N ; i++) {
        int tmp = a + b;
        a = b;
        b = tmp;
        
        sem_wait(empty);
        sem_wait(mutex);
        buf[in] = a;
        in = (in + 1) % BUF_LEN;
        sem_post(mutex);
        sem_post(full);
        sleep(1);
    }

    printf("PRODUCER FINISHED\n");
}

void consumer() {
    printf("CONSUMER STARTED\n");

    for(int i = 0 ; i < N ; i++) {
        sem_wait(full);
        sem_wait(mutex);
        int p = buf[out];
        out = (out + 1) % BUF_LEN;
        sem_post(mutex);
        sem_post(empty);

        printf("%d, ", p);
        sleep(2);
    }

    printf("CONSUMER FINISHED\n");
}

int main(int argc, char *argv[]) {
    in = 0;
    out = 0;

    mutex = sem_open("buf", O_CREAT | O_RDWR, 0600, 1);
    full = sem_open("full", O_CREAT | O_RDWR, 0600, 0);
    empty = sem_open("empty", O_CREAT | O_RDWR, 0600, BUF_LEN);

    int shmid = shmget(IPC_PRIVATE, BUF_LEN * sizeof(int), 0600 | IPC_CREAT);
    void *shm;

    int prd_pid, csm_pid;

    prd_pid = fork();

    if (prd_pid == 0) {
        shm = shmat(shmid, NULL, 0);
        buf = (int *)shm;
        producer();
        shmdt(shm);
    } else {
        csm_pid = fork();
        if (csm_pid == 0) {
            shm = shmat(shmid, NULL, 0);
            buf = (int *)shm;
            consumer();
            shmdt(shm);
        } else {
            int status;
            wait(&status);
            wait(&status);

            shmctl(shmid, IPC_RMID, NULL);
            sem_unlink("buf");
            sem_unlink("full");
            sem_unlink("empty");
            printf("\n");
        }
    }


    return 0;
}