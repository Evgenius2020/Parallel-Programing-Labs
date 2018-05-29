clean:
	rm -rf ./out

build:
	mkdir -p out
	mpicc ./src/*.c -lm -o out/a.out

run:
	mpiexec -n 4 ./out/a.out