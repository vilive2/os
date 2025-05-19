#include "../common.h"
#include<pthread.h>

typedef struct {
    int **mat;
    int row_num;
    int num_cols;
} data;

void *sum_row(void *in) {
    data d = *(data *)in;
    int *res = Malloc(1, int);
    *res = 0;

    for(int j = 0 ; j < d.num_cols ; j++)
        *res = *res + d.mat[d.row_num][j];

    printf("row %d: %d\n", d.row_num, *res);
    return (void *)res;
    // pthread_exit((void *)res);
}

int main() {
    int num_rows = 5, num_cols = 5;
    int **mat;
    matrix_alloc(mat,num_rows, num_cols, int);

    for(int i = 0 ; i < num_rows ; i++) {
        for(int j = 0 ; j < num_cols ; j++) {
            mat[i][j] = i*j;
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }

    void **row_sum = Malloc(5, void *);
    pthread_t *tids = Malloc(num_rows, pthread_t);

    data d[5];
    for(int i = 0 ; i < num_rows ; i++) {
        d[i].row_num = i;
        d[i].num_cols = num_cols;
        d[i].mat = mat;
        pthread_create(tids+i, NULL, sum_row, (void *)&d[i]);
    }

    for(int i = 0 ; i < num_rows ; i++) {
        pthread_join(tids[i], (void **)&row_sum[i]);
    }

    for(int i = 0 ; i < num_rows ; i++) printf("%d\n", *(int *)row_sum[i]);

    pthread_exit(NULL);
}