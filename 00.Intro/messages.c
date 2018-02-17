#include <mpi.h>
#include <stdio.h>

void main()
{
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char x = world_rank;
    MPI_Send(&x, 1, MPI_CHAR, (world_rank + 1)%world_size, 123, MPI_COMM_WORLD);
    MPI_Recv(&x, 1, MPI_CHAR, ((world_rank - 1) + world_size) % world_size, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("#%d %d\n", world_rank, x);

    MPI_Finalize();
}