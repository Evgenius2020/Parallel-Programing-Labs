double *init_vector(unsigned vector_size);
void matrix_x_vector(double* matrix, double* vector, double** result, unsigned vector_size);
double scalar_vector_x_vector(double *left, double *right, unsigned vector_size);
void vector_sub_vector(double* left, double* right, unsigned vector_size);
void vector_x_scalar(double *vector, double scalar, unsigned vector_size);
double vector_norm(double *vector, unsigned vector_size);
void print_vector(double *vector, unsigned vector_size);