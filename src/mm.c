#include <stdlib.h>
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
