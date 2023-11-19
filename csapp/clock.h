#ifndef CLOCK_H
#define CLOCK_H

// start counter
void start_counter();

// get num of cycles since counter started
double get_counter();

// measure overhead of counter
double get_counter_overhead();

// clock rate of processor
double mhz(int verbose);

// clock rate of processor - more control over accuracy
double mhz_full(int verbose, int sleeptime);

// special counters that compensate for timer interrupt overhead
void start_comp_counter();
double get_comp_counter();

#endif
