# Lab 1

### Description
This program uses MPI to solve linear equation Ax = b, where 
* A - (NxN)-matrix (2 1 1...1 1) (1 2 1...1 1) (1 1 2...1 1) ... (1 1 1...1 2)
* b - N-vector (N+1 N+1 N+1 ... N+1).
* x - solution, N-vector.

### Main functions
In the "src/linear_equations_solving.c" file you can find main MPI-based functions:
##### Solve with basic algorithm (storing x and b vectors in every MPI-process).
`void solve(unsigned N);`
##### Solve with partial algorithm (separating x and b vectors between MPI-processes).
`void solve_partial(unsigned N);`

Every MPI-proccess working with part of A matrix.
After calculating, first process prints x vector.

## Instalation

`make build`
## Start
#### Run basic algoritm for N=6, using 6 mpi-processes.
`mpiexec -n 6 ./out/a.out 6`
#### Run partial algoritm for N=6, using 3 mpi-processes.
`mpiexec -n 3 ./out/a.out 6 -p`