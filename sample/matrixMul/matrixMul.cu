// Martinez Noriega Edgar Josafat     14/05/2013
// Based on CUDA SDK 4.1 MatrixMul

// C includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

// CUDA Includes
#include <cuda_runtime.h>
//#include <cuda_runtime_api.h>
#include <cutil.h>
#include <cutil_inline.h>
//DSCUDA Includes
//#include "dscuda.h"
//CUBLAS include
//#include <cublas_v2.h>
//#include <cuda_runtime.h>

// Kernel Includes
#include "matrixMulkernel.cu"

#define WA (4 * block_size) // Matrix A width
#define HA (6 * block_size) // Matrix A height
#define WB (4 * block_size) // Matrix B width
#define HB WA  // Matrix B height
#define WC WB  // Matrix C width 
#define HC HA  // Matrix C height
#define MEGA 1024*1024




///To measure the time...
static void
get_cputime(double *splittime, double *laptime)
{
    struct timeval x;

    gettimeofday(&x, NULL);

    *splittime = x.tv_sec + x.tv_usec/1000000.0 - *laptime;
    *laptime = x.tv_sec + x.tv_usec/1000000.0;
}

////Random numbers generator for matrix
void randomInit(float* data, int size)
{
    for (int i = 0; i < size; ++i)
        data[i] = rand() / (float)RAND_MAX;
}

////Matrix Multiplication routine in CPU
void KernelCPU(float* C, const float* A, const float* B, unsigned int hA, unsigned int wA, unsigned int wB)
{
    for (unsigned int i = 0; i < hA; ++i)
        for (unsigned int j = 0; j < wB; ++j) {
            double sum = 0;
            for (unsigned int k = 0; k < wA; ++k) {
                double a = A[i * wA + k];
                double b = B[k * wB + j];
                sum += a * b;
            }
            C[i * wB + j] = (float)sum;
        }
}

////////////////////////////////Main//////////////////////////////////
//////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	printf("\n\n[matrixMul starting...]\n");

	char *as=NULL;
	char num ='1';

	int devID=0;
	int iSizeMultiple=5;
	cudaDeviceProp props;
	double lt=0.0, st=0.0;

	cutilSafeCall(cudaGetDeviceCount(&devID));
	printf("\n# %d device%s found.\n", devID, devID > 1 ? "s" : "");
	devID=0;
	cutilSafeCall(cudaSetDevice(devID));
	cutilSafeCall(cudaGetDeviceProperties(&props, devID));
	int block_size = 32;
	
	printf("\n|||||||||||||||||||||||||||||||||||||||||DS_CUDA mulMatrix....|||||");
	printf("\n\nDevice %d: \"%s\" with Compute %d.%d capability\n", 0, props.name, props.major, props.minor);
	
	// Optional Command-line multiplier for matrix sizes
	unsigned int uiWA, uiHA, uiWB, uiHB, uiWC, uiHC;

	//Checking for Arguments....
	////////////////////////////////////
	if ( argc < 2 ) /* argc should be 2 for correct execution */
		{
			iSizeMultiple = 5;
		}
		else
		{
			//ts = (int) argv[1];
			as = argv[1];

			for(int i=1;i<11;i++)
				{
					if(num == as[0])
					{
						iSizeMultiple = i;
						break;
					}
					num++;
				}
		}
	/////////////////////////////////////
	
	// For GPUs with fewer # of SM's, we limit the maximum size of the matrix
	if (props.multiProcessorCount <= 4) {
		uiWA = 2 * block_size * iSizeMultiple;
		uiHA = 4 * block_size * iSizeMultiple;
		uiWB = 2 * block_size * iSizeMultiple;
		uiHB = 4 * block_size * iSizeMultiple;
		uiWC = 2 * block_size * iSizeMultiple;
		uiHC = 4 * block_size * iSizeMultiple;
	} else {
		uiWA = WA * iSizeMultiple;
		uiHA = HA * iSizeMultiple;
		uiWB = WB * iSizeMultiple;
		uiHB = HB * iSizeMultiple;
		uiWC = WC * iSizeMultiple;
		uiHC = HC * iSizeMultiple;
	}


	//Variables for GPU kernell
	// setup execution parameters
	printf("\nMatrix Sizes:A(%u x %u), B(%u x %u), C(%u x %u)\n\n",uiWA, uiHA, uiWB, uiHB, uiWC, uiHC);
	dim3 threads(block_size, block_size);
	dim3 grid(uiWC / threads.x, uiHC / threads.y);
	
	printf("\nRunning kernels......\n");
	int nIter = 60;
	printf("\nNumber of iterations for each kernel %i",nIter);
	
	
	// allocate host memory for matrices A,B and C
	unsigned int size_A = uiWA * uiHA;
	unsigned int mem_size_A = sizeof(float) * size_A;
	float* h_A = (float*)malloc(mem_size_A);
	
	unsigned int size_B = uiWB * uiHB;
	unsigned int mem_size_B = sizeof(float) * size_B;
	float* h_B = (float*)malloc(mem_size_B);
	
	unsigned int size_C = uiWC * uiHC;	
	unsigned int mem_size_C = sizeof(float) * size_C;
	float* h_C      = (float*) malloc(mem_size_C);
	
	//Generate Aleatory Data
	srand(2013);
	randomInit(h_A, size_A);
	randomInit(h_B, size_B);

	
	// Allocate device (GPU) memory
	float* d_A, *d_B, *d_C;
	
	cutilSafeCall(cudaMalloc((void**) &d_A, mem_size_A));
	cutilSafeCall(cudaMalloc((void**) &d_B, mem_size_B));
	cutilSafeCall(cudaMalloc((void**) &d_C, mem_size_C));
	
	// Copy data from CPU memory to GPU memory
	cutilSafeCall(cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice) );
	cutilSafeCall(cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice) );
	printf("\nTotal amount of memory to be sent from CPU to GPU: %d Bytes",mem_size_A+mem_size_B);

	
	
	// Make warmup operation and Synchronize GPU
	matrixMulDS<<< grid, threads >>>(d_C, d_A, d_B, uiWA, uiWB);
	//matrixMul<32><<< grid, threads >>>(d_C, d_A, d_B, uiWA, uiWB);
	cudaDeviceSynchronize();
	
	
	// execute the kernel in GPU
	get_cputime(&lt,&st);
	for (int j = 0;j< nIter;j++){
		matrixMulDS<<< grid, threads >>>(d_C, d_A, d_B, uiWA, uiWB);
		//matrixMul<32><<< grid, threads >>>(d_C, d_A, d_B, uiWA, uiWB);
	}
	cudaDeviceSynchronize();
	get_cputime(&lt,&st);

	
	// Copying memory back from GPU to CPU
	cutilSafeCall(cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost) );
	

	//Calculating of Performance .... Gflops
	printf("\nTotal amount of memory to be sent from GPU to CPU: %d Bytes",mem_size_C);
	double dSeconds = lt/((double)nIter);
	double dNumOps = 2.0 * (double)uiWA * (double)uiHA * (double)uiWB;
	double gflops = 1.0e-9 * dNumOps/dSeconds;
	printf("\n>DSCUDA\t%.4f GFlop/s,\tTime:%.8f s,\tSize:%.0f Ops ",gflops,dSeconds,dNumOps);

#if 1	
	// Variables for measuring the time
	lt = 0.0;
	st = 0.0;
	nIter = 2;
	dSeconds = 0.0;
	dNumOps = 0.0;
	gflops = 0.0;
	bool correct = true;
	float* reference = (float*)malloc(mem_size_C);
	
	// execute kernel in CPU
	get_cputime(&lt,&st);
	
	for(int i=0;i<nIter;i++)
	{
		KernelCPU(reference, h_A, h_B, uiHA, uiWA, uiWB);
		
	}
	get_cputime(&lt,&st);
	
	
	
	dSeconds = lt/((double)nIter);
	dNumOps = 2.0 * (double)uiWA * (double)uiHA * (double)uiWB;
	gflops = 1.0e-9 * dNumOps/dSeconds;
	printf("\n> CPU\t\t%.4f GFlop/s\t, Time:%.5f s, Size:%.0f Ops ",gflops,dSeconds,dNumOps);

	printf("\n\nComparing GPU results with CPU calculation...");
	// Comparing Results between CPU and GPU calculations
	for (int i = 0; i < size_C; i++)
	    {
	        if (fabs(h_C[i] - reference[i]) > 1e-3)
	        {
	            printf("Error! GPUmem[%05d]=%.8f, CPUmem=%.8f error term is %.8f > 1e-3\n", i, h_C[i],reference[i],fabs(h_C[i] - reference[i]));
	            correct = false;
	        }
	    }

	printf("%s\n", correct ? "OK" : "FAIL");
	if (correct){
		printf("\nMatrix");
		for (int i=0;i<10;i++){
			printf("\nGPUmem[%05d]=%.8f, CPUmem=%.8f ---- Difference... %.8f < 1e-3", i, h_C[i],reference[i],fabs(h_C[i] - reference[i]));
		}
	}
	
	//Free memory
	free(reference);
	
#endif

	//Cleaning Memory....
	free(h_A);
	free(h_B);
	free(h_C);
	
	cutilSafeCall(cudaFree(d_A));
	cutilSafeCall(cudaFree(d_B));
	cutilSafeCall(cudaFree(d_C));
	printf("\n\nExit mulMatrix Program...\n\n");

	
    exit(0);
    return 0;
}
