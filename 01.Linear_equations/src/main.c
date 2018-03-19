#include <mpi.h>
#include <stdlib.h>
#include "linear_equations_solving.h"

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    solve(atoi(argv[1]));
    // solve_partial(N);

    MPI_Finalize();
}