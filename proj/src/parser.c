#include <string.h>
#include <stdlib.h>

extern unsigned int options;
extern const char *pattern;
extern const char *start_dir;
extern const char *program_name;
extern void help(int status);

void set_options(int size, const char *opt[]) {
    options |= (1<<0);
    for(int i = 0 ; i < size ; i++) {
        if (strcmp(opt[i], "-t") == 0) {
            options |= (1<<1);
            if (i+1 < size && strcmp(opt[i+1], "f") == 0) {
                options |= (1<<2);
            } else if (i+1 < size && strcmp(opt[i+1], "d") == 0) {
                options |= (1<<3);
            } else {
                help(EXIT_FAILURE);
            }
            i++;
        } else if (strcmp(opt[i], "-i") == 0) {
            options |= (1<<4);
        } else {
            help(EXIT_FAILURE);
        }
    }
}

void parse(int size, char *argv[]) {
    program_name = argv[0];

    if (size < 2) {
        help(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-h") == 0) {
        help(EXIT_SUCCESS);
    }

    pattern = argv[size - 1];
    size--;

    if (size > 1 && strncmp(argv[1], "-", 1) != 0) {
        char *pathend = argv[1]+(strlen(argv[1])-1);
        if (*pathend == '/') *pathend = '\0';
        start_dir = argv[1];
        set_options(size - 2, (const char **)(argv+2));
    } else {
        start_dir = ".";
        set_options(size - 1, (const char **)(argv+1));
    }
}