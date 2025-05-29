#include "common.h"

int main(int ac, char *av[])
{
    char x, *p = &x;

    while(1) {
        if (*p == '\0') putchar('\n');
        else putchar(*p);
        p++;
    }
    return 0;
}
