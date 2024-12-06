/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Your transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * The transpose function is evaluated by counting the number of misses
 * on a 512B direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */

void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    /*
     * Please define your local variables here. 
     * You are allowed to define at most 12 local variables.
    */
    int a1, a2, a3, a4, a5, a6, a7, a8;
    
    if(M == 16) { //16x16 matrix
        for (int i = 0; i < 8; i++) {
            for (int j = 8; j < 16; j++) {
                B[j][i] = A[i][j];
            }
        }
        for (int i = 8; i < 16; i++) {
            for (int j = 0; j < 8; j++) {
                B[j][i] = A[i][j];
            }
        }
        for (int i = 0; i < 8; i++) {
            a1 = A[i][0];
            a2 = A[i][1];
            a3 = A[i][2];
            a4 = A[i][3];
            a5 = A[i][4];
            a6 = A[i][5];
            a7 = A[i][6];
            a8 = A[i][7];
            B[0][i] = a1;
            B[1][i] = a2;
            B[2][i] = a3;
            B[3][i] = a4;
            B[4][i] = a5;
            B[5][i] = a6;
            B[6][i] = a7;
            B[7][i] = a8;
        }
        for (int i = 8; i < 16; i++) {
            a1 = A[i][8];
            a2 = A[i][9];
            a3 = A[i][10];
            a4 = A[i][11];
            a5 = A[i][12];
            a6 = A[i][13];
            a7 = A[i][14];
            a8 = A[i][15];
            B[8][i] = a1;
            B[9][i] = a2;
            B[10][i] = a3;
            B[11][i] = a4;
            B[12][i] = a5;
            B[13][i] = a6;
            B[14][i] = a7;
            B[15][i] = a8;
        }
    }
    else if(M == 32) { //32x32 matrix
        for (int i = 0; i < 32; i += 8) {
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 8; k++)
                    B[i + j][(i + k + 8) % 32] = A[i + j][i + k];
            for (int j = 4; j < 8; j++)
                for (int k = 0; k < 8; k++)
                    B[i + j][(i + k + 16) % 32] = A[i + j][i + k];
            for (int k = 0; k < 8; k++)
                for (int j = 0; j < 4; j++)
                    B[i + k][i + j] = B[i + j][(i + k + 8) % 32];
            for (int k = 0; k < 8; k++)
                for (int j = 4; j < 8; j++)
                    B[i + k][i + j] = B[i + j][(i + k + 16) % 32];
        }
        for (int i = 0; i < 32; i += 8) {
            for (int j = 0; j < 32; j += 8) {
                if (i != j) {
                    for (int l = 0; l < 4; l++)
                        for (int k = 0; k < 4; k++)
                            B[j + k][i + l] = A[i + l][j + k];
                    for (int l = 0; l < 4; l++)
                        for (int k = 4; k < 8; k++)
                            B[j + k - 4][i + l + 4] = A[i + l][j + k];
                    for (int k = 0; k < 4; k++) {
                        a1 = B[j + k][i + 4];
                        a2 = B[j + k][i + 5];
                        a3 = B[j + k][i + 6];
                        a4 = B[j + k][i + 7];
                        B[j + k][i + 4] = A[i + 4][j + k];
                        B[j + k][i + 5] = A[i + 5][j + k];
                        B[j + k][i + 6] = A[i + 6][j + k];
                        B[j + k][i + 7] = A[i + 7][j + k];
                        B[j + k + 4][i] = a1;
                        B[j + k + 4][i + 1] = a2;
                        B[j + k + 4][i + 2] = a3;
                        B[j + k + 4][i + 3] = a4;
                    }

                    for (int l = 4; l < 8; l++)
                        for (int k = 4; k < 8; k++)
                            B[j + k][i + l] = A[i + l][j + k];
                }
            }
        }
    }
    
}




