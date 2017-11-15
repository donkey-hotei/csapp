/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    switch(M) {
        case 32:
            transpose_32_32(M, N, A, B);
            break;
        case 64:
            transpose_64_64(M, N, A, B);
            break;
        case 61:
            transpose_61_67(M, N, A, B);
            break;
    }
}

char transpose_32_32_desc[] = "32 by 32 Transpose submission";
void transpose_32_32(int M, int N, int A[N][M], int B[M][N]) {
    int block_size, i, j, ii, jj,
        diag_elem, diag_index;
    /*
     * sizeof(int) is 4 bytes and there are 32 bytes per block.
     * if we divide that by 4 bytes we get 8 integers per block.
     */
    block_size = M / sizeof(int); // 8
    /*
     * break up the matrix into square sub-matrices of blocksize
     * squared, fitting a single row into a cacheline.
     * diag_elem and diag_index are used to avoid misses - one to
     * store the value and the other to store the position which
     * helps to avoid having to re-access those elements.
     */
    for (j = 0; j < M; j += block_size)
    for (i = 0; i < N; i += block_size)
    for (ii = i; ii < i + block_size; ++ii) {
        for (jj = j; jj < j + block_size; ++jj) {
            if (ii != jj)
                B[jj][ii] = A[ii][jj];
            else {
                diag_elem = A[ii][jj];
                diag_index = ii;
            }
        }
        // there is only one element in the diagonal each traversal
        if (i == j)
            B[diag_index][diag_index] = diag_elem;
    }
}


char transpose_64_64_desc[] = "32 by 32 Transpose submission";
void transpose_64_64(int M, int N, int A[N][M], int B[M][N]) {
    int block_size, i, j, ii, jj,
        diag_elem, diag_index;
    block_size = 4;
    for (j = 0; j < M; j += block_size)
    for (i = 0; i < N; i += block_size)
    for (ii = i; (ii < N) && ii < i + block_size; ++ii) {
        for (jj = j; (jj < M) && jj < j + block_size; ++jj) {
            if (ii != jj)
                B[jj][ii] = A[ii][jj];
            else {
                diag_elem = A[ii][jj];
                diag_index = ii;
            }
        }
        if (i == j)
            B[diag_index][diag_index] = diag_elem;
    }
}



char transpose_61_67_desc[] = "Transpose 61 x 67 matrix";
void transpose_61_67(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, ii, jj, tmp;
    for (i = 0; i < N; i += 16)
    for (j = 0; j < M; j += 4)
    for (ii = i; (ii < i + 16 && ii < N); ++ii) {
        for (jj = j; (jj < j + 4 && jj < M); ++jj) {
            if (ii - i == jj - j)
                tmp = A[ii][jj];
            else
                B[jj][ii] = A[ii][jj];
        }
        for (jj = j; (jj < j + 4 && jj < M); ++jj)
            if (ii - i == jj - j)
                B[jj][ii] = tmp;
    }
}
/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

