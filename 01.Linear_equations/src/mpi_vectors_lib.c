#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

void matrix_x_vector(int *matrix_part, int *vector, int *result, int size)
{
    int comm_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    result[comm_rank] = 0;

    for (int i = 0; i < size; i++)
    {
        result[comm_rank] += matrix_part[i] * vector[i];
    }

    MPI_Allgather(result + comm_rank, 1, MPI_INT, result, 1, MPI_INT, MPI_COMM_WORLD);
}

void print_vector(int *vector, int size)
{
    for (int j = 0; j < size; j++)
    {
        printf("%d ", vector[j]);
    }
    printf("\n");
}

int *init_vector(int start_value, int size)
{
    int *vector = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
    {
        vector[i] = start_value + i;
    }
    return vector;
}