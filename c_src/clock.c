#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <x86intrin.h>

#include "clock.h"

uint64_t inline read_CPU_tsc(void) {
    uint32_t lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

uint64_t inline read_OS_timer_ns(void) {
    struct timespec now;
    clockid_t clock_id = CLOCK_MONOTONIC;
    clock_gettime(clock_id, &now);
    uint64_t now_ns = (now.tv_sec * 1000000000) + now.tv_nsec;
    return now_ns;
}

// get num of cycles since counter started
double get_duration(uint64_t start, uint64_t end) {
    double res = (double)(end - start);
    if (res < 0) {
        fprintf(stderr, "Error: Cycle counter returning negative value: %.0f\n",
                res);
    }
    return res;
};

// measure overhead of tsc counter
double get_tsc_counter_overhead(void) {
    // Do it n-times to eliminate cache effects
    double result;
    int repeat = 5;
    for (int i = 0; i < repeat; i++) {
        uint64_t start = read_CPU_tsc();
        uint64_t end   = read_CPU_tsc();
        result         = get_duration(start, end);
    }
    return result;
};

#define MAXBUF 512

double get_nomimal_CPU_frequency_GHz(void) {
    static char buf[MAXBUF];
    FILE* fp       = fopen("/proc/cpuinfo", "r");
    double cpu_ghz = 0.0;

    if (!fp) {
        fprintf(stderr, "Can't open /proc/cpuinfo to get clock information\n");
        cpu_ghz = 2.6;
        return cpu_ghz * 1000.0;
    }
    while (fgets(buf, MAXBUF, fp)) {
        if (strstr(buf, "cpu MHz")) {
            double cpu_mhz = 0.0;
            sscanf(buf, "cpu MHz\t: %lf", &cpu_mhz);
            cpu_ghz = cpu_mhz / 1000.0;
            break;
        }
    }
    fclose(fp);
    if (cpu_ghz == 0.0) {
        fprintf(stderr, "Can't open /proc/cpuinfo to get clock information\n");
        cpu_ghz = 2.6;
        return cpu_ghz * 1000.0;
    }
    return cpu_ghz;
}

// calculate clock rate by measuring cycles after waiting for given milliseconds
double calc_CPU_frequency_GHz(uint64_t wait_time_milliseconds) {
    uint64_t os_start_ns = 0, os_end_ns = 0, os_elapsed_ns = 0;
    uint64_t os_wait_time_ns = wait_time_milliseconds * 1000000;

    uint64_t cpu_start = read_CPU_tsc();     // read CPU start
    os_start_ns        = read_OS_timer_ns(); // read start OS time

    // wait until wait_time duration is over
    while (os_elapsed_ns < os_wait_time_ns) {
        os_end_ns     = read_OS_timer_ns();
        os_elapsed_ns = os_end_ns - os_start_ns;
    }

    uint64_t cpu_end    = __rdtsc();
    uint64_t cpu_cycles = cpu_end - cpu_start;

    return (double)cpu_cycles / (double)os_elapsed_ns;
}
