#include <stdlib.h>
#include <string.h> // memcpy
#include "matrix_operations.h"
#include "vector_operations.h"

Matrix *create_matrix(unsigned height, unsigned width)
{
    Matrix *result = malloc(sizeof(Matrix));
    result->height = height;
    result->width = width;
    result->data = malloc(sizeof(double) * height * width);

    return result;
}

Matrix *create_matrix_initialized(unsigned height, unsigned width, double *data)
{
    Matrix *result = malloc(sizeof(Matrix));
    result->height = height;
    result->width = width;
    result->data = data;

    return result;
}

void delete_matrix(Matrix *matrix)
{
    free(matrix->data);
    free(matrix);
}

Matrix *create_transposed_matrix(Matrix *matrix)
{
    Matrix *result = create_matrix(matrix->width, matrix->height);

    unsigned i, j;
    for (i = 0; i < result->height; i++)
    {
        for (j = 0; j < result->width; j++)
        {
            result->data[i * result->width + j] = matrix->data[j * matrix->width + i];
        }
    }

    return result;
}

/// Pre-conditions:
/// 1) left->width == left->height.
/// 2) result->height and result->width is initialized.
/// 2) result->data is allocated correctly.
void matrix_x_transposed_matrix(Matrix *left, Matrix *transposed_right, Matrix *result)
{
    unsigned i, j;
    for (i = 0; i < left->height; i++)
    {
        for (j = 0; j < transposed_right->width; j++)
        {
            result->data[i * result->width + j] = scalar_vector_x_vector(
                left->data + left->width * i,
                transposed_right->data + transposed_right->width * j,
                left->width);
        }
    }
}

Matrix *create_submatrix(Matrix *source, unsigned line_size, unsigned lines_start, unsigned lines_count)
{
    double *data = malloc(sizeof(double) * line_size * lines_count);
    memcpy(
        data, 
        source->data + line_size * lines_start, 
        line_size * lines_count * sizeof(double));
    return create_matrix_initialized(lines_count, line_size, data);
}

void print_matrix(Matrix *matrix)
{
    unsigned i;
    for (i = 0; i < matrix->height; i++)
    {
        print_vector(matrix->data + i * matrix->width, matrix->width);
    }
}