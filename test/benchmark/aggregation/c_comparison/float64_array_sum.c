#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>

#define N 400000000UL //

int main() {
    double *data;
    if (posix_memalign((void**)&data, 64, N * sizeof(double)) != 0) return 1;
    for (size_t i = 0; i < N; i++) data[i] = 1.0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    __m256d acc0 = _mm256_setzero_pd();
    __m256d acc1 = _mm256_setzero_pd();
    __m256d acc2 = _mm256_setzero_pd();
    __m256d acc3 = _mm256_setzero_pd();

    for (size_t i = 0; i < N; i += 16) {
        acc0 = _mm256_add_pd(acc0, _mm256_load_pd(&data[i]));
        acc1 = _mm256_add_pd(acc1, _mm256_load_pd(&data[i + 4]));
        acc2 = _mm256_add_pd(acc2, _mm256_load_pd(&data[i + 8]));
        acc3 = _mm256_add_pd(acc3, _mm256_load_pd(&data[i + 12]));
    }

    acc0 = _mm256_add_pd(acc0, acc1);
    acc2 = _mm256_add_pd(acc2, acc3);
    acc0 = _mm256_add_pd(acc0, acc2);

    double res[4];
    _mm256_storeu_pd(res, acc0);
    double total = res[0] + res[1] + res[2] + res[3];

    clock_gettime(CLOCK_MONOTONIC, &end);
    double sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
    double gb = (N * sizeof(double)) / (1024.0 * 1024.0 * 1024.0);

    printf("Total sum: %f\n", total);
    printf("Read Bandwidth: %.2f GB/s\n", gb / sec);

    free(data);
    return 0;
}
