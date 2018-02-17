#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mpi_vectors_lib.h"

void matrix_x_vector(float *matrix_part, int part_size, float *vector, float *result, int size)
{
    int comm_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    for (int i = 0; i < part_size; i++)
    {
        int line = comm_rank * part_size + i;
        result[line] = 0;
        for (int j = 0; j < size; j++)
        {
            result[line] += matrix_part[i * size + j] * vector[j];
        }
    }
    MPI_Allgather(result + comm_rank * part_size, part_size, MPI_FLOAT, result, part_size, MPI_FLOAT, MPI_COMM_WORLD);
}

void vector_sub_vector(float *left, float *right, int size)
{
    for (int i = 0; i < size; i++)
    {
        left[i] -= right[i];
    }
}

float vector_norm(float *vector, int size)
{
    float result = 0;
    for (int i = 0; i < size; i++)
    {
        result += pow(vector[i], 2);
    }
    return sqrt(result);
}

void vector_x_scalar(float *vector, float scalar, int size)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] *= scalar;
    }
}

void print_vector(float *vector, int size)
{
    for (int j = 0; j < size; j++)
    {
        printf("%f ", vector[j]);
    }
    printf("\n");
}

float *init_vector(int size)
{
    return calloc(size, sizeof(float));
}