#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define N 2

char str1[] = "This is an example of tokens, separated by spaces and a comma. for thread1";
char str2[] = "This is an example of tokens, separated by spaces and a comma. for thread2";

void *printtok(void *id) {
    char *cp;
    if((int)id == 1)
        cp = strtok(str2, " ,");
    else 
        cp = strtok(str1, " ,");
    
    do
        printf("THREAD %d: %s\n", (int)id, cp);
    while (NULL != (cp = strtok(NULL, " ,")));

    return NULL;
}

int main(int argc, char *argv[]) {
    printf("str1: %s\nstr2: %s\n", str1, str2);

    pthread_t thread[N];

    for(int i = 0 ; i < N ; i++) {
        pthread_create(&thread[i], NULL, printtok, (void *)i);
    }

    for(int i = 0 ; i < N ; i++) {
        pthread_join(thread[i], NULL);
    }

    printf("str1: %s\nstr2: %s\n", str1, str2);

    pthread_exit((void *)0);
}
