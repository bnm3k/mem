#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bench.h"

#define MIN_N 2
#define MAX_N 4
#define INC_BY 1

#define min(x, y) (x < y ? x : y)

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
    printf("N, ");
    for (size_t cs = 0; cs < num_cases; cs++) {
        printf("%s, ", matrix_multiply[cs].name);
    }
    printf("frequency_GHz\n");
    double frequency = get_cpu_frequency_GHz(100);

    // get first comp
    double n_cycles[num_cases];
    for (size_t N = 3; N < 6; N++) {
        for (size_t cs = 0; cs < num_cases; cs++) {
            fill_with_val(0.0, MAX_N, C);
            double cycles = bench(matrix_multiply[cs].fn, N, A, B, C, false);
            n_cycles[cs]  = cycles;
        }
        // output row
        printf("%lu, ", N);
        for (size_t cs = 0; cs < num_cases; cs++) {
            printf("%f, ", n_cycles[cs]);
        }
        printf("%f\n", frequency);
    }

    return 0;
}
