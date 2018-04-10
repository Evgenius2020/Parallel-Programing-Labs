#include <stdio.h>
#include "matrix_operations.h"
#include "mpi.h"

void main(int argc, char *argv[])
{
    unsigned A_height = 4;
    unsigned A_width = 5;
    unsigned B_width = 4;

    // ====== Building 2d topology, splited into column- and row- subcomms. =============
    MPI_Init(&argc, &argv);
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
    unsigned A_working_part_height = A_height / comm_2d_sizes[1];
    unsigned B_working_part_width = B_width / comm_2d_sizes[0];
    Matrix *A_working_part = create_matrix(A_working_part_height, A_width);
    Matrix *B_working_part = create_matrix(B_working_part_width, A_width);
    Matrix *C_working_part = create_matrix(A_working_part_height, B_working_part_width);
    unsigned A_working_part_size = A_working_part_height * A_width;
    unsigned B_working_part_size = B_working_part_width * A_width;
    unsigned C_working_part_size = A_working_part_height * B_working_part_width;
    // ----------------------------------------------------------------------------------

    // ========= Initialization A and B in #0 proc. =====================================
    Matrix *A_initial = NULL;
    Matrix *B_initial = NULL;
    Matrix *B_initial_transposed = NULL;
    Matrix *C = NULL;
    if (process_id == 0)
    {
        A_initial = create_matrix(A_height, A_width);
        B_initial = create_matrix(A_width, B_width);
        C = create_matrix(A_height, B_width);

        unsigned i;
        for (i = 0; i < A_height * A_width; i++)
        {
            A_initial->data[i] = i;
        }
        for (i = 0; i < A_width * B_width; i++)
        {
            B_initial->data[i] = i;
        }
        B_initial_transposed = create_transposed_matrix(B_initial);
    }
    // ----------------------------------------------------------------------------------

    // ========= A and B distribution. ==================================================
    if (comm_2d_position[0] == 0)
    {
        double *data_source = A_initial ? A_initial->data : NULL;
        MPI_Scatter(data_source, A_working_part_size, MPI_DOUBLE, A_working_part->data,
                    A_working_part_size, MPI_DOUBLE, 0, comm_column);
    }
    if (comm_2d_position[1] == 0)
    {
        double *data_source = B_initial_transposed ? B_initial_transposed->data : NULL;
        MPI_Scatter(data_source, B_working_part_size, MPI_DOUBLE, B_working_part->data,
                    B_working_part_size, MPI_DOUBLE, 0, comm_line);
    }
    MPI_Bcast(A_working_part->data, A_working_part_size, MPI_DOUBLE, 0, comm_line);
    MPI_Bcast(B_working_part->data, B_working_part_size, MPI_DOUBLE, 0, comm_column);
    // ----------------------------------------------------------------------------------

    // ====== Distributed exucution A*B = C, gathering C into #0 proc. ==================
    matrix_x_transposed_matrix(A_working_part, B_working_part, C_working_part);
    MPI_Gather(C_working_part->data, C_working_part_size, MPI_DOUBLE, C ? C->data : NULL,
                  C_working_part_size, MPI_DOUBLE, 0, comm_2d);
    if (process_id == 0)
    {
        print_matrix(C);
    }
    // ----------------------------------------------------------------------------------

    // ========= Release memory. ========================================================
    if (process_id == 0)
    {
        delete_matrix(A_initial);
        delete_matrix(B_initial);
        delete_matrix(B_initial_transposed);
        delete_matrix(C);
    }
    delete_matrix(A_working_part);
    delete_matrix(B_working_part);
    delete_matrix(C_working_part);
    // ----------------------------------------------------------------------------------

    MPI_Finalize();
}