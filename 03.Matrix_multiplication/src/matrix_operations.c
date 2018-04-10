#include <stdio.h>
#include <stdlib.h>
#include "matrix_operations.h"

Matrix *create_matrix(unsigned height, unsigned width)
{
    Matrix *result = malloc(sizeof(Matrix));
    result->height = height;
    result->width = width;
    result->data = malloc(sizeof(double) * height * width);

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


double scalar_vector_x_vector(double *left, double *right, unsigned vector_size)
{
    double result = 0;
    unsigned i;
    for (i = 0; i < vector_size; i++)
    {
        result += left[i] * right[i];
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
        for (j = 0; j < transposed_right->height; j++)
        {
            result->data[i * result->width + j] = scalar_vector_x_vector(
                left->data + left->width * i,
                transposed_right->data + transposed_right->width * j,
                left->width);
        }
    }
}

void print_matrix(Matrix *matrix)
{
    unsigned i, j;
    for (i = 0; i < matrix->height; i++)
    {
        for (j = 0; j < matrix->width; j++)
        {
            printf("%f ", matrix->data[i * matrix->width + j]);
        }
        printf("\n");
    }
}