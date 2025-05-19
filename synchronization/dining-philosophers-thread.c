#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define N 20

int num_philosopher;
sem_t chopstick[N];

void *philosopher(void *in) {
    int id = (int)in;
    int i = 0;
    while(i++ < 5) {
        printf("PHILOSOPHER %d thinking...\n", id);
        sleep(1);
        
        printf("PHILOSOPHER %d wants to eat\n", id);
        
        if(id % 2 != 0) {
            sem_wait(&chopstick[id]);
            sem_wait(&chopstick[(id+1)%num_philosopher]);
        } else {
            sem_wait(&chopstick[(id+1)%num_philosopher]);
            sem_wait(&chopstick[id]);
        }
        
        printf("PHILOSOPHER %d eating\n", id);
        sleep(1);
        
        sem_post(&chopstick[id]);
        sem_post(&chopstick[(id+1)%num_philosopher]);
        printf("PHILOSOPHER %d ate\n", id);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t child_thread[N];
    
    if (argc != 2) {
        fprintf(stderr, "Missing Number of philosopher argument\n");
        exit(0);
    }

    num_philosopher = atoi(argv[1]);

    for(int i = 0 ; i < num_philosopher ; i++) {
        sem_init(&chopstick[i], 0, 1);
    }

    for(int i = 0 ; i < num_philosopher ; i++) {
        pthread_create(&child_thread[i], NULL, philosopher, (void *)i);
    }

    for(int i = 0 ; i < num_philosopher ; i++) {
        pthread_join(child_thread[i], NULL);
    }

    for(int i = 0 ; i < num_philosopher ; i++) {
        sem_destroy(&chopstick[i]);
    }

    return 0;
}