typedef struct Matrix
{
    unsigned height;
    unsigned width;
    double *data;
} Matrix;

Matrix* create_matrix(unsigned height, unsigned width);
void delete_matrix(Matrix* matrix);
Matrix* create_transposed_matrix(Matrix* matrix);
void matrix_x_transposed_matrix(Matrix *left, Matrix *transposed_right, Matrix *result);
void print_matrix(Matrix* matrix);