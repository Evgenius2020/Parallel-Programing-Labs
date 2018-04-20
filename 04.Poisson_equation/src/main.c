#include <mpi.h>
#include <math.h>
#include "initialization.h"
#include "data_types.h"
#include <stdio.h>

double phi(double x, double y)
{
    return 0.3 * pow(x, 3) + 0, 7 * pow(x, 2) - 0.4 * pow(y, 3);
}

void finalize_local_data(Local_Data local_data);
void print_local_data(Cart_Data cart_data, Local_Data local_data);
void resolve_neighbours(Cart_Data cart_data, Local_Data local_data,
                        MPI_Datatype column_datatype);

void main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    Parameters parameters;
    Cart_Data cart_data;
    Local_Data local_data;
    MPI_Datatype column_datatype;
    initialize_parameters(&parameters, argc, argv, phi);
    initialize_cart_data(&cart_data);
    initialize_local_data(parameters, cart_data, &local_data);
    MPI_Type_vector(local_data.matrix_height, 1, local_data.matrix_width,
                    MPI_DOUBLE, &column_datatype);
    MPI_Type_commit(&column_datatype);

    resolve_neighbours(cart_data, local_data, column_datatype);
    printf("ffffffff\n");   
    print_local_data(cart_data, local_data);

    finalize_local_data(local_data);
    MPI_Type_free(&column_datatype);
    MPI_Finalize();
}