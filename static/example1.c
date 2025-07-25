#include <stdio.h>

static int fn() {
  static int counter = 0;
  counter++;
  return counter;
}

int main(int argc, char *argv[]) {
  static int counter = 0;
  int rv = fn();
  printf("%d\n", counter);

  return 0;
}