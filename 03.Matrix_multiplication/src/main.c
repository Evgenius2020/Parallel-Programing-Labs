#include <stdio.h>
#include "matrix_operations.h"

void main() {
    Matrix* matrix = create_matrix(4, 5);
    unsigned i;
    for(i = 0; i < matrix->height * matrix->width; i++) {
        matrix->data[i] = i;
    }
    print_matrix(matrix);
    printf("\n");

    Matrix* transposed = create_transposed_matrix(matrix);
    print_matrix(transposed);
    printf("\n");    

    Matrix* multiplication = create_matrix(4, 4);
    matrix_x_matrix(matrix, transposed, multiplication);
    print_matrix(multiplication);

    delete_matrix(multiplication);
    delete_matrix(transposed);
    delete_matrix(matrix);
}