#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10
#define BUFFER_LEN 1024

typedef struct {
    char *from;
    char *to;
    long offset;
    long nbytes;
} CPDATA;

void *copy(void *in) {
    CPDATA data = *(CPDATA *)in;

    FILE *fromfp = fopen(data.from, "rb");
    FILE *tofp = fopen(data.to, "r+b");

    char buffer[BUFFER_LEN];
    int nchunks = data.nbytes / BUFFER_LEN;

    fseek(fromfp, data.offset, SEEK_SET);    
    fseek(tofp, data.offset, SEEK_SET); 
    
    for(int i = 0 ; i < nchunks ; i++) {
        fread(buffer, 1, BUFFER_LEN, fromfp);
        fwrite(buffer, 1, BUFFER_LEN, tofp);
    }

    int rem = data.nbytes % BUFFER_LEN;
    if(rem > 0) {
        fread(buffer, 1, rem, fromfp);
        fwrite(buffer, 1, rem, tofp);
    }

    fclose(fromfp);
    fclose(tofp);

    return NULL;
}


int main(int argc, char *argv[]) {
    pthread_t thread[N];
    CPDATA data[N];

    if(argc != 4) {
        fprintf(stderr, "invalid arg\n");
        exit(1);
    }

    int num_thread = atoi(argv[1]);

    clock_t start = clock();
    
    FILE *from = fopen(argv[2], "rb");
    FILE *to = fopen(argv[3], "wb");

    fseek(from, 0, SEEK_END);
    long file_size = ftell(from);
    fclose(from);
    fclose(to);

    printf("File size: %ld\n", file_size);

    long nbytes = (file_size + num_thread - 1) / num_thread;

    for(int i = 1 ; i < num_thread ; i++) {
        data[i].from = argv[2];
        data[i].to = argv[3];
        data[i].offset = (i-1)*nbytes;
        data[i].nbytes = nbytes;
        pthread_create(&thread[i], NULL, copy, (void *)&data[i]);
    }

    data[0].from = argv[2];
    data[0].to = argv[3];
    data[0].offset = (num_thread - 1) * nbytes;
    data[0].nbytes = file_size - (num_thread - 1) * nbytes;
    pthread_create(&thread[0], NULL, copy, (void *)&data[0]);

    for(int i = 0 ; i < num_thread ; i++) {
        pthread_join(thread[i], NULL);
    }
    
    clock_t end = clock();

    long timetaken = ((double)(end - start))*1000 / CLOCKS_PER_SEC;

    printf("Time taken to copy: %ld ms\n", timetaken);

    return 0;
}