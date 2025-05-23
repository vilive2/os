#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define ERROR -1
#define INVALID ERROR
#define FAILURE ERROR
#define UNDEF ERROR

#define BUF_LEN 256
#define INFTY 1000000
#define EPS 0.0001

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef SQR
#define SQR(x) ((x) * (x))
#endif

#define ERR_MESG(x) { perror(x); exit(1); }
#if DEBUG
//#ifdef __GNUC__
#define dprintf(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
#define dprintf(format, ...)
#endif

#define TIMESTAMP(x) {                                                  \
  struct timespec _tp_;                                                 \
  if (0 > clock_gettime(CLOCK_REALTIME, &_tp_))                         \
      ERR_MESG("clock_gettime failed");                                 \
  fprintf(stderr, "%s %llu\n", x, (_tp_.tv_sec * 1000000000 + _tp_.tv_nsec) % 100000000); \
}

#define Malloc(n,type) (type *) malloc( (unsigned) ((n)*sizeof(type)))
#define Realloc(loc,n,type) (type *) realloc( (char *)(loc), \
                                              (unsigned) ((n)*sizeof(type)))

#define matrix_alloc(mat,rows,cols,type) {                        \
    int ii;                                                       \
    type *temp;                                                   \
                                                                  \
    if (NULL == (temp = (type *) malloc(rows*cols*sizeof(type))) || \
        NULL == (mat = (type **) malloc(rows * sizeof(type *))))  \
        ERR_MESG("Out of memory");                                \
    for (ii = 0; ii < rows; temp += cols, ii++)                   \
        mat[ii] = temp;                                           \
  }

#define matrix_free(mat) {                      \
    free(mat[0]);                               \
    free(mat);                                  \
  }

#endif