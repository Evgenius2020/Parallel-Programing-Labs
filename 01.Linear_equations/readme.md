# Lab 1

### Description
This program uses MPI to solve linear equation Ax = b, where 
* A -- (NxN)-matrix (2 1 1...1)
                   (1 2 1...1)
                   (1 1 .   1)
                   (1 1   2 1)
                   (1 1...1 2)
* b -- N-vector (N+1 N+1 N+1 ... N+1).
* x -- solution, N-vector.

### Main functions
In the "src/linear_equations_solving.c" file you can find functions
* `void solve(int comm_size, int comm_rank, int part_size, int N);`
Solving with basic algorithm, stores x and b vectors in every MPI-process.
* `void solve_partial(int comm_size, int comm_rank, int part_size, int N);`
Solving with partial algorithm, separates x and b vectors between MPI-processes.

There is MPI-based functions. Every MPI-proccess working with part of A matrix, 'part_size' lines.
After calculating, prints x vector.