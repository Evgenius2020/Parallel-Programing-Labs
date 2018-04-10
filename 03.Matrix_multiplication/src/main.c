#include <stdio.h>
#include <stdlib.h> // atoi
#include "matrix_operations.h"
#include "mpi.h"

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    // ====== Obtaining A and B sizes from args. ========================================
    if (argc <= 3)
    {
        printf("Please, enter A_height, A_width and B_width\n");
        MPI_Finalize();
        return;
    }
    unsigned A_height = atoi(argv[1]);
    unsigned A_width = atoi(argv[2]);
    unsigned B_width = atoi(argv[3]);
    // ----------------------------------------------------------------------------------

    // ====== Building 2d topology, splited into column- and row- subcomms. =============
    unsigned processes_count, process_id;
    MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    MPI_Comm comm_2d;
    unsigned comm_2d_sizes[2] = {0, 0};
    unsigned comm_2d_period_flags[2] = {0, 0};
    unsigned comm_2d_position[2];
    MPI_Dims_create(processes_count, 2, comm_2d_sizes);
    MPI_Cart_create(MPI_COMM_WORLD, 2, comm_2d_sizes, comm_2d_period_flags, 0, &comm_2d);
    MPI_Cart_get(comm_2d, 2, comm_2d_sizes, comm_2d_period_flags, comm_2d_position);

    MPI_Comm comm_line;
    MPI_Comm comm_column;
    unsigned comm_line_dims_flags[2] = {1, 0};
    unsigned comm_column_dims_flags[2] = {0, 1};
    MPI_Cart_sub(comm_2d, comm_line_dims_flags, &comm_line);
    MPI_Cart_sub(comm_2d, comm_column_dims_flags, &comm_column);
    // ----------------------------------------------------------------------------------

    // ======== Initializing working parts. =============================================
    unsigned A_part_height = A_height / comm_2d_sizes[1];
    unsigned B_part_width = B_width / comm_2d_sizes[0];
    Matrix *A_part = create_matrix(A_part_height, A_width);
    Matrix *B_part = create_matrix(B_part_width, A_width);
    Matrix *C_part = create_matrix(A_part_height, B_part_width);
    unsigned A_part_size = A_part_height * A_width;
    unsigned B_part_size = B_part_width * A_width;
    unsigned C_part_size = A_part_height * B_part_width;
    // ----------------------------------------------------------------------------------

    // ========= Initialization A and B in #0 proc. =====================================
    Matrix *A = NULL;
    Matrix *B = NULL;
    Matrix *B_transposed = NULL;
    Matrix *C = NULL;
    if (process_id == 0)
    {
        A = create_matrix(A_height, A_width);
        B = create_matrix(A_width, B_width);
        C = create_matrix(A_height, B_width);

        unsigned i;
        for (i = 0; i < A_height * A_width; i++)
        {
            A->data[i] = i;
        }
        for (i = 0; i < A_width * B_width; i++)
        {
            B->data[i] = i;
        }
        B_transposed = create_transposed_matrix(B);
    }
    // ----------------------------------------------------------------------------------

    // ========= A and B distribution. ==================================================
    if (comm_2d_position[0] == 0)
    {
        double *data_source = A ? A->data : NULL;
        MPI_Scatter(data_source, A_part_size, MPI_DOUBLE, A_part->data,
                    A_part_size, MPI_DOUBLE, 0, comm_column);
    }
    if (comm_2d_position[1] == 0)
    {
        double *data_source = B_transposed ? B_transposed->data : NULL;
        MPI_Scatter(data_source, B_part_size, MPI_DOUBLE, B_part->data,
                    B_part_size, MPI_DOUBLE, 0, comm_line);
    }
    MPI_Bcast(A_part->data, A_part_size, MPI_DOUBLE, 0, comm_line);
    MPI_Bcast(B_part->data, B_part_size, MPI_DOUBLE, 0, comm_column);
    // ----------------------------------------------------------------------------------

    // ====== Distributed exucution A*B = C, gathering C into #0 proc. ==================
    matrix_x_transposed_matrix(A_part, B_part, C_part);
    MPI_Gather(C_part->data, C_part_size, MPI_DOUBLE, C ? C->data : NULL,
               C_part_size, MPI_DOUBLE, 0, comm_2d);
    if (process_id == 0)
    {
        print_matrix(C);
    }
    // ----------------------------------------------------------------------------------

    // ========= Release memory. ========================================================
    if (process_id == 0)
    {
        delete_matrix(A);
        delete_matrix(B);
        delete_matrix(B_transposed);
        delete_matrix(C);
    }
    delete_matrix(A_part);
    delete_matrix(B_part);
    delete_matrix(C_part);
    // ----------------------------------------------------------------------------------

    MPI_Finalize();
}