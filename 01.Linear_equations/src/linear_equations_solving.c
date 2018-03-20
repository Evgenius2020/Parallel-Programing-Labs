#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vector_operations.h"

#define EPSILON 0.01
#define TAU 0.00001

unsigned calculate_part_size(unsigned N, unsigned process_count)
{
    if (N % process_count == 0)
    {
        return N / process_count;
    }
    else
    {
        return N / process_count + 1;
    }
}

double *init_matrix_part(unsigned N, unsigned N_extended, unsigned part_size, unsigned process_id)
{
    double *matrix_part = init_vector(N_extended * part_size);
    unsigned line;
    unsigned i;
    for (line = 0; line < part_size; line++)
    {
        if (N > process_id * part_size + line)
        {
            for (i = 0; i < N; i++)
            {
                matrix_part[N_extended * line + i] = (i == process_id * part_size + line) ? 2 : 1;
            }
        }
    }
    return matrix_part;
}

void matrix_x_vector(double *matrix_part, unsigned part_size, double *vector, double *allgather_buf, double *result, unsigned N, unsigned process_id)
{
    unsigned line;
    for (line = 0; line < part_size; line++)
    {
        unsigned line_index = process_id * part_size + line;
        allgather_buf[line_index] = scalar_vector_x_vector(matrix_part + line * N, vector, N);
    }

    MPI_Allgather(allgather_buf + process_id * part_size, part_size, MPI_DOUBLE, result, part_size, MPI_DOUBLE, MPI_COMM_WORLD);
}

void solve(unsigned N)
{
    unsigned process_count;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    unsigned process_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    unsigned part_size = calculate_part_size(N, process_count);
    unsigned N_extended = part_size * process_count;
    double *matrix_part = init_matrix_part(N, N_extended, part_size, process_id);
    double *x = init_vector(N_extended);
    double *buf = init_vector(N_extended);
    double *b = init_vector(N_extended);
    unsigned i;
    for (i = 0; i < N; i++)
    {
        b[i] = N + 1;
    }
    double *allgather_buf = init_vector(N_extended); // Intel compiler specific.

    double b_norm = vector_norm(b, N_extended);
    double buf_norm;

    while (1)
    {
        matrix_x_vector(matrix_part, part_size, x, allgather_buf, buf, N_extended, process_id);
        vector_sub_vector(buf, b, N_extended);
        buf_norm = vector_norm(buf, N_extended);
        if (buf_norm / b_norm < EPSILON)
        {
            break;
        }
        vector_x_scalar(buf, TAU, N_extended);
        vector_sub_vector(x, buf, N_extended);
    }
    if (process_id == 0)
    {
        print_vector(x, N);
    }

    free(matrix_part);
    free(x);
    free(b);
    free(buf);
    free(allgather_buf);
}