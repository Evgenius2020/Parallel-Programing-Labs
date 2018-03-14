#include <stdlib.h>
#include <omp.h>
#include "vector_operations.h"

#define N 10

#define EPSILON 0.001
#define TAU 0.0001

void main()
{
    float *A = init_vector(N * N);
    float *x = init_vector(N);
    float *b = init_vector(N);

    float *buf = init_vector(N);
    int i;

    for (i = 0; i < N * N; i++)
    {
        A[i] = 1;
    }
    for (i = 0; i < N; i++)
    {
        A[i * N + i] = 2;
    }

    for (i = 0; i < N; i++)
    {
        b[i] = N + 1;
    }

    float b_norm = vector_norm(b, N);
    while (1)
    {
        for (i = 0; i < N; i++)
        {
            buf[i] = scalar_vector_x_vector(A + N * i, x, N);
        }
        vector_sub_vector(buf, b, N);
        float buf_norm = vector_norm(buf, N);
        if (buf_norm / b_norm < EPSILON)
        {
            break;
        }
        vector_x_scalar(buf, TAU, N);
        vector_sub_vector(x, buf, N);
    }
    print_vector(x, N);

    free(A);
    free(x);
    free(b);
    free(buf);
}