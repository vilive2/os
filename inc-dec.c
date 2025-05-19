#include "common.h"

void increment(FILE *fp, int i) {
    int x;
    rewind(fp);
    x = 0;
    fscanf(fp, "%d", &x);
    x = x+i;
    rewind(fp);
    fprintf(fp, "%d", x);
}

void decrement(FILE *fp, int i) {
    int x;
    rewind(fp);
    x = 0;
    fscanf(fp, "%d", &x);
    x = x-i;
    rewind(fp);
    fprintf(fp, "%d", x);
}

int main() {

    FILE *fp;
    fp = fopen("dummy.txt", "r+");
    // rewind(fp);
    int x = 0;
    fprintf(fp, "%d", x);
    // exit(0);
    int status = fork();
    for(int i = 0 ; i < 10 ; i++) {
        if(status > 0) increment(fp, i);
        if(status == 0) decrement(fp, i);
    }
    return 0;
}