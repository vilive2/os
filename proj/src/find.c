#define _GNU_SOURCE 
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

extern int match(const char *name, const char *expr, unsigned int opts);
extern void print(const char *dir, const char *d_name, char d_type);
extern const char *pattern;
extern unsigned int options;
extern char cwd[MAX_PATH_LEN];

int find(const char *dir) {
    int cwd_len = strlen(cwd);
    if (cwd_len == 0) {
        snprintf(cwd, MAX_PATH_LEN, "%s", dir);
    } else {
        snprintf(cwd+cwd_len, MAX_PATH_LEN-cwd_len, "/%s", dir);
    }

    int fd;
    char d_type;
    char buf[BUF_SIZE];
    long nread;
    struct linux_dirent *d;

    int found = 1;

    fd = open(cwd, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        fprintf(stderr, "%s Permission denied.\n", cwd);
        cwd[cwd_len] = '\0';
        return 1;
    }


    for (;;) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1) {
            break;
        }

        if (nread == 0)
            break;

        for (size_t bpos = 0 ; bpos < nread ; bpos += d->d_reclen) {
            d = (struct linux_dirent *) (buf + bpos);
            
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
                continue;
            d_type = *(buf + bpos + d->d_reclen - 1);

            unsigned int sat = 0;

            sat |= options & (1<<4);
            sat |= (match(d->d_name, pattern, options) << 0);
            if (options & (1<<1)) {
                sat |= (1<<1);
                if (d_type == DT_REG) sat |= (1<<2);
                else if (d_type == DT_DIR) sat |= (1<<3);
            }
            if (sat == options) {
                found = 0;
                print(cwd, d->d_name, d_type);
            }

            if (d_type == DT_DIR) {
                found = find(d->d_name) && found;
            }
        }
    }

    close(fd);
    cwd[cwd_len] = '\0';

    return found;
}