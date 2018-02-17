#include <mpi.h>
#include <stdlib.h>
#include "mpi_vectors_lib.h"

#define N 4

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int comm_rank;        
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    int *part = init_vector(comm_rank * N, N);
    int *vector = init_vector(0, N);
    int *result = init_vector(0, N);

    matrix_x_vector(part, vector, result, N);
    print_vector(result, N);

    free(part);
    free(vector);
    free(result);
    MPI_Finalize();
}