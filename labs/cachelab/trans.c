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
    transpose_32_32(M, N, A, B);
}


/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */


char transpose_32_32_desc[] = "32 by 32 Transpose submission";
void transpose_32_32(int M, int N, int A[N][M], int B[M][N]) {
    int block_size, i, j, block_i, block_j,
        block_diag_elem, block_diag_index;
    /*
     * sizeof(int) is 4 bytes and there are 32 bytes per block.
     * if we divide that by 4 bytes we get 8 integers per block.
     */
    block_size = 8;
    /*
     * break up the matrix into square sub-matrices as large
     * as large a block.
     */
    for (j = 0; j < M; j += block_size)
    for (i = 0; i < N; i += block_size)
    for (block_i = i; block_i < i + block_size; ++block_i) {
        for (block_j = j; block_j < j + block_size; ++block_j) {
            if (block_i != block_j)
                B[block_j][block_i] = A[block_i][block_j];
            else {
                block_diag_elem = A[block_i][block_j];
                block_diag_index = block_i;
            }
        }
        if (i == j)
            B[block_diag_index][block_diag_index] = block_diag_elem;
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

