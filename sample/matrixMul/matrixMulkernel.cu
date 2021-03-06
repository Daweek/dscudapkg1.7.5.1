#include <stdio.h>
#define CHECK_BANK_CONFLICTS 0
#if CHECK_BANK_CONFLICTS
#define AS(i, j) cutilBankChecker(((float*)&As[0][0]), (BLOCK_SIZE * i + j))
#define BS(i, j) cutilBankChecker(((float*)&Bs[0][0]), (BLOCK_SIZE * i + j))
#else
#define AS(i, j) As[i][j]
#define BS(i, j) Bs[i][j]
#endif

//////This kernel is much faster
template <int BLOCK_SIZE> __global__ void
matrixMul( float* C, float* A, float* B, int wA, int wB)
{
    // Block index

    int bx = blockIdx.x;
    int by = blockIdx.y;

    // Thread index
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Index of the first sub-matrix of A processed by the block
    int aBegin = wA * BLOCK_SIZE * by;

    // Index of the last sub-matrix of A processed by the block
    int aEnd   = aBegin + wA - 1;

    // Step size used to iterate through the sub-matrices of A
    int aStep  = BLOCK_SIZE;

    // Index of the first sub-matrix of B processed by the block
    int bBegin = BLOCK_SIZE * bx;

    // Step size used to iterate through the sub-matrices of B
    int bStep  = BLOCK_SIZE * wB;

    // Csub is used to store the element of the block sub-matrix
    // that is computed by the thread
    float Csub = 0;

    // Loop over all the sub-matrices of A and B
    // required to compute the block sub-matrix
    for (int a = aBegin, b = bBegin;
             a <= aEnd;
             a += aStep, b += bStep) {

        // Declaration of the shared memory array As used to
        // store the sub-matrix of A
        __shared__ float As[BLOCK_SIZE][BLOCK_SIZE];

        // Declaration of the shared memory array Bs used to
        // store the sub-matrix of B
        __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];

        // Load the matrices from device memory
        // to shared memory; each thread loads
        // one element of each matrix
        AS(ty, tx) = A[a + wA * ty + tx];
        BS(ty, tx) = B[b + wB * ty + tx];

        // Synchronize to make sure the matrices are loaded
        __syncthreads();

        // Multiply the two matrices together;
        // each thread computes one element
        // of the block sub-matrix
#pragma unroll
        for (int k = 0; k < BLOCK_SIZE; ++k)
            Csub += AS(ty, k) * BS(k, tx);

        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        __syncthreads();
    }

    // Write the block sub-matrix to device memory;
    // each thread writes one element
    int c = wB * BLOCK_SIZE * by + BLOCK_SIZE * bx;
    C[c + wB * ty + tx] = Csub;
}




////This kernel is much slower...
__global__ void matrixMulDS(float *C, float *A, float *B, int wA, int wB)
{
   // Block index
   const int BLOCK_SIZE = 32;
   int bx = blockIdx.x;
   int by = blockIdx.y;

   // Thread index
   int tx = threadIdx.x;
   int ty = threadIdx.y;

   // Index of the first sub-matrix of A processed by the block
   int aBegin = wA * BLOCK_SIZE * by;

   // Index of the last sub-matrix of A processed by the block
   int aEnd   = aBegin + wA - 1;

   // Step size used to iterate through the sub-matrices of A
   int aStep  = BLOCK_SIZE;

   // Index of the first sub-matrix of B processed by the block
   int bBegin = BLOCK_SIZE * bx;

   // Step size used to iterate through the sub-matrices of B
   int bStep  = BLOCK_SIZE * wB;

   // Csub is used to store the element of the block sub-matrix
   // that is computed by the thread
   float Csub = 0;

   // Loop over all the sub-matrices of A and B
   // required to compute the block sub-matrix



   for (int a = aBegin, b = bBegin;a <= aEnd;a += aStep, b += bStep)
   {
       // Declaration of the shared memory array As used to
       // store the sub-matrix of A
 	   __shared__ float As[32][32];
       __shared__ float Bs[32][32];

	   // Load the matrices from device memory
       // to shared memory; each thread loads
       // one element of each matrix
       //As[ty][tx] = A[a + wA * ty + tx];
       //Bs[ty][tx] = B[b + wB * ty + tx];
       AS(ty, tx) = A[a + wA * ty + tx];
       BS(ty, tx) = B[b + wB * ty + tx];

       // Synchronize to make sure the matrices are loaded
       __syncthreads();

       // Multiply the two matrices together;
       // each thread computes one element
       // of the block sub-matrix
#pragma unroll

       for (int k = 0; k < BLOCK_SIZE; ++k)
       {
           //Csub += As[ty][k] * Bs[k][tx];
    	   Csub += AS(ty, k) * BS(k, tx);
       }
       // Synchronize to make sure that the preceding
       // computation is done before loading two new
       // sub-matrices of A and B in the next iteration
       __syncthreads();
   }

   // Write the block sub-matrix to device memory;
   // each thread writes one element
   int c = wB * BLOCK_SIZE * by + BLOCK_SIZE * bx;
   C[c + wB * ty + tx] = Csub;
}



