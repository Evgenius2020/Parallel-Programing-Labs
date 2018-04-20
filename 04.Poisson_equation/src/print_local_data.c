#include <mpi.h>
#include <stdio.h>
#include "data_types.h"

void print_local_data(Cart_Data cart_data, Local_Data local_data)
{
    unsigned i, j, k;
    for (k = 0; k < cart_data.comm_size; k++)
    {
        MPI_Barrier(cart_data.comm);
        if (cart_data.Size.y * cart_data.Size.x + cart_data.Pos.x == k)
        {
            printf("\n=========Process #%d(%d;%d)==========\n",
                   cart_data.comm_id, cart_data.Pos.x, cart_data.Pos.y);
            printf("Top-neighbour is #%d\n", cart_data.Neighbours.top);
            printf("Bottom-neighbour is #%d\n", cart_data.Neighbours.bottom);
            printf("Left-neighbour is #%d\n", cart_data.Neighbours.left);
            printf("Right-neighbour is #%d\n", cart_data.Neighbours.right);
            printf("RHO-matrix:\n");
            for (i = 0; i < local_data.matrix_height; i++)
            {
                for (j = 0; j < local_data.matrix_width; j++)
                {
                    printf("%.2e ",
                           local_data.rho_matrix[i * local_data.matrix_width + j]);
                }
                printf("\n");
            }
            printf("\n");

            printf("send-receive buffers \n");

            printf("(l-receive, r-recieve):\n");
            for (i = 0; i < local_data.matrix_height; i++)
            {
                printf("%.2e ", local_data.Receive_Buffers.left[i]);
                printf("%.2e ", local_data.Receive_Buffers.right[i]);
                printf("\n");
            }
            printf("\n");

            printf("(t-receive, b-recieve):\n");
            for (i = 0; i < local_data.matrix_width; i++)
            {
                printf("%.2e ", local_data.Receive_Buffers.top[i]);
                printf("%.2e ", local_data.Receive_Buffers.bottom[i]);
                printf("\n");
            }
            printf("\n");
        }
    }
}