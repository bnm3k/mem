#ifndef BENCH_H
#define BENCH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// function to be tested
typedef void (*mm_fn)(const size_t N,
                      double A[N][N],
                      double B[N][N],
                      double C[N][N]);

#define KEEP_SAMPLES 1

enum clock_type { CLOCK_CPU_TSC, CLOCK_OS_TIMER_NS };

typedef struct {
    double* k_min_samples;
#if KEEP_SAMPLES
    double* all_samples;
#endif
    double epsilon;
    int k;
    int sample_count;
    int max_samples;
    bool clear_cache;
    enum clock_type clock;
} bench_t;

bench_t* new_bench(int k,
                   double epsilon,
                   int max_samples,
                   bool clear_cache,
                   enum clock_type clock);

// k=3, epsilon=0.01, max_samples=500, clear_cache=false
bench_t* new_bench_default(void);

// bench_reset: set sample count to 0, clear k_min_samples & all_samples
void bench_reset(bench_t* b);

// temp for inner loop
void bench_add_sample(bench_t* b, double val);

// free heap-allocated mem used by bench_t
void free_bench(bench_t* b);

// get current minimum
double bench_get_min(bench_t* b);

// what is the relative error for the kth smallest sample
double bench_err(bench_t* b);

// have k minimum measurements converged
// what is the convergence status for k minimum measurements within epsilon
// returns 0 if not converted
// returns samplecount if converged
// returns -1 if can't reach convergence
int bench_has_converged(bench_t* b);

// compute time used by function f
double bench_run(bench_t* b,
                 mm_fn f,
                 const size_t N,
                 double A[N][N],
                 double B[N][N],
                 double C[N][N]);

void assign_to_core(int core_id);

#endif
