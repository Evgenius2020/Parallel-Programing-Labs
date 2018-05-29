clean:
	rm -rf ./out

build:
	mkdir -p out
	mpicc.mpich ./src/*.c -lm -o out/a.out

run:
	mpiexec.mpich -n 4 ./out/a.out