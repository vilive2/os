#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "defs.h"

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

    int found = 1;
    DIR *dirp = opendir(cwd);
    struct dirent *entry;

    if (!dirp) {
        fprintf(stderr, "%s Permission denied.\n", cwd);
        cwd[cwd_len] = '\0';
        return 1;
    }

    for (;(entry = readdir(dirp));) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        unsigned int sat = 0;

        sat |= options & (1<<4);
        sat |= (match(entry->d_name, pattern, options) << 0);
        if (options & (1<<1)) {
            sat |= (1<<1);
            if (entry->d_type == DT_REG) sat |= (1<<2);
            else if (entry->d_type == DT_DIR) sat |= (1<<3);
        }
        if (sat == options) {
            found = 0;
            print(cwd, entry->d_name, entry->d_type);
        }

        if (entry->d_type == DT_DIR) {
            found = find(entry->d_name) && found;
        }
    }

    closedir(dirp);
    cwd[cwd_len] = '\0';

    return found;
}