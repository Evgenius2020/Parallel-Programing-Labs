#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "vector_operations.h"

void matrix_x_vector(double *matrix, double *vector, double **result, unsigned vector_size)
{
    unsigned i;
    for (i = 0; i < vector_size; i++)
    {
        double *matrix_line = matrix + vector_size * i;
        (*result)[i] = scalar_vector_x_vector(matrix_line, vector, vector_size);
    }
}

double scalar_vector_x_vector(double *left, double *right, unsigned vector_size)
{
    double result = 0;
    unsigned i;
    for (i = 0; i < vector_size; i++)
    {
        result += left[i] * right[i];
    }
    return result;
}

void vector_sub_vector(double *left, double *right, unsigned vector_size)
{
    unsigned i;
    for (i = 0; i < vector_size; i++)
    {
        left[i] -= right[i];
    }
}

double vector_norm(double *vector, unsigned vector_size)
{
    double result = 0;
    unsigned i;
    for (i = 0; i < vector_size; i++)
    {
        result += pow(vector[i], 2);
    }
    return sqrt(result);
}

void vector_x_scalar(double *vector, double scalar, unsigned vector_size)
{
    unsigned i;
    for (i = 0; i < vector_size; i++)
    {
        vector[i] *= scalar;
    }
}

void print_vector(double *vector, unsigned vector_size)
{
    unsigned i;
    for (i = 0; i < vector_size; i++)
    {
        printf("%f ", vector[i]);
    }
    printf("\n");
}

double *init_vector(unsigned vector_size)
{
    return calloc(vector_size, sizeof(double));
}