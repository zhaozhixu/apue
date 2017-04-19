#include "apue.h"

int
main(void) {
     printf("hello world from precess ID %ld\n", (long)getpid());
     exit(0);
}
