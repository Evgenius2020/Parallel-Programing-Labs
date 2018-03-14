#include <math.h>
#include <stdlib.h>
#include <stdio.h>

float scalar_vector_x_vector(float *left, float *right, int size)
{
    float result = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        result += left[i] * right[i];
    }
    return result;
}

void vector_sub_vector(float *left, float *right, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        left[i] -= right[i];
    }
}

float vector_norm(float *vector, int size)
{
    float result = 0;
    int i;
    for (i = 0; i < size; i++)
    {
        result += pow(vector[i], 2);
    }
    return sqrt(result);
}

void vector_x_scalar(float *vector, float scalar, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        vector[i] *= scalar;
    }
}

void print_vector(float *vector, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        printf("%f ", vector[i]);
    }
    printf("\n");
}

float *init_vector(int size)
{
    return calloc(size, sizeof(float));
}