#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_OF_STAT 10000
#define BOUND_OF_LOOP 10000
/* #define UINT64_MAX (18446744073709551615ULL) */

void run_benchmark(uint64_t** times) {
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

    // warm up the instruction cache to avoid spurious measurements due to
    // cache effects in the first iterations of the following loop
    asm volatile("CPUID\n\t"
                 "RDTSC\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                   "%rdx");

    asm volatile("RDTSCP\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 "CPUID\n\t"
                 : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx",
                   "%rcx", "%rdx");

    asm volatile("CPUID\n\t"
                 "RDTSC\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                   "%rdx");

    asm volatile("RDTSCP\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 "CPUID\n\t"
                 : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx",
                   "%rcx", "%rdx");

    // why two nested loops:
    // - when there is no function to be measured:
    //      - inner loop: calculate statistic values such as min, max, deviation
    //        from min, variance
    //      - outher loop: evaluate the ergodicity of the method taking the
    //        measures
    for (int i = 0; i < BOUND_OF_LOOP; i++) {
        for (int j = 0; j < SIZE_OF_STAT; j++) {
            asm volatile("CPUID\n\t"
                         "RDTSC\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax, %1\n\t"
                         : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx",
                           "%rcx", "%rdx");

            // call function to measure here

            asm volatile("RDTSCP\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax, %1\n\t"
                         "CPUID\n\t"
                         : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax",
                           "%rbx", "%rcx", "%rdx");

            uint64_t start = (((uint64_t)cycles_high << 32) | cycles_low);
            uint64_t end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);

            if (start > end) {
                fprintf(stderr, "critical error in taking time: start > end");
                exit(1);
            } else {
                times[i][j] = end - start;
            }
        }
    }
    return;
}

double get_variance(uint64_t* inputs, int size) {
    double sum   = 0;
    double count = (double)size;
    for (int i = 0; i < size; ++i) {
        sum += (double)inputs[i];
    }
    double mean              = sum / count;
    double sum_squared_diffs = 0;
    for (int i = 0; i < size; ++i) {
        double diff         = (mean - (double)inputs[i]);
        double squared_diff = diff * diff;
        sum_squared_diffs += squared_diff;
    }
    double variance = sum_squared_diffs / count;
    return variance;
}

double get_variance_d(double* inputs, int size) {
    double sum   = 0;
    double count = (double)size;
    for (int i = 0; i < size; ++i) {
        sum += inputs[i];
    }
    double mean              = sum / count;
    double sum_squared_diffs = 0;
    for (int i = 0; i < size; ++i) {
        double diff         = (mean - inputs[i]);
        double squared_diff = diff * diff;
        sum_squared_diffs += squared_diff;
    }
    double variance = sum_squared_diffs / count;
    return variance;
}

double get_average(uint64_t* inputs, int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += (double)inputs[i];
    }
    return sum / (double)size;
}

double get_average_d(double* inputs, int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += inputs[i];
    }
    return sum / (double)size;
}

uint64_t get_min(uint64_t* inputs, int size) {
    uint64_t min = UINT64_MAX;
    for (int i = 0; i < size; i++) {
        if (inputs[i] < min) min = inputs[i];
    }
    return min;
}

uint64_t get_max(uint64_t* inputs, int size) {
    uint64_t max = 0;
    for (int i = 0; i < size; i++) {
        if (inputs[i] > max) max = inputs[i];
    }
    return max;
}
void print_arr(uint64_t* inputs, int size) {
    printf("[ ");
    for (int i = 0; i < size; i++) {
        printf("%lu ", inputs[i]);
    }
    printf("]\n");
}

int cmp_u64(const void* ap, const void* bp) {
    uint64_t a = *(uint64_t*)ap;
    uint64_t b = *(uint64_t*)bp;
    return a - b;
}

double get_median(uint64_t* input, int size) {
    size_t num_bytes = sizeof(uint64_t) * size;
    uint64_t* copy   = malloc(num_bytes);
    copy             = memcpy(copy, input, num_bytes);
    qsort(copy, size, sizeof(uint64_t), cmp_u64);
    double median = 0;
    if (size % 2 == 0) {
        median = (double)(copy[size / 2] + copy[(size / 2) - 1]) / 2;
    } else {
        median = (double)(copy[size / 2]);
    }
    free(copy);
    return median;
}

int main() {
    // stores all the time measurements (clock cycles)
    // length: BOUND_OF_LOOP * SIZE_OF_STAT
    uint64_t** times = malloc(BOUND_OF_LOOP * sizeof(uint64_t*));
    for (int i = 0; i < BOUND_OF_LOOP; i++) {
        times[i] = malloc(SIZE_OF_STAT * sizeof(uint64_t));
    }
    // store variances
    double* variances  = malloc(BOUND_OF_LOOP * sizeof(uint64_t));
    bool* low_variance = malloc(BOUND_OF_LOOP * sizeof(bool));

    // store min values
    uint64_t* min_values = malloc(BOUND_OF_LOOP * sizeof(uint64_t));

    // store max deviations
    uint64_t* max_deviations = malloc(BOUND_OF_LOOP * sizeof(uint64_t));

    // carry out benchmark
    run_benchmark(times);

    int filtered_in_count = 0;
    for (int i = 0; i < BOUND_OF_LOOP; i++) {

        //  get min and max
        uint64_t min_time = get_min(times[i], SIZE_OF_STAT);
        uint64_t max_time = get_max(times[i], SIZE_OF_STAT);
        min_values[i]     = min_time;

        // calculate maximum deviation from the minimum for this ensemble
        max_deviations[i] = max_time - min_time;

        // variance
        double variance = get_variance(times[i], SIZE_OF_STAT);
        variances[i]    = variance;

        // mark if low variance
        low_variance[i] = false;
        if (variance <= 2) {
            low_variance[i] = true;
            filtered_in_count += 1;
        }
    }

    // calculate median ignore from ensembles with high variance
    uint64_t* filtered_times =
        malloc(sizeof(uint64_t) * BOUND_OF_LOOP * SIZE_OF_STAT);
    int temp_count   = 0;
    char* temp       = (char*)filtered_times;
    size_t num_bytes = sizeof(uint64_t) * SIZE_OF_STAT;
    for (int i = 0; i < BOUND_OF_LOOP; i++) {
        if (low_variance[i] == false) continue;
        temp_count += SIZE_OF_STAT;
        memcpy(temp, times[i], num_bytes);
        temp += num_bytes;
    }
    double filtered_median_time  = get_median(filtered_times, temp_count);
    double filtered_average_time = get_average(filtered_times, temp_count);
    free(filtered_times);

    // calculate variance of variances and variance of minimums so as to
    // evaluate the ergodicity of the measurement process. If the process is
    // ergodic, the variance of variances tends to zero and (in this case),
    // also the variance of the minimum value

    // done
    printf("absolute max deviation = %lu\n",
           get_max(max_deviations, BOUND_OF_LOOP));

    printf("variance (variance=%.4f) (average=%.4f)\n",
           get_variance_d(variances, BOUND_OF_LOOP),
           get_average_d(variances, BOUND_OF_LOOP));

    printf("min time (variance=%.4f) (median=%.4f)\n",
           get_variance(min_values, BOUND_OF_LOOP),
           get_median(min_values, BOUND_OF_LOOP));

    printf("time taken (kept=%.1f%%) (median=%.4f) (average=%.4f)\n",
           ((double)filtered_in_count / BOUND_OF_LOOP) * 100,
           filtered_median_time, filtered_average_time);

    free(min_values);
    free(max_deviations);
    free(variances);
    for (int i = 0; i < BOUND_OF_LOOP; i++) {
        free(times[i]);
    }
    free(times);
    return 0;
}
