#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include "bench.h"
#include "clock.h"

// create new sampling process
bench_t* new_bench(int k, double epsilon, int max_samples, bool clear_cache) {
    bench_t* b       = calloc(sizeof(bench_t), 1);
    b->k_min_samples = calloc(k, sizeof(double));
#if KEEP_SAMPLES
    b->all_samples = calloc(max_samples + k, sizeof(double));
#endif
    b->epsilon      = epsilon;
    b->k            = k;
    b->sample_count = 0;
    b->max_samples  = max_samples;
    b->clear_cache  = clear_cache;

    return b;
}

bench_t* new_bench_default(void) {
    return new_bench(3, 0.01, 500, false);
}

void bench_reset(bench_t* b) {
    b->sample_count = 0;
    // set k_min_samples to zero
    memset(b->k_min_samples, 0, b->k * sizeof(double));
    // set all samples to zero
#if KEEP_SAMPLES
    memset(b->all_samples, 0, (b->k + b->max_samples) * sizeof(double));
#endif
}
void free_bench(bench_t* b) {
#if KEEP_SAMPLES
    free(b->all_samples);
#endif
    free(b->k_min_samples);
    free(b);
}

// add new sample
static void bench_add_sample(bench_t* b, double val) {
    int pos = 0;
    if (b->sample_count < b->k) {
        // so for, only have samplecount < k (default k = 3)
        pos                   = b->sample_count;
        b->k_min_samples[pos] = val;
    } else if (val < b->k_min_samples[b->k - 1]) {
        // samplecount == k
        // if new val is less than max, remove max
        pos                   = b->k - 1;
        b->k_min_samples[pos] = val;
    }

#if KEEP_SAMPLES
    b->all_samples[b->sample_count] = val;
#endif

    b->sample_count++;

    // insertion sort
    // 0th position - smallest val
    // (k-1) position - largest val
    while (pos > 0 && b->k_min_samples[pos - 1] > b->k_min_samples[pos]) {
        double temp               = b->k_min_samples[pos - 1];
        b->k_min_samples[pos - 1] = b->k_min_samples[pos];
        b->k_min_samples[pos]     = temp;
        pos--;
    }
}

// get current minimum
double bench_get_min(bench_t* b) {
    return b->k_min_samples[0];
}

// what is the relative error for the kth smallest sample
double bench_err(bench_t* b) {
    int k = b->k;
    if (b->sample_count < k) return 1000.0;
    return (b->k_min_samples[-1] - b->k_min_samples[0]) / b->k_min_samples[0];
}

// have k minimum measurements converged
// what is the convergence status for k minimum measurements within epsilon
// returns 0 if not converted
// returns samplecount if converged
// returns -1 if can't reach convergence
int bench_has_converged(bench_t* b) {
    if ((b->sample_count >= b->k) &&
        ((1 + b->epsilon) * b->k_min_samples[0] >= b->k_min_samples[b->k - 1]))
        return b->sample_count;
    if (b->sample_count < b->max_samples) {
        return 0; // not converged yet
    } else {
        return -1; // can't reach convergence
    }
}

#define ASIZE 383216 // l2 cache size (1.5 MB)
#define STRIDE 16    // cache block size 64 bytes
static int stuff[ASIZE];
static int sink;
static void do_clear_cache() {
    int x = sink;
    for (int i = 0; i < ASIZE; i += STRIDE)
        x += stuff[i];
    sink = x;
}

// compute time used by function f
double bench_run(bench_t* b,
                 mm_fn f,
                 const size_t N,
                 double A[N][N],
                 double B[N][N],
                 double C[N][N]) {
    do {
        if (b->clear_cache) do_clear_cache();
        f(N, A, B, C); // warm cache

        uint64_t start = read_CPU_tsc();
        f(N, A, B, C);
        uint64_t end  = read_CPU_tsc();
        double cycles = get_duration(start, end);
        bench_add_sample(b, cycles);
    } while (bench_has_converged(b) == 0 && b->sample_count < b->max_samples);
    double result = bench_get_min(b);
    return result;
}

void assign_to_core(int core_id) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);
    sched_setaffinity(1, sizeof(mask), &mask);
}
