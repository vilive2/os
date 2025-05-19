#include "../common.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>


#define NUM 5


sem_t *lock; // Being lazy




int race_condition(int pid, char *filename)
{
    int count, i;
    FILE *fp;
    if (NULL == (fp = fopen(filename, "r+")))
        ERR_MESG("semaphores: error opening file for second time");
    for (i = 0; i < NUM; i++) {
        if (ERROR == sem_wait(lock))
            ERR_MESG("sem_wait failed");
        fscanf(fp, "%d", &count);
        rewind(fp);
        fprintf(stderr, "Value read by %d: %d\n", pid, count);
#ifndef DEBUG
        sleep(1);
#endif

        (pid) ? count++ : count-- ;


        fprintf(fp, "%d\n", count);
        rewind(fp);
        fprintf(stderr, "Value written by %d: %d\n", pid, count);
        if (ERROR == sem_post(lock))
            ERR_MESG("sem_post failed");
#ifndef DEBUG
        sleep(rand() % 2*NUM);
#endif
    }
    fclose(fp);


    return 0;
}


int main(int ac, char *av[])
{
    char tmpfilename[BUF_LEN];
    int pid, status;
    FILE *fp;


    srand((int) time(NULL));


    strncpy(tmpfilename, "rcXXXXXX", BUF_LEN - 1);
    close(mkstemp(tmpfilename)); // V. lazy, don't do this!
    
    if (NULL == (lock = sem_open(tmpfilename, O_RDWR|O_CREAT, 0600, 1)))
        ERR_MESG("semaphores: error creating semaphore");


    if (NULL == (fp = fopen(av[1], "w")))
        ERR_MESG("semaphores: error opening file");
    fprintf(fp, "0\n");
    fclose(fp);
    
    if (ERROR == (pid = fork()))
        ERR_MESG("semaphores: fork failed");
    if (ERROR == race_condition(pid, av[1]))
        ERR_MESG("semaphores: error in race-condition() function");        
    if (pid && // parent
        (ERROR == wait(&status) ||
         ERROR == sem_unlink(tmpfilename) ||
         ERROR == unlink(tmpfilename)))
        ERR_MESG("semaphores: error cleaning up");


    return 0;
}
