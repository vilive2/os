#include <stdio.h>
#include <string.h>


/* Adapted from the example given in the man page */
/* Using strtok on str1 will cause a segmentation fault because strtok
 * is supposed to overwrite delimiter bytes with '\0' (see man page).
 */
int main(int argc, char *argv[]) {
    char *str1 = "This is an example of tokens, separated by spaces and a comma.";
    char str2[] = "This is an example of tokens, separated by spaces and a comma.";
    char *cp;


    cp = strtok(str2, " ,");
    do
        puts(cp);
    while (NULL != (cp = strtok(NULL, " ,")));


    return 0;
}
