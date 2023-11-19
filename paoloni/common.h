#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define SIZE_OF_STAT 1000
#define BOUND_OF_LOOP 10000

double get_variance(uint64_t *inputs, int size);
double get_variance_d(double *inputs, int size);
double get_average(uint64_t *inputs, int size);
double get_average_d(double *inputs, int size);
uint64_t get_min(uint64_t *inputs, int size);
uint64_t get_max(uint64_t *inputs, int size);
void print_arr(uint64_t *inputs, int size);
double get_median(uint64_t *input, int size);
void run_benchmark(void (*bench_fn)(uint64_t **));

#endif
