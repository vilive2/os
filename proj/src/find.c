#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <string.h>
#include "mytypes.h"

extern int match(char *name, char *expr, char *opts);

int find(char *dir, char *target) {
    int fd;
    char d_type;
    char buf[BUF_SIZE];
    long nread;
    struct linux_dirent *d;

    int found = 0;

    fd = open(dir, O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        err(EXIT_FAILURE, "open");

    for (;;) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            err(EXIT_FAILURE, "read");

        if (nread == 0)
            break;

        for (size_t bpos = 0 ; bpos < nread ; bpos += d->d_reclen) {
            d = (struct linux_dirent *) (buf + bpos);
            
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;
            d_type = *(buf + bpos + d->d_reclen - 1);

            if (match(d->d_name, target, NULL)) {
                found = 1;
                printf("%s/%s\t%s\n", dir, d->d_name, (d_type == DT_REG) ? "regular" :
                                                          (d_type == DT_DIR) ? "directory" :
                                                          (d_type == DT_FIFO) ? "FIFO" :
                                                          (d_type == DT_SOCK) ? "socket" :
                                                          (d_type == DT_LNK) ? "symlink" :
                                                          (d_type == DT_BLK) ? "block dev" :
                                                          (d_type == DT_CHR) ? "char dev" : "???");
            }


            if (d_type == DT_DIR) {
                char nextDir[BUF_SIZE];
                sprintf(nextDir, "%s/%s", dir, d->d_name);
                found = found | find(nextDir, target);
            }
        }
    }

    close(fd);

    return found;
}