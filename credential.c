#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Real UID: %d\n", getuid());       // Gets the real user ID
    printf("Effective UID: %d\n", geteuid()); // Gets the effective user ID

    return 0;
}
