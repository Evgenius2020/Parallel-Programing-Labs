build:
	mpicc.mpich ./src/*.c -pthread

build_silent:
	mpicc.mpich ./src/*.c -pthread -D SILENT

build_unoptimized:
	mpicc.mpich ./src/*.c -pthread -D SILENT -D UNOPTIMIZED

run:
	mpiexec.mpich -n 6 ./a.out