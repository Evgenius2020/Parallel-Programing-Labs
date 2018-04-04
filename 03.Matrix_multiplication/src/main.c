#include <stdio.h>
#include <stdlib.h>
#include "matrix_operations.h"
#include "mpi.h"

void initialize_A_B(Matrix **A, Matrix **B)
{
    *A = create_matrix(4, 5);
    *B = create_matrix(5, 4);

    unsigned i;
    for (i = 0; i < (*A)->height * (*A)->width; i++)
    {
        (*A)->data[i] = i;
        (*B)->data[i] = i;
    }
}

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    unsigned processes_count;
    MPI_Comm comm2d;    
    unsigned comm2d_sizes[2] = {0, 0};
    unsigned comm2d_torus_flags[2] = {0, 0};
    MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
    MPI_Dims_create(processes_count, 2, comm2d_sizes);
    MPI_Cart_create(MPI_COMM_WORLD, 2, comm2d_sizes, comm2d_torus_flags, 0, &comm2d);

    unsigned cart_position[2];
    MPI_Cart_get(comm2d, 2, comm2d_sizes, comm2d_torus_flags, cart_position);

    Matrix *A_working_part, *B_working_part, *C_working_part;
    if ((cart_position[0] == 0) && (cart_position[1] == 0))
    {
        Matrix *A_initial;
        Matrix *B_initial;
        initialize_A_B(&A_initial, &B_initial);
        Matrix *B_initial_transposed = create_transposed_matrix(B_initial);
        delete_matrix(B_initial);
        unsigned i, j;
        for (i = 0; i < comm2d_sizes[0]; i++)
        {
            for (j = 0; j < comm2d_sizes[1]; j++)
            {
                if ((i == 0) && (j == 0))
                {
                    continue;
                }
                // Sending parts
            }
        }
        A_working_part = create_submatrix(A_initial, A_initial->width, 0, A_initial->height);
        B_working_part = create_submatrix(
            B_initial_transposed, B_initial_transposed->width, 0, B_initial_transposed->height);
        C_working_part = create_matrix(A_initial->height, B_initial_transposed->height);
        matrix_x_transposed_matrix(A_working_part, B_working_part, C_working_part);
        print_matrix(C_working_part);
    }
    else {
        // Receiving parts from (0,0)
    }

    MPI_Finalize();
}