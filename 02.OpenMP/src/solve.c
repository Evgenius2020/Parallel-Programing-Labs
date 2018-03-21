#include <stdlib.h>
#include "vector_operations.h"

#define EPSILON 0.001
#define TAU 0.00001

void initialize_program(double **A, double **x, double **b, double **buf, unsigned N);

void solve(unsigned N)
{
    double *A, *x, *b, *buf;
    double b_norm, buf_norm;

    initialize_program(&A, &x, &b, &buf, N);

    b_norm = vector_norm(b, N);
    while(1)
    {
        matrix_x_vector(A, x, &buf, N);
        vector_sub_vector(buf, b, N);
        buf_norm = vector_norm(buf, N);
        if ((buf_norm / b_norm) < EPSILON)
        {
            break;
        }
        vector_x_scalar(buf, TAU, N);
        vector_sub_vector(x, buf, N);
    }

#ifdef BUILD_OMP_LIGHT
#pragma omp single
#endif
    print_vector(x, N);

    free(A);
    free(x);
    free(b);
    free(buf);
}

void initialize_program(double **A, double **x, double **b, double **buf, unsigned N)
{
    *A = init_vector(N * N);
    *x = init_vector(N);
    *b = init_vector(N);
    *buf = init_vector(N);

    unsigned i;

#ifdef BUILD_OMP_HARD
#pragma omp parallel for shared(A) private(i)
#endif
    for (i = 0; i < N * N; i++)
    {
        (*A)[i] = 1;
    }

#ifdef BUILD_OMP_HARD
#pragma omp parallel for shared(A) private(i)
#endif
    for (i = 0; i < N; i++)
    {
        (*A)[i * N + i] = 2;
    }

#ifdef BUILD_OMP_HARD
#pragma omp parallel for shared(b) private(i)
#endif
    for (i = 0; i < N; i++)
    {
        (*b)[i] = N + 1;
    }
}