#include <mpi.h>
#include <stdio.h>
#include "linear_equations_solving.h"

#define N 32
#define EPSILON 10e-5
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

    //  solve(comm_size, comm_rank, part_size, N, TAU, EPSILON);
    solve_partial(comm_size, comm_rank, part_size, N, TAU, EPSILON);

    MPI_Finalize();
}