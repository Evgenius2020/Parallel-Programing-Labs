#include <math.h>   // pow()
#include <stdlib.h> // abs()
#include <stdio.h>
#include "data_types.h"

void calculate_next_matrix(Parameters parameters, Local_Data local_data)
{
    double max_delta = 0;
    int i, j;
    for (i = 0; i < local_data.matrix_height; i++)
    {
        for (j = 0; j < local_data.matrix_width; j++)
        {
            double phi_center =
                local_data.phi_matrix[i * local_data.matrix_width + j];

            double phi_top =
                (i - 1 == -1)
                    ? local_data.Neighbors.top[j]
                    : local_data.phi_matrix[(i - 1) * local_data.matrix_width + j];
            double phi_bottom =
                (i + 1 == local_data.matrix_height)
                    ? local_data.Neighbors.bottom[j]
                    : local_data.phi_matrix[(i + 1) * local_data.matrix_width + j];

            double phi_left =
                (j - 1 == -1)
                    ? local_data.Neighbors.left[i]
                    : local_data.phi_matrix[i * local_data.matrix_width + (j - 1)];
            double phi_right =
                (j + 1 == local_data.matrix_width)
                    ? local_data.Neighbors.right[i]
                    : local_data.phi_matrix[i * local_data.matrix_width + (j + 1)];

            double phi_next =
                ((phi_left + phi_right - 2 * phi_center) / pow(parameters.x_step, 2) +
                 (phi_top + phi_bottom - 2 * phi_center) / pow(parameters.y_step, 2) -
                 local_data.rho_matrix[i * local_data.matrix_width + j]) /
                (2 / pow(parameters.x_step, 2) +
                 2 / pow(parameters.y_step, 2) +
                 parameters.a_coeff);

            local_data.phi_matrix_next[i * local_data.matrix_width + j] = phi_next;
        }
    }
}

double apply_next_matrix(Local_Data local_data)
{
    unsigned i, j;
    double max_delta = 0;
    for (i = 0; i < local_data.matrix_height; i++)
    {
        for (j = 0; j < local_data.matrix_width; j++)
        {
            double next_phi =
                local_data.phi_matrix_next[i * local_data.matrix_width + j];
            double *curr_phi =
                &(local_data.phi_matrix[i * local_data.matrix_width + j]);
            double delta = sqrt(pow(next_phi - *curr_phi, 2));
            if (delta > max_delta)
            {
                max_delta = delta;
            }
            *curr_phi = next_phi;
        }
    }

    return max_delta;
}

#define TOP_BOT_TAG 1
#define BOT_TOP_TAG 2
#define LEFT_RIGHT_TAG 3
#define RIGHT_LEFT_TAG 4

void resolve_neighbors(Cart_Data cart_data, Local_Data local_data,
                       MPI_Datatype column_datatype)
{
    // ====== Left neighbour. ===========================================================
    if (cart_data.Neighbors.left >= 0)
    {
        MPI_Send(local_data.phi_matrix, 1,
                 column_datatype, cart_data.Neighbors.left,
                 RIGHT_LEFT_TAG, cart_data.comm);
        MPI_Recv(local_data.Neighbors.left, local_data.matrix_height,
                 MPI_DOUBLE, cart_data.Neighbors.left,
                 LEFT_RIGHT_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------

    // ====== Right neighbour. ==========================================================
    if (cart_data.Neighbors.right >= 0)
    {
        MPI_Send(local_data.phi_matrix + local_data.matrix_width - 1, 1,
                 column_datatype, cart_data.Neighbors.right,
                 LEFT_RIGHT_TAG, cart_data.comm);
        MPI_Recv(local_data.Neighbors.right, local_data.matrix_height,
                 MPI_DOUBLE, cart_data.Neighbors.right,
                 RIGHT_LEFT_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------

    // ====== Top neighbour. ============================================================
    if (cart_data.Neighbors.top >= 0)
    {
        MPI_Send(local_data.phi_matrix, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbors.top,
                 BOT_TOP_TAG, cart_data.comm);
        MPI_Recv(local_data.Neighbors.top, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbors.top,
                 TOP_BOT_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------

    // ====== Bottom neighbour. =========================================================
    if (cart_data.Neighbors.bottom >= 0)
    {
        unsigned last_line = local_data.matrix_width * (local_data.matrix_height - 1);
        MPI_Send(local_data.phi_matrix + last_line, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbors.bottom,
                 TOP_BOT_TAG, cart_data.comm);
        MPI_Recv(local_data.Neighbors.bottom, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbors.bottom,
                 BOT_TOP_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------
}

void reduce_max_delta_phi(double *max_delta, MPI_Comm comm)
{
    double recv_buf;
    MPI_Allreduce(max_delta, &recv_buf, 1, MPI_DOUBLE, MPI_MAX, comm);
    *max_delta = recv_buf;
}

double step(Parameters parameters, Cart_Data cart_data, Local_Data local_data,
            MPI_Datatype column_datatype)
{
    resolve_neighbors(cart_data, local_data, column_datatype);
    calculate_next_matrix(parameters, local_data);
    double max_delta = apply_next_matrix(local_data);
    reduce_max_delta_phi(&max_delta, cart_data.comm);
    return max_delta;
}