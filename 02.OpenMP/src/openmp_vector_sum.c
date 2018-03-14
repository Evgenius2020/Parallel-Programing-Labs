#include <stdio.h>
#include <omp.h>

#define N 100

void openmp_vector_sum()
{
    double a[N], b[N], c[N];
    int i;
    omp_set_dynamic(0);      // запретить библиотеке openmp менять число потоков во время исполнения
    omp_set_num_threads(10); // установить число потоков в 10

    // инициализируем массивы
    for (i = 0; i < N; i++)
    {
        a[i] = i * 1.0;
        b[i] = i * 2.0;
    }
#pragma omp parallel
    printf("%d", omp_get_thread_num());

    // вычисляем сумму массивов
#pragma omp parallel for shared(a, b, c) private(i)
    for (i = 0; i < N; i++)
        c[i] = a[i] + b[i];

    printf(" %f\n", c[10]);
    return 0;
}