#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <string.h>
#include "mytypes.h"

extern int match(char *name, char *expr, char *opts);
extern void print(char *dir, char *d_name, char d_type);

int find(char *dir, char *target) {
    int fd;
    char d_type;
    char buf[BUF_SIZE];
    long nread;
    struct linux_dirent *d;

    int found = 1;

    fd = open(dir, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        fprintf(stderr, "failed to open %s\n",dir);
        exit(1);
    }

    for (;;) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1) {
            fprintf(stderr, "read directory entry failed : %s\n", dir);
            exit(EXIT_FAILURE);
        }

        if (nread == 0)
            break;

        for (size_t bpos = 0 ; bpos < nread ; bpos += d->d_reclen) {
            d = (struct linux_dirent *) (buf + bpos);
            
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;
            d_type = *(buf + bpos + d->d_reclen - 1);

            if (match(d->d_name, target, NULL)) {
                found = 0;
                print(dir, d->d_name, d_type);
            }


            if (d_type == DT_DIR) {
                char nextDir[BUF_SIZE];
                sprintf(nextDir, "%s/%s", dir, d->d_name);
                found = find(nextDir, target) && found;
            }
        }
    }

    close(fd);

    return found;
}