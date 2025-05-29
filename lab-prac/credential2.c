#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    printf("Before setuid:\n");
    printf("Real UID: %d\n", getuid());       // Get real user ID
    printf("Effective UID: %d\n", geteuid()); // Get effective user ID

    // Attempt to change the effective UID to root
    if (seteuid(getuid()) == 0) {
        printf("\nsetuid(0) successful!\n");
    } else {
        perror("\nsetuid failed");
    }

    printf("\nAfter setuid:\n");
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    return 0;
}

// set suid bit
// sudo chmod u+s <file> 
// unset suid bit
// sudo chmod u-s <file>
// change file owner
// sudo chown 