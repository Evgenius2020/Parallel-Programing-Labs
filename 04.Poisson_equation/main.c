#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

double phi(double x, double y)
{
    return 0.3 * pow(x, 3) + 0, 7 * pow(x, 2) - 0.4 * pow(y, 3);
}

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    double x_global_start = -1;
    double y_global_start = -1;
    double x_range_size = 2;
    double y_range_size = 2;
    double x_step = 0.25;
    double y_step = 0.25;

    unsigned processes_count, process_id;
    MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    // ====== Building 2d topology. =====================================================
    MPI_Comm comm_2d;
    unsigned comm_2d_sizes[2] = {0, 0};
    unsigned comm_2d_period_flags[2] = {0, 0};
    unsigned comm_2d_position[2];
    MPI_Dims_create(processes_count, 2, comm_2d_sizes);
    MPI_Cart_create(MPI_COMM_WORLD, 2, comm_2d_sizes, comm_2d_period_flags, 0, &comm_2d);
    MPI_Cart_get(comm_2d, 2, comm_2d_sizes, comm_2d_period_flags, comm_2d_position);
    // ----------------------------------------------------------------------------------

    // ====== Setting up operating range. ===============================================
    double x_local_range_size = x_range_size / comm_2d_sizes[0];
    double x_local_start = x_global_start + comm_2d_position[0] * x_local_range_size;

    double y_local_range_size = y_range_size / comm_2d_sizes[1];
    double y_local_start = y_global_start + comm_2d_position[1] * y_local_range_size;
    // ----------------------------------------------------------------------------------

    // ====== Building a rho-matrix. ====================================================
    unsigned rho_matrix_local_width = x_local_range_size / x_step;
    unsigned rho_matrix_local_heigth = y_local_range_size / y_step;
    double *rho_matrix = calloc(sizeof(double),
                                rho_matrix_local_width * rho_matrix_local_heigth);

    unsigned i, j;
    for (i = 0; i < rho_matrix_local_heigth; i++)
    {
        for (j = 0; j < rho_matrix_local_width; j++)
        {
            double x = x_local_start + j * x_step;
            double x_prev = x_local_start + (j - 1) * x_step;
            double x_next = x_local_start + (j + 1) * x_step;
            double y = y_local_start + i * y_step;
            double y_prev = y_local_start + (i - 1) * y_step;
            double y_next = y_local_start + (i + 1) * y_step;

            double phi_center = phi(x, y);
            double phi_left = phi(x_prev, y);
            double phi_right = phi(x_next, y);
            double phi_bottom = phi(x, y_next);
            double phi_top = phi(x, y_prev);

            rho_matrix[i * rho_matrix_local_width + j] =
                (phi_left + phi_right - 2 * phi_center) /
                    pow(x_step, 2) +
                (phi_bottom + phi_top - 2 * phi_center) /
                    pow(y_step, 2);
        }
    }
    // ----------------------------------------------------------------------------------

    // ====== Creating phi-matrix and exchange buffers. =================================
    double *phi_matrix = calloc(sizeof(double),
                                rho_matrix_local_width * rho_matrix_local_heigth);
    double *receive_buffer_top = calloc(sizeof(double), rho_matrix_local_width);
    double *receive_buffer_bottom = calloc(sizeof(double), rho_matrix_local_width);
    double *receive_buffer_left = calloc(sizeof(double), rho_matrix_local_heigth);
    double *receive_buffer_right = calloc(sizeof(double), rho_matrix_local_heigth);
    double *send_buffer_top = calloc(sizeof(double), rho_matrix_local_width);
    double *send_buffer_bottom = calloc(sizeof(double), rho_matrix_local_width);
    double *send_buffer_left = calloc(sizeof(double), rho_matrix_local_heigth);
    double *send_buffer_right = calloc(sizeof(double), rho_matrix_local_heigth);
    // ----------------------------------------------------------------------------------

    // ====== Creating MPI-vector for sending left-right buffers. =======================
    MPI_Datatype send_buffer_mpi_vector;
    MPI_Type_vector(rho_matrix_local_heigth, 1, rho_matrix_local_width, MPI_DOUBLE,
                    &send_buffer_mpi_vector);
    // ----------------------------------------------------------------------------------

    for (i = 0; i < rho_matrix_local_heigth; i++)
    {
        for (j = 0; j < rho_matrix_local_width; j++)
        {
            rho_matrix[i * rho_matrix_local_width + j] =
                rho_matrix_local_width * rho_matrix_local_heigth * process_id +
                i * rho_matrix_local_width + j;
        }
    }

    if (comm_2d_position[0] == 0)
    {
        for (i = 0; i < rho_matrix_local_width; i++)
        {
            receive_buffer_left[i] = -1;
        }
    }
    if (comm_2d_position[0] == comm_2d_sizes[0] - 1)
    {
        for (i = 0; i < rho_matrix_local_width; i++)
        {
            receive_buffer_right[i] = -2;
        }
    }
    if (comm_2d_position[1] == 0)
    {
        for (i = 0; i < rho_matrix_local_heigth; i++)
        {
            receive_buffer_top[i] = -3;
        }
    }
    if (comm_2d_position[1] == comm_2d_sizes[1] - 1)
    {
        for (i = 0; i < rho_matrix_local_heigth; i++)
        {
            receive_buffer_bottom[i] = -4;
        }
    }

    unsigned k;
    for (k = 0; k < processes_count; k++)
    {
        MPI_Barrier(comm_2d);
        if (process_id == k)
        {
            printf("\n=========Process #%d==========\n", process_id);
            printf("RHO-matrix:\n");
            for (i = 0; i < rho_matrix_local_heigth; i++)
            {
                for (j = 0; j < rho_matrix_local_width; j++)
                {
                    printf("%.2e ", rho_matrix[i * rho_matrix_local_width + j]);
                }
                printf("\n");
            }
            printf("\n");

            printf("send-receive buffers \n");

            printf("(l-send, l-receive, r-send, r-recieve):\n");
            for (i = 0; i < rho_matrix_local_heigth; i++)
            {
                printf("%.2e ", send_buffer_left[i]);
                printf("%.2e ", receive_buffer_left[i]);
                printf("%.2e ", send_buffer_right[i]);
                printf("%.2e ", receive_buffer_right[i]);
                printf("\n");
            }
            printf("\n");

            printf("(t-send, t-receive, b-send, b-recieve):\n");
            for (i = 0; i < rho_matrix_local_width; i++)
            {
                printf("%.2e ", send_buffer_top[i]);
                printf("%.2e ", receive_buffer_top[i]);
                printf("%.2e ", send_buffer_bottom[i]);
                printf("%.2e ", receive_buffer_bottom[i]);
                printf("\n");
            }
            printf("\n");
        }
    }

    free(rho_matrix);
    free(phi_matrix);
    free(receive_buffer_top);
    free(receive_buffer_bottom);
    free(receive_buffer_left);
    free(receive_buffer_right);
    free(send_buffer_top);
    free(send_buffer_bottom);
    free(send_buffer_left);
    free(send_buffer_right);

    MPI_Finalize();
}