#include <stdio.h>
#include <stdlib.h>

extern int find(char *dir, char *target);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        exit(1);
    }

    if (find(".", argv[1])) {
        fprintf(stderr, "'%s': No such file or directory\n", argv[1]);
        exit(1);
    }

    exit(0);
}