#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi_vectors_lib.h"

#define N 4
#define EPSILON 10e-5
#define TAU 0.01

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int comm_size, comm_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    int part_size = N / comm_size;
    if (part_size < 1)
    {
        if (comm_rank == 0)
        {
            printf("Please set valid input data\n");
        }
        MPI_Finalize();
        return;
    }

    int *part = init_vector(part_size * N);
    for (int i = 0; i < part_size; i++)
    {
        for (int j = 0; j < N; j++)
        {
            // "2" for main diagonal elements.
            part[i * N + j] = (j == comm_rank * part_size + i) ? 2 : 1;
        }
    }
    int *x = init_vector(N);
    int *b = init_vector(N);
    for (int i = 0; i < N; b[i++] = N + 1)
        ;
    printf("#%d v\n", comm_rank);
    print_vector(part, part_size * N);

    free(part);
    free(x);
    free(b);
    MPI_Finalize();
}