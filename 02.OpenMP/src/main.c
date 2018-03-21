#ifndef BUILD_SEQUENT
#include <omp.h>
#endif

#include <stdlib.h>
#include <stdio.h>

void solve (unsigned N);

void main(int argc, char *argv[])
{
#ifndef BUILD_SEQUENT
    omp_set_dynamic(0);
    omp_set_num_threads(4);
#endif

    if (argc < 2)
    {
        printf("Please enter N. Aborting...\n");
        return;
    }
    unsigned N = atoi(argv[1]);

#ifdef BUILD_OMP_LIGHT
#pragma omp parallel
#endif
    {
        solve(N);
    }
}