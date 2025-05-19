#include <stdio.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>

#define N 10
#define BUF_LEN 3

int *buf;
int in, out;
sem_t full, empty, mutex;

void *producer(void *) {
    printf("PRODUCER STARTED\n");

    int a = 0, b = 1;
    for(int i = 0 ; i < N ; i++) {
        int tmp = a + b;
        a = b;
        b = tmp;
        
        sem_wait(&empty);
        sem_wait(&mutex);
        buf[in] = a;
        in = (in + 1) % BUF_LEN;
        sem_post(&mutex);
        sem_post(&full);
        sleep(1);
    }

    printf("PRODUCER FINISHED\n");

    return NULL;
}

void *consumer(void *) {
    printf("CONSUMER STARTED\n");

    for(int i = 0 ; i < N ; i++) {
        sem_wait(&full);
        sem_wait(&mutex);
        int p = buf[out];
        out = (out + 1) % BUF_LEN;
        sem_post(&mutex);
        sem_post(&empty);

        printf("%d, ", p);
        sleep(2);
    }

    printf("CONSUMER FINISHED\n");

    return NULL;
}

int main(int argc, char *argv[]) {
    int shmid = shmget(IPC_PRIVATE, BUF_LEN * sizeof(int), 0600|IPC_CREAT);
    void *shm = shmat(shmid, NULL, 0);
    buf = (int *)shm;

    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUF_LEN);


    pthread_t prd_thread, csm_thread;

    pthread_create(&prd_thread, NULL, producer, NULL);
    pthread_create(&csm_thread, NULL, consumer, NULL);

    pthread_join(prd_thread, NULL);
    pthread_join(csm_thread, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}