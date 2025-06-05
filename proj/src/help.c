#include <stdio.h>
#include <stdlib.h>

extern const char *program_name;

void help(int status) {
    if (status != EXIT_SUCCESS) {
        fprintf (stderr, "Try '%s -h' for more information.\n", program_name);
        exit(status);
    }

    fprintf(stdout, "Usage: %s [path] [options] [pattern]\n", program_name);
    fputs("\npath is optional parameter.\n", stdout);
    fputs("\nSearch start from path.\n", stdout);
    fputs("\nCurrent directory is default path.\n", stdout);
    fputs("\npattern is required parameter.\n", stdout);

    exit(status);
}