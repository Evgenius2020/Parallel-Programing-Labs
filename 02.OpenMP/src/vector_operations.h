void initialize_program(float **A, float **x, float **b, float **buf, int N);
void matrix_x_vector(float* matrix, float* vector, float** result, int vector_size);

float scalar_vector_x_vector(float *left, float *right, int vector_size);
void vector_sub_vector(float* left, float* right, int vector_size);
void vector_x_scalar(float *vector, float scalar, int vector_size);
float vector_norm(float *vector, int vector_size);
void print_vector(float *vector, int vector_size);
float *init_vector(int vector_size);