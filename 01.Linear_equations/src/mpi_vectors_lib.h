void matrix_x_vector(float *matrix_part, int part_size, float *vector, float *result, int size);
void vector_sub_vector(float* left, float* right, int size);
void vector_x_scalar(float *vector, float scalar, int size);
float vector_norm(float *vector, int size);
void print_vector(float *vector, int size);
float *init_vector(int size);