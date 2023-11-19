#include <stdio.h>

#include "clock.h"

int main() {
    int verbose = 0;
    double freq = mhz(verbose);
    double freq_cal = mhz_full(verbose, 3);
    printf("mhz (%.2f) (%.4f)\n", freq, freq_cal);

    double overhead = get_counter_overhead();
    printf("overhead = %.2f\n", overhead);
    return 0;
}
