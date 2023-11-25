#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <x86intrin.h>

void assign_to_core(int core_id) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);
    sched_setaffinity(1, sizeof(mask), &mask);
}

static inline uint64_t read_OS_timer_ns(void) {
    struct timespec now;
    clockid_t clock_id = CLOCK_MONOTONIC;
    clock_gettime(clock_id, &now);
    uint64_t now_ns = (now.tv_sec * 1000000000) + now.tv_nsec;
    return now_ns;
}

double get_cpu_frequency_GHz(uint64_t wait_time_milliseconds) {
    uint64_t os_start_ns = 0, os_end_ns = 0, os_elapsed_ns = 0;
    uint64_t os_wait_time_ns = wait_time_milliseconds * 1000000;

    uint64_t cpu_start = __rdtsc();          // read CPU start
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

int main() {
    int core_id            = 1;
    uint64_t wait_times_ms = 100;
    assign_to_core(core_id);

    double frequency = get_cpu_frequency_GHz(wait_times_ms);
    printf("frequency=%f\n", frequency);

    return 0;
}
