#include "../common.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define NUM 5

int count;

void *inc(void *) {

    int i;
    for (i = 0; i < NUM; i++) {
        fprintf(stderr, "Value read by inc: %d\n", count);
        count++;
        fprintf(stderr, "Value written by inc: %d\n", count);
    }

    return NULL;
}

void *dec(void *) {
    int i;
    for (i = 0; i < NUM; i++) {
        fprintf(stderr, "Value read by dec: %d\n", count);
        count--;
        fprintf(stderr, "Value written by dec: %d\n", count);
        sleep(2);
    }

    return NULL;
}

int main(int ac, char *av[])
{

    count = 0;

    pthread_t inc_thread, dec_thread;
    pthread_create(&inc_thread, NULL, inc, NULL);
    pthread_create(&dec_thread, NULL, dec, NULL);

    pthread_join(inc_thread, NULL);
    pthread_join(dec_thread, NULL);

    printf("final count: %d\n", count);

    pthread_exit(NULL);    
}
