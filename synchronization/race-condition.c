#include "../common.h"
#define NUM 10

int main(int argc, char *argv[]) {

    int i, count;
    FILE *fp = fopen("tmp.txt", "w+");
    pid_t pid = fork();

    for (i = 0; i < NUM; i++) {
        fscanf(fp, "%d", &count);
        rewind(fp);
        fprintf(stderr, "Value read by %d: %d\n", pid, count);
        sleep(1);

        (pid) ? count++ : count--;

        fprintf(fp, "%d\n", count);
        rewind(fp);
        fprintf(stderr, "Value written by %d: %d\n", pid, count);
        sleep(1);
    }
    return 0;
}