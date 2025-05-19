#include <stdio.h>

int sum(int a, int b) {
    int res;
    res = a + b;
    return res;
}

float mean(int a, int b) {
    float res;
    res = sum(a, b);
    res = res / 2.0;
    return res;
}

int main(int argc, char *argv[]) {
    int a, b, s;
    float m;

    a = 30;
    b = 1;
    s = sum(a, b);
    m = mean(a, b);

    printf("%d+%d=%d\n", a, b, s);
    printf("mean(%d, %d)=%.1f\n", a, b, m);

    return 0;
}