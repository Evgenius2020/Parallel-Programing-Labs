clean:
	rm -rf ./out

build_sequent:
	mkdir -p ./out
	gcc src/*.c -lm -o out/a.out -D BUILD_SEQUENT

build_omp_hard:
	mkdir -p ./out
	gcc src/*.c -lm -o out/a.out -D BUILD_OMP_HARD -fopenmp

build_omp_light:
	mkdir -p ./out
	gcc src/*.c -lm -o out/a.out -D BUILD_OMP_LIGHT -fopenmp

run:
	./out/a.out 1000