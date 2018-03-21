clean:
	rm -rf ./out

build_sequent:
	mkdir -p ./out
	gcc src/main.c src/solve.c src/vector_operations_sequent.c -lm -o out/a.out -D BUILD_SEQUENT

build_omp_hard:
	mkdir -p ./out
	gcc src/main.c src/solve.c src/vector_operations_omp.c -lm -fopenmp -o out/a.out

build_omp_light:
	mkdir -p ./out
	gcc src/main.c src/solve.c src/vector_operations_sequent.c -lm -fopenmp -o out/a.out -D BUILD_OMP_LIGHT