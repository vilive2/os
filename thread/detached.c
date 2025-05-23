/*******************************************************************
* FILE: detached.c
* DESCRIPTION:
*   This example demonstrates how to explicitly create a thread in a
*   detached state. This might be done to conserve some system resources
*   if the thread never needs to join later. Compare with the join.c program
*   where the threads are created joinable.
* AUTHOR: 01/30/08 Blaise Barney
* LAST REVISED: 01/29/09
********************************************************************/
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define NTHREADS 4

void *busy_work(void *t) {
    long i, tid;
    double result=0.0;
    tid = (long)t;
    printf("Thread %ld starting...\n", tid);
    for (i=0; i<1000000; i++) {
        result = result + sin(i) * tan(i);
    }
    printf("Thread %ld done. Result = %e\n", tid, result);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NTHREADS];
    pthread_attr_t attr;
    int rc;
    long t;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    for(t=0; t<NTHREADS; t++) {
        printf("Main: creating thread %ld\n", t);
        rc = pthread_create(&threads[t], &attr, busy_work, (void *)t);
        if (rc) {
            printf("ERROR; return code from pthread_create is %d\n", rc);
            exit(-1);
        }
    }

    /* We're done with the attribute object, so we can destroy it */
    pthread_attr_destroy(&attr);

    /* The main thread is done, so we need to call pthread_exit explicitly to
    * permit the working threads to continue even after main completes.
    */
    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
}