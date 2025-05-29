#include <stdio.h>
#include <stdlib.h>
#include <err.h>

extern int find(char *dir, char *target);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!find(".", argv[1])) {
        err(EXIT_FAILURE, "'%s': No such file or directory\n", argv[1]);
    }

    exit(EXIT_SUCCESS);
}