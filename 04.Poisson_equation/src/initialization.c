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
    return pow(x, 3) + pow(y, 3);
}

double rho(double x, double y)
{
    return 6 - 10e5 * phi(x, y);
}

void initialize_parameters(Parameters *parameters)
{
    parameters->x_start = -15;
    parameters->y_start = -15;
    parameters->x_range = 30;
    parameters->y_range = 30;
    parameters->x_step = 0.125;
    parameters->y_step = 0.125;
    parameters->convergence = 1e-8;
    parameters->a_coeff = 1e5;
    parameters->phi = phi;
    parameters->rho = rho;
}

void initialize_local_data(Parameters parameters, Cart_Data cart_data,
                           Local_Data *local_data)
{
    int i, j;

    // ====== Setting up operating range. ===============================================
    double x_local_range_size = parameters.x_range / cart_data.Size.x;
    double x_local_start = parameters.x_start + cart_data.Pos.x * x_local_range_size;
    double y_local_range_size = parameters.y_range / cart_data.Size.y;
    double y_local_start = parameters.y_start + cart_data.Pos.y * y_local_range_size;
    unsigned matrix_width = x_local_range_size / parameters.x_step;
    unsigned matrix_height = y_local_range_size / parameters.y_step;
    // ----------------------------------------------------------------------------------

    // ====== Pre-computing values of phi and rho. ======================================
    double *phi_matrix = calloc(sizeof(double), matrix_width * matrix_height);
    double *rho_matrix = calloc(sizeof(double), matrix_width * matrix_height);
    for (i = 0; i < matrix_height; i++)
    {
        for (j = 0; j < matrix_width; j++)
        {
            double x = x_local_start + j * parameters.x_step;
            double y = y_local_start + i * parameters.y_step;
            phi_matrix[i * matrix_width + j] = parameters.phi(x, y);
            rho_matrix[i * matrix_width + j] = parameters.rho(x, y);
        }
    }
    // ----------------------------------------------------------------------------------

    // ====== Building working matrixes and exchange buffers. ===========================
    double *curr_matrix = calloc(sizeof(double), matrix_width * matrix_height);
    double *receive_buffer_bottom = calloc(sizeof(double), matrix_width);
    double *receive_buffer_top = calloc(sizeof(double), matrix_width);
    double *receive_buffer_left = calloc(sizeof(double), matrix_height);
    double *receive_buffer_right = calloc(sizeof(double), matrix_height);
    // ----------------------------------------------------------------------------------

    // ====== Setting up default phi-values for edge points. ============================
    if (cart_data.Neighbors.left < 0)
    {
        double x = x_local_start - parameters.x_step;
        for (i = 0; i < matrix_height; i++)
        {
            double y = y_local_start + parameters.y_step * i;
            receive_buffer_left[i] = parameters.phi(x, y);
        }
    }
    if (cart_data.Neighbors.right < 0)
    {
        double x = x_local_start + x_local_range_size;
        for (i = 0; i < matrix_height; i++)
        {
            double y = y_local_start + parameters.y_step * i;
            receive_buffer_right[i] = parameters.phi(x, y);
        }
    }
    if (cart_data.Neighbors.top < 0)
    {
        double y = y_local_start - parameters.y_step;
        for (i = 0; i < matrix_width; i++)
        {
            double x = x_local_start + parameters.x_step * i;
            receive_buffer_top[i] = parameters.phi(x, y);
        }
    }
    if (cart_data.Neighbors.bottom < 0)
    {
        double y = y_local_start + y_local_range_size;
        for (i = 0; i < matrix_width; i++)
        {
            double x = x_local_start + parameters.x_step * i;
            receive_buffer_bottom[i] = parameters.phi(x, y);
        }
    }
    // ----------------------------------------------------------------------------------

    local_data->matrix_height = matrix_height;
    local_data->matrix_width = matrix_width;
    local_data->phi_matrix = phi_matrix;
    local_data->rho_matrix = rho_matrix;
    local_data->curr_matrix = curr_matrix;
    local_data->Neighbors.top = receive_buffer_top;
    local_data->Neighbors.bottom = receive_buffer_bottom;
    local_data->Neighbors.left = receive_buffer_left;
    local_data->Neighbors.right = receive_buffer_right;
}