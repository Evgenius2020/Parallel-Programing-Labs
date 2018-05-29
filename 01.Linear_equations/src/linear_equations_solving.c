#include "mpich/mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vector_operations.h"

#define EPSILON 0.1
#define TAU 0.001
#define SEND_RECV_TAG 123

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

void mpi_ring_send_recv(double *vector, unsigned size, unsigned process_id, unsigned process_count)
{
    MPI_Sendrecv_replace(vector, size, MPI_DOUBLE,
                         (process_id + process_count - 1) % process_count, SEND_RECV_TAG,
                         (process_id + 1) % process_count, SEND_RECV_TAG,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

double part_vector_norm(double *vector, unsigned part_size, unsigned process_id, unsigned process_count)
{
    double result = 0;

    unsigned i;
    for (i = 0; i < process_count; i++)
    {
        result += scalar_vector_x_vector(vector, vector, part_size);
        mpi_ring_send_recv(vector, part_size, process_id, process_count);
    }

    return sqrt(result);
}

void part_matrix_x_vector(double *matrix_part, double *vector_part, double *result, unsigned part_size, unsigned line_size, unsigned process_id, unsigned process_count)
{
    unsigned line;
    for (line = 0; line < part_size; line++)
    {
        result[line] = 0;
    }
    unsigned curr_part_process_id = process_id;
    do
    {
        for (line = 0; line < part_size; line++)
        {
            unsigned shift = line * line_size + curr_part_process_id * part_size;
            result[line] += scalar_vector_x_vector(matrix_part + shift, vector_part, part_size);
            mpi_ring_send_recv(vector_part, part_size, process_id, process_count);
        }
        curr_part_process_id = (curr_part_process_id + 1) % process_count;
    } while (curr_part_process_id != process_id);
}

void print_partial_vector(double *vector, unsigned part_size, unsigned process_id, unsigned process_count)
{
    unsigned i;
    unsigned j;

    for (i = 0; i < process_count; i++)
    {
        if (process_id == 0)
        {
            for (j = 0; j < part_size; j++)
            {
                printf("%f ", vector[j]);
            }
        }
        mpi_ring_send_recv(vector, part_size, process_id, process_count);
    }

    if (process_id == 0)
    {
        printf("\n");
    }
}

void solve_partial(unsigned N)
{
    unsigned process_count;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    unsigned process_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    unsigned part_size = calculate_part_size(N, process_count);
    unsigned N_extended = part_size * process_count;

    double *matrix_part = init_matrix_part(N, N_extended, part_size, process_id);
    double *x_part = init_vector(part_size);
    double *buf_part = init_vector(part_size);
    double *b_part = init_vector(part_size);
    unsigned i;
    for (i = 0; i < part_size; i++)
    {
        b_part[i] = (process_id * part_size + i < N) ? N + 1 : 0;
    }

    double b_norm = part_vector_norm(b_part, part_size, process_id, process_count);
    while(1)
    {
        part_matrix_x_vector(matrix_part, x_part, buf_part, part_size, N_extended, process_id, process_count);
        vector_sub_vector(buf_part, b_part, part_size);
        double buf_norm = part_vector_norm(buf_part, part_size, process_id, process_count);
        if (buf_norm / b_norm < EPSILON)
        {
            break;
        }
        vector_x_scalar(buf_part, TAU, part_size);
        vector_sub_vector(x_part, buf_part, part_size);
    }
    print_partial_vector(x_part, part_size, process_id, process_count);

    free(matrix_part);
    free(x_part);
    free(b_part);
    free(buf_part);
}