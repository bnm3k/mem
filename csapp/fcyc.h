#ifndef FCYC_H
#define FCYC_H

#include <stdbool.h>

// funciton to be tested
typedef int (*test_funct)(int, int);

// compute time used by function f
double fcyc2(test_funct f, int param1, int param2, bool clear_cache);

// routines used to help with analysis
double fyc2_full(test_funct f,
                 int param1,
                 int param2,
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

// try other clocking methods
double fcyc2_full_tod(test_funct f, int param1, int param2, int clear_cache);

// full version that uses time of day clock
double fyc2_full_tod(test_funct f,
                     int param1,
                     int param2,
                     int clear_cache,
                     int k,
                     double epsilon,
                     int maxsamples,
                     bool compensate);
#endif
