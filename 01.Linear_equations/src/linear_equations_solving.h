/// Solving linear equation
///     Ax = b, where 
/// 1) A -(NxN)-matrix (2 1 1...1)
///                    (1 2 1...1)
///                    (1 1 .   1)
///                    (1 1   2 1)
///                    (1 1...1 2)
/// 2) b - N-vector (N+1 N+1 N+1 ... N+1).
/// 3) x - solution, N-vector.
///
/// There is MPI-based functions.
/// Every MPI-proccess working with
/// part of A matrix, 'part_size' lines.
///
/// In order not to receive udefined behavior,
/// ('part_size' * 'comm_size') must be equal 'N'.
///
/// After calculating, prints x vector.


/// Solving with basic algorithm, stores 
/// x and b vectors in every MPI-process.
void solve(int comm_size, int comm_rank, int part_size, int N);

/// Solving with partial algorithm, separates
/// x and b vectors between MPI-processes.
void solve_partial(int comm_size, int comm_rank, int part_size, int N);