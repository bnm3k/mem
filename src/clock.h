#ifndef CLOCK_H
#define CLOCK_H
#include <stdint.h>

// read CPU TSC counter (cycles)
uint64_t read_CPU_tsc(void);

// read OS timer (CLOCK_MONOTONIC, nanosecond resolution)
uint64_t read_OS_timer_ns(void);

// helper function: get duration given start and end values from tsc
double get_duration(uint64_t start, uint64_t end);

// measure overhead of tsc counter
double get_tsc_counter_overhead(void);

// get nominal CPU frequency by reading from /proc/cpuinfo
double get_nomimal_CPU_frequency_GHz(void);

// calculate clock rate by measuring cycles after waiting for given milliseconds
double calc_CPU_frequency_GHz(uint64_t wait_time_milliseconds);

#endif
