clean:
	rm -rf ./out

build:
	mkdir -p out
	mpicc.mpich ./src/*.c -pthread -o out/a.out

build_silent:
	mkdir -p out
	mpicc.mpich ./src/*.c -pthread -D SILENT -o out/a.out

build_unoptimized:
	mkdir -p out
	mpicc.mpich ./src/*.c -pthread -D SILENT -D UNOPTIMIZED -o out/a.out

run:
	mpiexec.mpich -n 6 ./out/a.out