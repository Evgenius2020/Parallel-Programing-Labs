#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "vector_operations.h"

double scalar_vector_x_vector(double *left, double *right, int size)
{
    double result = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        result += left[i] * right[i];
    }
    return result;
}

void vector_sub_vector(double *left, double *right, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        left[i] -= right[i];
    }
}

double vector_norm(double *vector, int size)
{
    double result = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        result += pow(vector[i], 2);
    }
    return sqrt(result);
}

void vector_x_scalar(double *vector, double scalar, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        vector[i] *= scalar;
    }
}

void print_vector(double *vector, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        printf("%f ", vector[i]);
    }
    printf("\n");
}

double *init_vector(int size)
{
    return calloc(size, sizeof(double));
}