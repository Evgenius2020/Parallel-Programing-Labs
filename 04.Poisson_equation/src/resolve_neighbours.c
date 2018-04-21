#include "data_types.h"

#define TOP_BOT_TAG 1
#define BOT_TOP_TAG 2
#define LEFT_RIGHT_TAG 3
#define RIGHT_LEFT_TAG 4

void resolve_neighbours(Cart_Data cart_data, Local_Data local_data,
                        MPI_Datatype column_datatype)
{
    unsigned i;

    // ====== Left neighbour. ===========================================================
    if (cart_data.Neighbours.left < 0)
    {
        for (i = 0; i < local_data.matrix_height; i++)
        {
            local_data.Receive_Buffers.left[i] = -1;
        }
    }
    else
    {
        MPI_Send(local_data.phi_matrix, 1,
                 column_datatype, cart_data.Neighbours.left,
                 RIGHT_LEFT_TAG, cart_data.comm);
        MPI_Recv(local_data.Receive_Buffers.left, local_data.matrix_height,
                 MPI_DOUBLE, cart_data.Neighbours.left,
                 LEFT_RIGHT_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------

    // ====== Right neighbour. ==========================================================
    if (cart_data.Neighbours.right < 0)
    {
        for (i = 0; i < local_data.matrix_height; i++)
        {
            local_data.Receive_Buffers.right[i] = -2;
        }
    }
    else
    {
        MPI_Send(local_data.phi_matrix + local_data.matrix_width - 1, 1,
                 column_datatype, cart_data.Neighbours.right,
                 LEFT_RIGHT_TAG, cart_data.comm);
        MPI_Recv(local_data.Receive_Buffers.right, local_data.matrix_height,
                 MPI_DOUBLE, cart_data.Neighbours.right,
                 RIGHT_LEFT_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------

    // ====== Top neighbour. ============================================================
    if (cart_data.Neighbours.top < 0)
    {
        for (i = 0; i < local_data.matrix_width; i++)
        {
            local_data.Receive_Buffers.top[i] = -3;
        }
    }
    else
    {
        MPI_Send(local_data.phi_matrix, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbours.top,
                 BOT_TOP_TAG, cart_data.comm);
        MPI_Recv(local_data.Receive_Buffers.top, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbours.top,
                 TOP_BOT_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------

    // ====== Bottom neighbour. =========================================================
    if (cart_data.Neighbours.bottom < 0)
    {
        for (i = 0; i < local_data.matrix_width; i++)
        {
            local_data.Receive_Buffers.bottom[i] = -4;
        }
    }
    else
    {
        unsigned last_line = local_data.matrix_width * (local_data.matrix_height - 1);
        MPI_Send(local_data.phi_matrix + last_line, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbours.bottom,
                 TOP_BOT_TAG, cart_data.comm);
        MPI_Recv(local_data.Receive_Buffers.bottom, local_data.matrix_width,
                 MPI_DOUBLE, cart_data.Neighbours.bottom,
                 BOT_TOP_TAG, cart_data.comm, MPI_STATUS_IGNORE);
    }
    // ----------------------------------------------------------------------------------
}