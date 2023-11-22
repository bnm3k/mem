#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>

#include <x86intrin.h>

#include "clock.h"

static uint64_t start = 0;

// start counter
void start_counter() { start = __rdtsc(); };

// get num of cycles since counter started
double get_counter() {
    uint64_t end = __rdtsc();
    double res = (double)(end - start);
    if (res < 0) {
        fprintf(stderr, "Error: Cycle counter returning negative value: %.0f\n",
                res);
    }
    return res;
};

// measure overhead of counter
double get_counter_overhead() {
    // Do it twice to eliminate cache effects
    int i;
    double result;
    for (i = 0; i < 2; i++) {
        start_counter();
        result = get_counter();
    }
    return result;
};

// keep track of clock speed
double cpu_ghz = 0.0;

#define MAXBUF 512

static double core_mhz(int verbose) {
    static char buf[MAXBUF];
    FILE *fp = fopen("/proc/cpuinfo", "r");
    cpu_ghz = 0.0;

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
    if (verbose) {
        printf("Processor Clock Rate ~= %.4f GHz (extracted from file)\n",
               cpu_ghz);
    }
    return cpu_ghz * 1000;
}

// clock rate of processor
double mhz(int verbose) {
    double val = core_mhz(verbose);
    return val;
}

// determine clock rate by measuring cycles elapsed while sleeping for
// sleeptime seconds
double mhz_full(int verbose, int sleeptime) {
    double rate;
    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6 * sleeptime);
    if (verbose)
        printf("Processor Clock Rate ~= %.1f MHz\n", rate);
    return rate;
}

// special counters that compensate for timer interrupt overhead

static double cyc_per_tick = 0.0;

#define NEVENT 100
#define THRESHOLD 1000
#define RECORDTHRESH 3000

// attempt to see how much time is used by timer interrupt
static void callibrate(int verbose) {
    double oldt;
    struct tms t;
    clock_t oldc;
    int e = 0;
    times(&t);
    oldc = t.tms_utime;
    start_counter();
    oldt = get_counter();
    while (e < NEVENT) {
        double newt = get_counter();
        if (newt - oldt >= THRESHOLD) {
            clock_t newc;
            times(&t);
            newc = t.tms_utime;
            if (newc > oldc) {
                double cpt = (newt - oldt) / (newc - oldc);
                if ((cyc_per_tick == 0.0 || cyc_per_tick > cpt) &&
                    cpt > RECORDTHRESH)
                    cyc_per_tick = cpt;
                e++;
                oldc = newc;
            }
            oldt = newt;
        }
    }
    if (verbose)
        printf("Setting cyc_per_tick to %f\n", cyc_per_tick);
}

static clock_t start_tick = 0;

void start_comp_counter() {
    struct tms t;
    if (cyc_per_tick == 0.0)
        callibrate(1);
    times(&t);
    start_tick = t.tms_utime;
    start_counter();
}

double get_comp_counter() {
    double time = get_counter();
    double ctime;
    struct tms t;
    clock_t ticks;
    times(&t);
    ticks = t.tms_utime - start_tick;
    ctime = time - ticks * cyc_per_tick;
    /*
    printf("Measured %.0f cycles.  Ticks = %d.  Corrected %.0f cycles\n",
           time, (int) ticks, ctime);
    */
    return ctime;
}