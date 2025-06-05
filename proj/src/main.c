#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mytypes.h"

extern int find(const char *dir);
extern void help(int status);
extern void parse(int, char **);

char cwd[MAX_PATH_LEN];
const char *program_name = NULL;
const char *start_dir = NULL;
const char *pattern = NULL;
unsigned int options; 
/*
1<<0 : pattern match
1<<1 : type
1<<2 : file type
1<<3 : dir type
1<<4 : ignore case
*/

int main(int argc, char *argv[]) {
    assert(argc > 0);

    parse(argc, argv);
    cwd[0]='\0';
    
    if (find(start_dir)) {
        fprintf(stderr, "'%s': No such file or directory\n", pattern);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}