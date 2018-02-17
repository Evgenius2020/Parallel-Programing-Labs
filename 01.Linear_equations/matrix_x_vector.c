#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define N 4

void print_vector(int *vector)
{
    for (int j = 0; j < N; j++)
    {
        printf("%d ", vector[j]);
    }
    printf("\n");
}

int *init_vector(int start_value)
{
    int *vector = malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++)
    {
        vector[i] = start_value + i;
    }
    return vector;
}

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int comm_size, comm_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    int *part = init_vector(comm_rank * N);
    int *vector = init_vector(0);
    int *result = malloc(sizeof(int) * N);
    result[comm_rank] = 0;

    for (int i = 0; i < N; i++)
    {
        result[comm_rank] += part[i] * vector[i];
    }

    printf("#%d: ", comm_rank);
    print_vector(part);
    
    MPI_Allgather(result + comm_rank, 1, MPI_INT, result, 1, MPI_INT, MPI_COMM_WORLD);

    printf("#%d ", comm_rank);
    print_vector(result);

    free(part);
    free(vector);
    free(result);
    MPI_Finalize();
}