#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bench.h"
#include "clock.h"

#define MIN_N 50
/* #define MAX_N 1000 */
#define MAX_N 500
#define INC_N 50

static double A[MAX_N][MAX_N];
static double B[MAX_N][MAX_N];
static double C[MAX_N][MAX_N];

// inner-loop: column-wise access of A & C
void multiply_jki(const size_t N,
                  double A[N][N],
                  double B[N][N],
                  double C[N][N]) {
    int i, j, k;
    for (j = 0; j < N; j++) {
        for (k = 0; k < N; k++) {
            double r = B[k][j];
            for (i = 0; i < N; i++) {
                C[i][j] += A[i][k] * r;
            }
        }
    }
}

// inner-loop: column-wise access of A & C
void multiply_kji(const size_t N,
                  double A[N][N],
                  double B[N][N],
                  double C[N][N]) {
    int i, j, k;
    for (k = 0; k < N; k++) {
        for (j = 0; j < N; j++) {
            double r = B[k][j];
            for (i = 0; i < N; i++) {
                C[i][j] += A[i][k] * r;
            }
        }
    }
}

// inner-loop: row-wise access of A, column-wise access of B
void multiply_jik(const size_t N,
                  double A[N][N],
                  double B[N][N],
                  double C[N][N]) {
    int i, j, k;
    for (j = 0; j < N; j++) {
        for (i = 0; i < N; i++) {
            double sum = 0.0;
            for (k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// inner-loop: row-wise access of A, column-wise access of B
void multiply_ijk(const size_t N,
                  double A[N][N],
                  double B[N][N],
                  double C[N][N]) {
    int i, j, k;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            double sum = 0.0;
            for (k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// inner-loop: row-wise access of B & C
void multiply_kij(const size_t N,
                  double A[N][N],
                  double B[N][N],
                  double C[N][N]) {
    int i, j, k;
    for (k = 0; k < N; k++) {
        for (i = 0; i < N; i++) {
            double r = A[i][k];
            for (j = 0; j < N; j++) {
                C[i][j] += r * B[k][j];
            }
        }
    }
}

// inner-loop: row-wise access of B & C
void multiply_ikj(const size_t N,
                  double A[N][N],
                  double B[N][N],
                  double C[N][N]) {
    int i, j, k;
    for (i = 0; i < N; i++) {
        for (k = 0; k < N; k++) {
            double r = A[i][k];
            for (j = 0; j < N; j++) {
                C[i][j] += r * B[k][j];
            }
        }
    }
}

void fill_with_val(double val, const size_t N, double M[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            M[i][j] = val;
        }
    }
}

void print_matrix(const size_t N, const double M[N][N]) {
    for (int i = 0; i < N; i++) {
        if (i == 0)
            printf("[");
        else
            printf(" ");
        for (int j = 0; j < N; j++) {
            printf(" %1.1f", M[i][j]);
        }
        if (i == N - 1) printf(" ]");
        printf("\n");
    }
}

// generate a random floating point number from min to max
double rand_range(double min, double max) {
    double range = (max - min);
    double div   = RAND_MAX / range;
    return min + (rand() / div);
}

void fill_rand(const size_t N, double M[N][N]) {
    int v = 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            M[i][j] = rand_range(1, 10);
            ++v;
        }
    }
}

typedef struct {
    char* name;
    void (*fn)(const size_t N, double A[N][N], double B[N][N], double C[N][N]);
} matrix_multiply_t;

int main(void) {
    assign_to_core(4);
    srand(42);

    matrix_multiply_t matrix_multiply[] = {
        {
            .name = "jki",
            .fn   = multiply_jki,
        },
        {
            .name = "kji",
            .fn   = multiply_kji,
        },
        {
            .name = "jik",
            .fn   = multiply_jik,
        },
        {
            .name = "ijk",
            .fn   = multiply_ijk,
        },
        {
            .name = "kij",
            .fn   = multiply_kij,
        },
        {
            .name = "ikj",
            .fn   = multiply_ikj,
        },
    };

    fill_rand(MAX_N, A);
    fill_rand(MAX_N, B);

    size_t num_cases = sizeof(matrix_multiply) / sizeof(matrix_multiply[0]);

    // output header
    printf("N, fn, cycles, frequency_GHz, num_samples, convergence, k, "
           "epsilon, bench_type, unit\n");
    double frequency = calc_CPU_frequency_GHz(100);
    char* bench_type = "total";
    char* unit       = "cycles";

    bench_t* b     = new_bench_default();
    b->max_samples = 3;
    // clear cache after every run
    b->clear_cache = true;
    for (size_t N = MIN_N; N <= MAX_N; N += INC_N) {
        for (size_t cs = 0; cs < num_cases; cs++) {
            fill_with_val(0.0, MAX_N, C);
            double ov = get_tsc_counter_overhead();
            double cycles =
                bench_run(b, matrix_multiply[cs].fn, N, A, B, C) - ov;
            int convergence = bench_has_converged(b);
            // output row
            printf("%lu, %s, %f, %f, %d, %d, %d, %f, %s, %s\n", N,
                   matrix_multiply[cs].name, cycles, frequency, b->max_samples,
                   convergence, b->k, b->epsilon, bench_type, unit);
            fflush(stdout);
        }
        bench_reset(b);
    }
    free_bench(b);

    return 0;
}
