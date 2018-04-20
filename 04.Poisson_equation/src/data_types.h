#ifndef DATA_TYPES
#define DATA_TYPES

#include <mpi.h>

typedef struct Parameters
{
    double x_start;
    double y_start;
    double x_range;
    double y_range;
    double x_step;
    double y_step;
    double (*phi)(double x, double y);
} Parameters;

typedef struct Cart_Data
{
    MPI_Comm comm;
    unsigned comm_id;
    unsigned comm_size;
    struct
    {
        unsigned x;
        unsigned y;
    } Size;
    struct
    {
        unsigned x;
        unsigned y;
    } Pos;
    struct
    {
        int top;
        int bottom;
        int left;
        int right;
    } Neighbours;
} Cart_Data;

typedef struct Local_Data
{
    double *rho_matrix;
    double *phi_matrix;
    unsigned matrix_height;
    unsigned matrix_width;
    struct
    {
        double *top;
        double *bottom;
        double *left;
        double *right;
    } Receive_Buffers;
} Local_Data;

#endif