#include <string.h>

int match(char *name, char *expr, char *opts) {
    return strcmp(name, expr) == 0;
}