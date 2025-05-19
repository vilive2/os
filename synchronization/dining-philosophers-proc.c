#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

#define N 20

int num_philosopher;
sem_t *chopstick[N];

void philosopher(int id) {
    int i = 0;
    while(i++ < 5) {
        printf("PHILOSOPHER %d thinking...\n", id);
        sleep(1);
        
        printf("PHILOSOPHER %d wants to eat\n", id);
        
        if(id % 2 != 0) {
            sem_wait(chopstick[id]);
            sem_wait(chopstick[(id+1)%num_philosopher]);
        } else {
            sem_wait(chopstick[(id+1)%num_philosopher]);
            sem_wait(chopstick[id]);
        }
        
        printf("PHILOSOPHER %d eating\n", id);
        sleep(1);
        
        sem_post(chopstick[id]);
        sem_post(chopstick[(id+1)%num_philosopher]);
        printf("PHILOSOPHER %d ate\n", id);
    }
}

int main(int argc, char *argv[]) {
    pid_t child_pid[N];
    char name[N];

    if (argc != 2) {
        fprintf(stderr, "Missing Number of philosopher argument\n");
        exit(0);
    }

    num_philosopher = atoi(argv[1]);

    for(int i = 0 ; i < num_philosopher ; i++) {
        sprintf(name, "chop%d", i);
        chopstick[i] = sem_open(name, O_CREAT | O_RDWR, 0600, 1);
    }

    for(int i = 0 ; i < num_philosopher ; i++) {
        child_pid[i] = fork();
        if(!child_pid[i]) {
            philosopher(i);
            exit(0);
        }
    }


    for(int i = 0 ; i < num_philosopher ; i++) {
        wait(NULL);
    }

    for(int i = 0 ; i < num_philosopher ; i++) {
        sprintf(name, "chop%d", i);
        sem_unlink(name);
    }

    return 0;
}