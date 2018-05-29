clean:
	rm -rf ./out

build:
	mkdir -p out
	mpicc.mpich src/*.c -o out/a.out -lm

run_basic:
	mpiexec.mpich 1000 ./out/a.out

run_partial:
	mpiexec.mpich 1000 ./out/a.out -p