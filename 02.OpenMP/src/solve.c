#include <stdlib.h>

// This project have 2 different implementations
// of this methods (files are: "vector_operations.c"
// and "vector_operations_omp.c"). To compile, you must
// choose one of them.
#include "vector_operations.h"

#define EPSILON 0.001
#define TAU 0.00001

void solve(unsigned N)
{
    float *A, *x, *b, *buf;
    float b_norm, buf_norm;

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

    print_vector(x, N);

    free(A);
    free(x);
    free(b);
    free(buf);
}