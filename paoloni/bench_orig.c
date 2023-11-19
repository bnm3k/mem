#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

static void bench_fn_orig(uint64_t** times) {
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

    // warm up the instruction cache to avoid spurious measurements due to
    // cache effects in the first iterations of the following loop
    asm volatile("CPUID\n\t"
                 "RDTSC\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                   "%rdx");

    asm volatile("CPUID\n\t"
                 "RDTSC\n\t"
                 "CPUID\n\t"
                 "RDTSC\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                   "%rdx");

    asm volatile("CPUID\n\t"
                 "RDTSC\n\t" ::
                     : "%rax", "%rbx", "%rcx", "%rdx");

    // why two nested loops:
    // - when there is no function to be measured:
    //      - inner loop: calculate statistic values such as min, max, deviation
    //        from min, variance
    //      - outher loop: evaluate the ergodicity of the method taking the
    //        measures
    for (int i = 0; i < BOUND_OF_LOOP; i++) {
        for (int j = 0; j < SIZE_OF_STAT; j++) {
            asm volatile("CPUID\n\t"
                         "RDTSC\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax, %1\n\t"
                         : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx",
                           "%rcx", "%rdx");

            // call function to measure here

            asm volatile("CPUID\n\t"
                         "RDTSC\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax, %1\n\t"
                         : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax",
                           "%rbx", "%rcx", "%rdx");

            uint64_t start = (((uint64_t)cycles_high << 32) | cycles_low);
            uint64_t end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);

            if (start > end) {
                fprintf(stderr, "critical error in taking time: start > end");
                exit(1);
            } else {
                times[i][j] = end - start;
            }
        }
    }
    return;
}

int main() {
    run_benchmark(bench_fn_orig);
    return 0;
}
