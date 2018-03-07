#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector_operations.h"

#define EPSILON 0.0001
#define TAU 0.00001

float *init_working_part(int part_size, int N, int comm_rank)
{
    float *part = init_vector(part_size * N);
    for (int i = 0; i < part_size * N; i++)
    {
        // "2" for main diagonal elements.)
        part[i] = (i % N == comm_rank * part_size + i / N) ? 2 : 1;
    }
    return part;
}

void matrix_x_vector(float *matrix_part, int part_size, float *vector, float *result, int size, int comm_rank)
{
    for (int i = 0; i < part_size; i++)
    {
        int line = comm_rank * part_size + i;
        result[line] = scalar_vector_x_vector(matrix_part + i * size, vector, size);
    }
    MPI_Allgather(result + comm_rank * part_size, part_size, MPI_FLOAT, result, part_size, MPI_FLOAT, MPI_COMM_WORLD);
}

void solve(int comm_size, int comm_rank, int part_size, int N)
{
    float *part = init_working_part(part_size, N, comm_rank);
    float *x = init_vector(N);
    float *b = init_vector(N);
    for (int i = 0; i < N; b[i++] = N + 1)
        ;
    float *buf = init_vector(N);

    float b_norm = vector_norm(b, N);
    while (1)
    {
        matrix_x_vector(part, part_size, x, buf, N, comm_rank);
        vector_sub_vector(buf, b, N);
        float buf_norm = vector_norm(buf, N);
        if (buf_norm / b_norm < EPSILON)
        {
            break;
        }
        vector_x_scalar(buf, TAU, N);
        vector_sub_vector(x, buf, N);
    }
    if (comm_rank == 0)
    {
        print_vector(x, N);
    }

    free(part);
    free(x);
    free(b);
}

void mpi_ring_send_recv(float *vector, int size, int tag, int comm_rank, int comm_size)
{
    MPI_Sendrecv_replace(vector, size, MPI_FLOAT,
                         (comm_rank + 1) % comm_size, 123, (comm_rank + comm_size - 1) % comm_size, 123,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

float part_vector_norm(float *vector, int size, int comm_rank, int comm_size)
{
    float result = 0;

    for (int i = 0; i < comm_size; i++)
    {
        result += scalar_vector_x_vector(vector, vector, size);
        mpi_ring_send_recv(vector, size, 123, comm_rank, comm_size);
    }

    return sqrt(result);
}

void part_matrix_x_vector(float *matrix_part, float *vector_part, float *result, int part_size, int line_size, int comm_rank, int comm_size)
{
    for (int part = 0; part < part_size; result[part++] = 0)
        ;
    int curr_part_n = comm_rank;
    do
    {
        for (int part = 0; part < part_size; part++)
        {
            int shift = part * line_size + curr_part_n * part_size;
            result[part] += scalar_vector_x_vector(matrix_part + shift, vector_part, part_size);
            mpi_ring_send_recv(vector_part, part_size, 3, comm_rank, comm_size);
        }
        curr_part_n = (curr_part_n + 1) % comm_size;
    } while (curr_part_n != comm_rank);
}

void solve_partial(int comm_size, int comm_rank, int part_size, int N)
{
    float *matrix_part = init_working_part(part_size, N, comm_rank);
    float *x_part = init_vector(part_size);
    float *b_part = init_vector(part_size);
    float *buf_part = init_vector(part_size);
    for (int i = 0; i < part_size; b_part[i++] = N + 1)
        ;

    float b_norm = part_vector_norm(b_part, part_size, comm_rank, comm_size);
    while (1)
    {
        part_matrix_x_vector(matrix_part, x_part, buf_part, part_size, N, comm_rank, comm_size);
        vector_sub_vector(buf_part, b_part, part_size);
        float buf_norm = part_vector_norm(buf_part, part_size, comm_rank, comm_size);
        if (buf_norm / b_norm < EPSILON)
        {
            break;
        }
        vector_x_scalar(buf_part, TAU, part_size);
        vector_sub_vector(x_part, buf_part, part_size);
    }
    print_vector(x_part, part_size);

    free(matrix_part);
    free(x_part);
    free(b_part);
    free(buf_part);
}