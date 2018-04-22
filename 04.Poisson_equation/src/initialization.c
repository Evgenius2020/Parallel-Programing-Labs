#include <math.h>
#include <stdlib.h>
#include "initialization.h"

void initialize_cart_data(Cart_Data *cart_data)
{
    unsigned comm_size, comm_id;
    MPI_Comm comm_2d;
    unsigned comm_2d_sizes[2] = {0, 0};
    unsigned comm_2d_period_flags[2] = {0, 0};
    unsigned comm_2d_position[2];
    int comm_2d_top_neighbour;
    int comm_2d_bottom_neighbour;
    int comm_2d_left_neighbour;
    int comm_2d_right_neighbour;
    int cart_shift_stub;

    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_id);
    MPI_Dims_create(comm_size, 2, comm_2d_sizes);
    MPI_Cart_create(MPI_COMM_WORLD, 2, comm_2d_sizes, comm_2d_period_flags, 0, &comm_2d);
    MPI_Cart_get(comm_2d, 2, comm_2d_sizes, comm_2d_period_flags, comm_2d_position);
    MPI_Cart_shift(comm_2d, 1, -1, &cart_shift_stub, &comm_2d_top_neighbour);
    MPI_Cart_shift(comm_2d, 1, 1, &cart_shift_stub, &comm_2d_bottom_neighbour);
    MPI_Cart_shift(comm_2d, 0, -1, &cart_shift_stub, &comm_2d_left_neighbour);
    MPI_Cart_shift(comm_2d, 0, 1, &cart_shift_stub, &comm_2d_right_neighbour);

    cart_data->comm = comm_2d;
    cart_data->comm_id = comm_id;
    cart_data->comm_size = comm_size;
    cart_data->Size.x = comm_2d_sizes[0];
    cart_data->Size.y = comm_2d_sizes[1];
    cart_data->Pos.x = comm_2d_position[0];
    cart_data->Pos.y = comm_2d_position[1];
    cart_data->Neighbors.top = comm_2d_top_neighbour;
    cart_data->Neighbors.bottom = comm_2d_bottom_neighbour;
    cart_data->Neighbors.left = comm_2d_left_neighbour;
    cart_data->Neighbors.right = comm_2d_right_neighbour;
}

double phi(double x, double y)
{
    return pow(x, 2) + pow(y, 2);
}

void initialize_parameters(Parameters *parameters)
{
    parameters->x_start = -1;
    parameters->y_start = -1;
    parameters->x_range = 2;
    parameters->y_range = 2;
    parameters->x_step = 0.25;
    parameters->y_step = 0.25;
    parameters->convergence = 0.01;
    parameters->phi = phi;
}

void initialize_local_data(Parameters parameters, Cart_Data cart_data,
                           Local_Data *local_data)
{
    unsigned i, j;

    // ====== Setting up operating range. ===============================================
    double x_local_range_size = parameters.x_range / cart_data.Size.x;
    double x_local_start = parameters.x_start + cart_data.Size.x * x_local_range_size;
    double y_local_range_size = parameters.y_range / cart_data.Size.y;
    double y_local_start = parameters.y_start + cart_data.Size.y * y_local_range_size;
    unsigned matrix_width = x_local_range_size / parameters.x_step;
    unsigned matrix_height = y_local_range_size / parameters.y_step;
    // ----------------------------------------------------------------------------------

    // ====== Building a target matrix. =================================================
    double *target_matrix = calloc(sizeof(double), matrix_width * matrix_height);
    for (i = 0; i < matrix_height; i++)
    {
        for (j = 0; j < matrix_width; j++)
        {
            double x = x_local_start + j * parameters.x_step;
            double x_prev = x_local_start + (j - 1) * parameters.x_step;
            double x_next = x_local_start + (j + 1) * parameters.x_step;
            double y = y_local_start + i * parameters.y_step;
            double y_prev = y_local_start + (i - 1) * parameters.y_step;
            double y_next = y_local_start + (i + 1) * parameters.y_step;

            double phi_center = parameters.phi(x, y);
            double phi_left = parameters.phi(x_prev, y);
            double phi_right = parameters.phi(x_next, y);
            double phi_bottom = parameters.phi(x, y_next);
            double phi_top = parameters.phi(x, y_prev);

            target_matrix[i * matrix_width + j] =
                (phi_left + phi_right - 2 * phi_center) /
                    pow(parameters.x_step, 2) +
                (phi_bottom + phi_top - 2 * phi_center) /
                    pow(parameters.y_step, 2);
        }
    }
    // ----------------------------------------------------------------------------------

    // ====== Building working matrixes and exchange buffers. ===========================
    double *curr_matrix = calloc(sizeof(double), matrix_width * matrix_height);
    double *next_matrix = calloc(sizeof(double), matrix_width * matrix_height);
    double *receive_buffer_top = calloc(sizeof(double), matrix_width);
    double *receive_buffer_bottom = calloc(sizeof(double), matrix_width);
    double *receive_buffer_left = calloc(sizeof(double), matrix_height);
    double *receive_buffer_right = calloc(sizeof(double), matrix_height);
    // ----------------------------------------------------------------------------------

    // ====== Setting up default phi-values for edge points. ============================
    if (cart_data.Neighbors.left < 0)
    {
        double x = parameters.x_start - parameters.x_step;
        for (i = 0; i < matrix_height; i++)
        {
            double y = parameters.y_start + parameters.y_step * i;
            receive_buffer_left[i] = parameters.phi(x, y);
        }
    }
    if (cart_data.Neighbors.right < 0)
    {
        double x = parameters.x_start + parameters.x_range + parameters.x_step;
        for (i = 0; i < matrix_height; i++)
        {
            double y = parameters.y_start + parameters.y_step * i;
            receive_buffer_right[i] = parameters.phi(x, y);
        }
    }
    if (cart_data.Neighbors.top < 0)
    {
        double y = parameters.y_start - parameters.y_step;
        for (i = 0; i < matrix_width; i++)
        {
            double x = parameters.x_start + parameters.x_step * i;
            receive_buffer_top[i] = parameters.phi(x, y);
        }
    }
    if (cart_data.Neighbors.bottom < 0)
    {
        double y = parameters.y_start + parameters.y_range + parameters.y_step;
        for (i = 0; i < matrix_width; i++)
        {
            double x = parameters.x_start + parameters.x_step * i;
            receive_buffer_bottom[i] = parameters.phi(x, y);
        }
    }
    // ----------------------------------------------------------------------------------

    local_data->matrix_height = matrix_height;
    local_data->matrix_width = matrix_width;
    local_data->target_matrix = target_matrix;
    local_data->curr_matrix = curr_matrix;
    local_data->next_matrix = next_matrix;
    local_data->Neighbors.top = receive_buffer_top;
    local_data->Neighbors.bottom = receive_buffer_bottom;
    local_data->Neighbors.left = receive_buffer_left;
    local_data->Neighbors.right = receive_buffer_right;
}