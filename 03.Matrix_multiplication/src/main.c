#include <stdio.h>
#include <stdlib.h>
#include "matrix_operations.h"
#include "mpi.h"

void initialize_A_B(Matrix **A, Matrix **B, unsigned A_height, unsigned A_width, unsigned B_width)
{
    *A = create_matrix(A_height, A_width);
    *B = create_matrix(A_width, B_width);

    unsigned i;
    for (i = 0; i < (*A)->height * (*A)->width; i++)
    {
        (*A)->data[i] = i;
    }
    for (i = 0; i < (*B)->height * (*B)->width; i++)
    {
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
    unsigned comm2d_position[2];
    MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
    MPI_Dims_create(processes_count, 2, comm2d_sizes);
    MPI_Cart_create(MPI_COMM_WORLD, 2, comm2d_sizes, comm2d_torus_flags, 0, &comm2d);
    MPI_Cart_get(comm2d, 2, comm2d_sizes, comm2d_torus_flags, comm2d_position);

    unsigned A_height = 4;
    unsigned A_width = 5;
    unsigned B_width = 4;

    Matrix *A_working_part, *B_working_part, *C_working_part;
    unsigned A_working_part_height = A_height / comm2d_sizes[0];
    unsigned B_working_part_width = B_width / comm2d_sizes[1];
    unsigned A_working_part_size = A_working_part_height * A_width;
    unsigned B_working_part_size = B_working_part_width * A_width;

    if ((comm2d_position[0] == 0) && (comm2d_position[1] == 0))
    {
        Matrix *A_initial;
        Matrix *B_initial;
        initialize_A_B(&A_initial, &B_initial, A_height, A_width, B_width);
        Matrix *B_initial_transposed = create_transposed_matrix(B_initial);
        unsigned i, j;
        for (i = 0; i < comm2d_sizes[0]; i++)
        {
            for (j = 0; j < comm2d_sizes[1]; j++)
            {
                if ((i == 0) && (j == 0))
                {
                    continue;
                }
                unsigned receiving_process_id = comm2d_sizes[1] * i + j;
                // Sending parts
                MPI_Send(A_initial->data + A_working_part_size * i, A_working_part_size,
                         MPI_DOUBLE, receiving_process_id, 0, comm2d);
                MPI_Send(B_initial->data + B_working_part_size * j, B_working_part_size,
                         MPI_DOUBLE, receiving_process_id, 0, comm2d);
            }
        }
        A_working_part = create_line_submatrix(A_initial, A_width, 0, A_working_part_height);
        B_working_part = create_line_submatrix(B_initial_transposed, A_width,
                                               0, B_working_part_width);
        delete_matrix(A_initial);
        delete_matrix(B_initial);
        delete_matrix(B_initial_transposed);
    }
    else
    {
        // Receiving parts from (0,0)
        A_working_part = create_matrix(A_working_part_height, A_width);
        B_working_part = create_matrix(A_width, B_working_part_width);
        MPI_Recv(A_working_part->data, A_working_part_size, MPI_DOUBLE, 0, 0, comm2d, NULL);
        MPI_Recv(B_working_part->data, B_working_part_size, MPI_DOUBLE, 0, 0, comm2d, MPI_STATUS_IGNORE);
    }

    print_matrix(A_working_part);

    C_working_part = create_matrix(A_working_part_height, B_working_part_width);
    matrix_x_transposed_matrix(A_working_part, B_working_part, C_working_part);

    // Sending/collecting result.
    if ((comm2d_position[0] == 0) && (comm2d_position[1] == 0))
    {
        Matrix *C = create_matrix(A_height, B_width);
        put_submatrix(C, C_working_part, 0, 0);

        unsigned i, j;
        for (i = 0; i < comm2d_sizes[0]; i++)
        {
            for (j = 0; j < comm2d_sizes[1]; j++)
            {
                if ((i == 0) && (j == 0))
                {
                    continue;
                }
                MPI_Recv(C_working_part->data,
                         A_working_part_height * B_working_part_width,
                         MPI_DOUBLE, i * comm2d_sizes[1] + j, 0, comm2d, MPI_STATUS_IGNORE);
                put_submatrix(C, C_working_part,
                              i * B_working_part_width, j * A_working_part_height);
            }
        }
        print_matrix(C);

        delete_matrix(C);
    }
    else
    {
        MPI_Send(C_working_part->data, A_working_part_height * B_working_part_width,
                 MPI_DOUBLE, 0, 0, comm2d);
    }

    delete_matrix(A_working_part);
    delete_matrix(B_working_part);
    delete_matrix(C_working_part);
    MPI_Finalize();
}