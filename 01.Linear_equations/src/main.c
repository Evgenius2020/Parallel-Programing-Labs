#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi_vectors_lib.h"

#define N 32
#define EPSILON 10e-7
#define TAU 0.01

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int comm_size, comm_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    int part_size = N / comm_size;
    if (part_size * comm_size != N)
    {
        if (comm_rank == 0)
        {
            printf("Please set valid input data\n");
        }
        MPI_Finalize();
        return;
    }

    float *part = init_vector(part_size * N);
    for (int i = 0; i < part_size * N; i++)
    {
        // "2" for main diagonal elements.
        part[i] = (i % N == comm_rank * part_size + i / N) ? 2 : 1;
    }
    float *x = init_vector(N);
    float *b = init_vector(N);
    for (int i = 0; i < N; b[i++] = N + 1)
        ;
    float *buf = init_vector(N);

    while (1)
    {
        matrix_x_vector(part, part_size, x, buf, N);
        vector_sub_vector(buf, b, N);
        float buf_norm = vector_norm(buf, N);
        float b_norm = vector_norm(b, N);
        if (buf_norm / b_norm < EPSILON)
        {
            break;
        }
        vector_x_scalar(buf, TAU, N);
        vector_sub_vector(x, buf, N);
    }
    if (comm_rank == 0)
    {
        print_vector(x, N);
    }

    free(part);
    free(x);
    free(b);
    MPI_Finalize();
}