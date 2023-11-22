#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

#include <x86intrin.h>

uint64_t inline read_ts_counter() {
    uint32_t lo = 0, hi = 0;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void fill(int* nums, size_t n) {
    for (size_t i = 0; i < n; i++) {
        nums[i] = (int)i;
    }
}

void sum(int* nums, size_t n, int* res) {
    int s = 0;
    for (size_t i = 0; i < n; i++) {
        s += nums[i];
    }
    *res = s;
}

int other() {
    size_t n  = 10000;
    int* nums = malloc(sizeof(int) * n);
    fill(nums, n);
    int res;

    uint64_t t1 = __rdtsc();
    // benchmark
    uint64_t start = read_ts_counter();
    sum(nums, n, &res);
    uint64_t t2  = __rdtsc();
    uint64_t end = read_ts_counter();

    printf("res=%d, cycles taken:\n%lu\n%lu\n", res, (end - start), (t2 - t2));

    free(nums);
    return 0;
}

int main() {
    return 0;
}
