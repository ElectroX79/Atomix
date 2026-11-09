#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <immintrin.h>

#define N 400000000UL

int main(void) {
    int32_t *data;
    if (posix_memalign((void**)&data, 64, N * sizeof(int32_t)) != 0) return 1;
    for (size_t i = 0; i < N; i++) data[i] = 1;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    __m256i acc0 = _mm256_setzero_si256();
    __m256i acc1 = _mm256_setzero_si256();
    __m256i acc2 = _mm256_setzero_si256();
    __m256i acc3 = _mm256_setzero_si256();


    for (size_t i = 0; i < N; i += 32) {
        acc0 = _mm256_add_epi32(acc0, _mm256_load_si256((const __m256i*)&data[i]));
        acc1 = _mm256_add_epi32(acc1, _mm256_load_si256((const __m256i*)&data[i + 8]));
        acc2 = _mm256_add_epi32(acc2, _mm256_load_si256((const __m256i*)&data[i + 16]));
        acc3 = _mm256_add_epi32(acc3, _mm256_load_si256((const __m256i*)&data[i + 24]));
    }

    acc0 = _mm256_add_epi32(acc0, acc1);
    acc2 = _mm256_add_epi32(acc2, acc3);
    acc0 = _mm256_add_epi32(acc0, acc2);

    int32_t res[8];
    _mm256_storeu_si256((__m256i*)res, acc0);

    int64_t total = 0;
    for (int i = 0; i < 8; i++) {
        total += res[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
    double gb = (N * sizeof(int32_t)) / (1024.0 * 1024.0 * 1024.0);

    printf("Total sum: %lld\n", (long long)total);
    printf("Read Bandwidth: %.2f GB/s\n", gb / sec);

    free(data);
    return 0;
}
