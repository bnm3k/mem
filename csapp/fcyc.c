#include "clock.h"
#include "fcyc.h"
#include <stdbool.h>
#include <stdlib.h>
//
static double* k_min_samples = NULL;
int samplecount              = 0;

#define KEEP_VALS 1
#define KEEP_SAMPLES 1

#if KEEP_SAMPLES
static double* all_samples = NULL;
#endif

// start new sampling process
static void init_sampler(int k, int maxsamples) {
    if (k_min_samples) free(k_min_samples);
    k_min_samples = calloc(k, sizeof(double));
#if KEEP_SAMPLES
    if (all_samples) free(all_samples);
    all_samples = calloc(maxsamples + k, sizeof(double));
#endif
    samplecount = 0;
}

// add new sample
void add_sample(double val, int k) {
    int pos = 0;
    if (samplecount < k) {
        // so for, only have samplecount < k (default k = 3)
        pos                = samplecount;
        k_min_samples[pos] = val;
    } else if (val < k_min_samples[k - 1]) {
        // samplecount == k
        // if new val is less than max, remove max
        pos                = k - 1;
        k_min_samples[pos] = val;
    }

#if KEEP_SAMPLES
    all_samples[samplecount] = val;
#endif

    samplecount++;

    // insertion sort
    // 0th position - smallest val
    // (k-1) position - largest val
    while (pos > 0 && k_min_samples[pos - 1] > k_min_samples[pos]) {
        double temp            = k_min_samples[pos - 1];
        k_min_samples[pos - 1] = k_min_samples[pos];
        k_min_samples[pos]     = temp;
        pos--;
    }
}

// get current minimum
double get_min() {
    return k_min_samples[0];
}

// what is the relative error for the kth smallest sample
double err(int k) {
    if (samplecount < k) return 1000.0;
    return (k_min_samples[k - 1] - k_min_samples[0]) / k_min_samples[0];
}

// have k minimum measurements converged
// what is the convergence status for k minimum measurements within epsilon
// returns 0 if not converted
// returns samplecount if converged
// returns -1 if can't reach convergence
int has_converged(int k, double epsilon, int maxsamples) {
    if ((samplecount >= k) &&
        ((1 + epsilon) * k_min_samples[0] >= k_min_samples[k - 1]))
        return samplecount;
    if ((samplecount < maxsamples)) {
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
double fcyc2(test_funct f, int param1, int param2, bool clear_cache) {
    return fyc2_full(f, param1, param2, clear_cache, 3, 0.01, 500, 0);
}

// routines used to help with analysis
double fyc2_full(test_funct f,
                 int param1,
                 int param2,
                 bool clear_cache,
                 int k,
                 double epsilon,
                 int maxsamples,
                 bool compensate) {
    void (*do_start_counter)() = start_counter;
    double (*do_get_counter)() = get_counter;
    if (compensate) {
        do_start_counter = start_comp_counter;
        do_get_counter   = get_comp_counter;
    }
    init_sampler(k, maxsamples);
    do {
        if (clear_cache) do_clear_cache();
        f(param1, param2); // warm cache

        do_start_counter();
        f(param1, param2);
        double cyc = do_get_counter();
        add_sample(cyc, k);
    } while (!has_converged(k, epsilon, maxsamples) &&
             samplecount < maxsamples);
    double result = k_min_samples[0];
#if !KEEP_VALS
    free(values);
    values = NULL;
#endif
    return result;
}
