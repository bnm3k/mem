#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE_OF_STAT 100000
#define BOUND_OF_LOOP 1000
/* #define UINT64_MAX (18446744073709551615ULL) */

void inline fill_times(uint64_t** times) {
    unsigned long flags;
    int i, j;
    uint64_t start, end;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
    volatile int variable = 0;

    asm volatile("CPUID\n\t"
                 "RDTSC\n\t"
                 "move %%edx, %0\n\t"
                 "move %%eax, %1\n\t"
                 : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                   "%rdx");

    asm volatile("CPUID\n\t"
                 "RDTSC\n\t"
                 "CPUID\n\t"
                 "RDTSC\n\t"
                 "move %%edx, %0\n\t"
                 "move %%eax, %1\n\t"
                 : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                   "%rdx");

    asm volatile("CPUID\n\t"
                 "RDTSC\n\t" ::
                     : "%rax", "%rbx", "%rcx", "%rdx");

    for (j = 0; j < BOUND_OF_LOOP; j++) {
        for (i = 0; i < SIZE_OF_STAT; i++) {
            variable = 0;
            asm volatile("CPUID\n\t"
                         "RDTSC\n\t"
                         "move %%edx, %0\n\t"
                         "move %%eax, %1\n\t"
                         : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx",
                           "%rcx", "%rdx");

            // call function to measure here

            asm volatile("CPUID\n\t"
                         "RDTSC\n\t"
                         "move %%edx, %0\n\t"
                         "move %%eax, %1\n\t"
                         : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax",
                           "%rbx", "%rcx", "%rdx");

            start = (((uint64_t)cycles_high << 32) | cycles_low);
            end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);

            if (start > end) {
                fprintf(stderr, "critical error in taking time: start > end");
                exit(1);
            } else {
                times[j][i] = end - start;
            }
        }
    }
    return;
}

uint64_t var_calc(uint64_t* inputs, int size) {
    int i;
    uint64_t acc = 0, previous = 0, temp_var = 0;
    for (i = 0; i < size; i++) {
        if (acc < previous) goto overflow;
        previous = acc;
        acc += inputs[i];
    }
    acc = acc * acc;
    if (acc < previous) goto overflow;
    previous = 0;
    for (i = 0; i < size; i++) {
        if (temp_var < previous) goto overflow;
        previous = temp_var;
        temp_var += (inputs[i] * inputs[i]);
    }
    temp_var = temp_var * size;
    if (temp_var < previous) goto overflow;
    temp_var = (temp_var - acc) / (((uint64_t)(size)) * ((uint64_t)(size)));
    return (temp_var);

overflow:
    fprintf(stderr, "critical overflow error");
    exit(1);
}

int main() {
    int i = 0, j = 0, spurious = 0, k = 0;
    uint64_t** times;
    uint64_t* variances;
    uint64_t* min_values;
    uint64_t max_dev = 0, min_time = 0, max_time = 0, prev_min = 0, tot_var = 0,
             max_dev_all = 0, var_of_vars = 0, var_of_mins = 0;

    times = malloc(BOUND_OF_LOOP * sizeof(uint64_t*));
    for (j = 0; j < BOUND_OF_LOOP; j++) {
        times[j] = malloc(SIZE_OF_STAT * sizeof(uint64_t));
    }

    variances  = malloc(BOUND_OF_LOOP * sizeof(uint64_t));
    min_values = malloc(BOUND_OF_LOOP * sizeof(uint64_t));

    fill_times(times);

    for (j = 0; j < BOUND_OF_LOOP; j++) {
        max_dev  = 0;
        min_time = 0;
        max_time = 0;

        //  get min and max
        for (i = 0; i < SIZE_OF_STAT; i++) {
            if ((min_time == 0) || (min_time > times[j][i])) {
                min_time = times[j][i];
            }
            if (max_time < times[j][i]) {
                max_time = times[j][i];
            }
        }

        max_dev       = max_time - min_time;
        min_values[j] = min_time;
        if ((prev_min != 0) && (prev_min > min_time)) spurious++;
        if (max_dev > max_dev_all) max_dev_all = max_dev;

        variances[j] = var_calc(times[j], SIZE_OF_STAT);
        tot_var += variances[j];

        printf("loop_size: %d >>> variance(cycles): %lu; max_deviation: %lu "
               "; min time: %lu\n",
               j, variances[j], max_dev, min_time);
        prev_min = min_time;
    }

    var_of_vars = var_calc(variances, BOUND_OF_LOOP);
    var_of_mins = var_calc(min_values, BOUND_OF_LOOP);
    printf("total number of spurious min values = %d\n", spurious);
    printf("total variance = %lu\n", (tot_var / BOUND_OF_LOOP));
    printf("absolute max deviation = %lu\n", max_dev_all);
    printf("variance of variances = %lu\n", var_of_vars);
    printf("variance of minimum values = %lu\n", var_of_mins);

    free(min_values);
    free(variances);
    for (j = 0; j < BOUND_OF_LOOP; j++) {
        free(times[j]);
    }
    free(times);
    return 0;
}
