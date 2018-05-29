#ifndef DATA_TYPES
#define DATA_TYPES

#include "mpich/mpi.h"

typedef struct Parameters
{
    double x_start;
    double y_start;
    double x_range;
    double y_range;
    double x_step;
    double y_step;
    double convergence;
    double a_coeff;
    double (*rho)(double x, double y);
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
        int x;
        int y;
    } Pos;
    struct
    {
        int top;
        int bottom;
        int left;
        int right;
    } Neighbors;
} Cart_Data;

typedef struct Local_Data
{
    double *phi_matrix;
    double *phi_matrix_next;    
    double *rho_matrix;
    unsigned matrix_height;
    unsigned matrix_width;
    // ===== Start point of working range. ==============================================
    double x_start;
    double y_start;
    // ----------------------------------------------------------------------------------
    struct
    {
        double *top;
        double *bottom;
        double *left;
        double *right;
    } Neighbors;
} Local_Data;

#endif