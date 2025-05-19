#include<stdlib.h>
#include<stdio.h>

int main(){

    char *bignum = "1098498287632090083681842";

    int *digit_count = (int *)malloc(9*sizeof(int));
	char c;
	int i = -1;
    
    while(i++ < 10) {
        digit_count[i] = 0;
    }
    
    printf("frequency of digits in big number %s\n", bignum);
    
    i = 0;
    while((c = bignum[i++]) != '\0') {	
		digit_count[c - '0']++;
	}

	for(i = 0; i < 10; i++) {
		printf("No. of '%d's in the bignum : %d\n", i, digit_count[i]);
    }
}