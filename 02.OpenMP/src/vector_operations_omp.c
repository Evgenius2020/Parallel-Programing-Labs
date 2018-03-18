#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "vector_operations.h"

void initialize_program(float **A, float **x, float **b, float **buf, int N)
{
    *A = init_vector(N * N);
    *x = init_vector(N);
    *b = init_vector(N);
    *buf = init_vector(N);

    int i;

#pragma omp parallel for shared(A) private(i)
    for (i = 0; i < N * N; i++)
    {
        (*A)[i] = 1;
    }
#pragma omp parallel for shared(A) private(i)
    for (i = 0; i < N; i++)
    {
        (*A)[i * N + i] = 2;
    }
#pragma omp parallel for shared(b) private(i)
    for (i = 0; i < N; i++)
    {
        (*b)[i] = N + 1;
    }
}

void matrix_x_vector(float *matrix, float *vector, float **result, int vector_size)
{
    int i;
#pragma omp parallel for shared(result) private(i)
    for (i = 0; i < vector_size; i++)
    {
        float *matrix_line = matrix + vector_size * i;
        (*result)[i] = scalar_vector_x_vector(matrix_line, vector, vector_size);
    }
}

float scalar_vector_x_vector(float *left, float *right, int vector_size)
{
    float result = 0;
    int i;
    for (i = 0; i < vector_size; i++)
    {
        result += left[i] * right[i];
    }
    return result;
}

void vector_sub_vector(float *left, float *right, int vector_size)
{
    int i;
    for (i = 0; i < vector_size; i++)
    {
        left[i] -= right[i];
    }
}

float vector_norm(float *vector, int vector_size)
{
    float result = 0;
    int i;
    for (i = 0; i < vector_size; i++)
    {
        result += pow(vector[i], 2);
    }
    return sqrt(result);
}

void vector_x_scalar(float *vector, float scalar, int vector_size)
{
    int i;
    for (i = 0; i < vector_size; i++)
    {
        vector[i] *= scalar;
    }
}

void print_vector(float *vector, int vector_size)
{
    int i;
    for (i = 0; i < vector_size; i++)
    {
        printf("%f ", vector[i]);
    }
    printf("\n");
}

float *init_vector(int vector_size)
{
    return calloc(vector_size, sizeof(float));
}