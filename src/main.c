#include <math.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>

void assign_to_core(int core_id) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);
}

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

int main() {
    printf("cpu=%d\n", sched_getcpu());
    assign_to_core(4);
    printf("cpu=%d\n", sched_getcpu());
    size_t n  = 1000000000;
    int* nums = malloc(sizeof(int) * n);
    fill(nums, n);
    int res;

    // benchmark
    uint64_t start = read_ts_counter();
    sum(nums, n, &res);
    uint64_t end = read_ts_counter();

    printf("res=%d, cycles taken:\n%lu\n", res, (end - start));

    free(nums);
    printf("cpu=%d\n", sched_getcpu());
    return 0;
}
