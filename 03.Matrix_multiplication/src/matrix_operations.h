typedef struct Matrix
{
    unsigned height;
    unsigned width;
    double *data;
} Matrix;

Matrix* create_matrix(unsigned height, unsigned width);
void delete_matrix(Matrix* matrix);
Matrix* create_transposed_matrix(Matrix* matrix);
Matrix* create_line_submatrix(Matrix* source, unsigned linesize, unsigned line_start, unsigned lines_count);
void *put_submatrix(Matrix *dest, Matrix *src, unsigned lines_start, unsigned column_start);
void matrix_x_transposed_matrix(Matrix *left, Matrix *transposed_right, Matrix *result);
void print_matrix(Matrix* matrix);