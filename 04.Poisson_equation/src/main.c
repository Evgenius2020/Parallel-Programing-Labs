#include <mpi.h>
#include <stdio.h>
#include "initialization.h"
#include "step.h"
#include "data_types.h"

void finalize_local_data(Local_Data local_data);
void print_local_data(Cart_Data cart_data, Local_Data local_data);

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    Parameters parameters;
    Cart_Data cart_data;
    Local_Data local_data;
    MPI_Datatype column_datatype;
    initialize_parameters(&parameters);
    initialize_cart_data(&cart_data);
    initialize_local_data(parameters, cart_data, &local_data);
    MPI_Type_vector(local_data.matrix_height, 1, local_data.matrix_width,
                    MPI_DOUBLE, &column_datatype);
    MPI_Type_commit(&column_datatype);

    double max_delta;
    do
    {
        max_delta = step(parameters, cart_data, local_data, column_datatype);
        if (cart_data.comm_id == 0)
        {
            printf("%e\n", max_delta);
        }
        // Use it for debugging.
        //
        // print_local_data(cart_data, local_data);
    } while (parameters.convergence < max_delta);

    finalize_local_data(local_data);
    MPI_Type_free(&column_datatype);
    MPI_Finalize();
}