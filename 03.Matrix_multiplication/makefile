clear:
	rm -rf ./out

build:
	mkdir -p out
	mpicc ./src/*.* -lm -o ./out/a.out

run:
	mpirun -n 4 ./out/a.out