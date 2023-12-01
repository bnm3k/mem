#ifndef BENCH_H
#define BENCH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void assign_to_core(int core_id);

double get_cpu_frequency_GHz(uint64_t wait_time_milliseconds);

// funciton to be tested
typedef void (*mm_fn)(const size_t N,
                      double A[N][N],
                      double B[N][N],
                      double C[N][N]);

// compute time used by function f
double bench(mm_fn f,
             const size_t N,
             double A[N][N],
             double B[N][N],
             double C[N][N],
             bool clear_cache);

// routines used to help with analysis
double bench_full(mm_fn f,
                  const size_t N,
                  double A[N][N],
                  double B[N][N],
                  double C[N][N],
                  bool clear_cache,
                  int k,
                  double epsilon,
                  int maxsamples,
                  bool compensate);

// get current minimum
double get_min();

// convergence status for k minimum measurements within epsilon
// returns 0 if not converged, #samples if converged and -1 if can't reach
// convergence
int has_converged(int k, double epsilon, int maxsamples);

// what is error of current measurement
double err(int k);
#endif
