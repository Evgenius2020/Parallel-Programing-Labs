clear:
	rm -rf ./out

build:
	mkdir -p out
	mpicc.mpich ./src/*.* -lm -o ./out/a.out

run:
	mpiexec.mpich -n 4 ./out/a.out 50 50 50