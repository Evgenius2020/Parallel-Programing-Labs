#include "mpich/mpi.h"
#include <stdlib.h>
#include <string.h>
#include "linear_equations_solving.h"

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    unsigned N;
    if (argc >= 2)
    {
        N = atoi(argv[1]);
        if ((argc == 3) && (strcmp(argv[2], "-p") == 0))
        {
            solve_partial(N);
        }
        else
        {
            solve(N);
        }
    }

    MPI_Finalize();
}