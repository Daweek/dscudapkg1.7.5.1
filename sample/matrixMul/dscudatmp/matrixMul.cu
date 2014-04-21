static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda4.1/cuda/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2012-01-12\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_0000053e_00000000-9_matrixMul.cpp3.i (/tmp/ccBI#.rWsLED)\n"
    "	//-----------------------------------------------------------\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Options:\n"
    "	//-----------------------------------------------------------\n"
    "	//  Target:ptx, ISA:sm_10, Endian:little, Pointer Size:64\n"
    "	//  -O3	(Optimization level)\n"
    "	//  -g0	(Debug level)\n"
    "	//  -m2	(Report advisories)\n"
    "	//-----------------------------------------------------------\n"
    "\n"
    "	.file	1	\"<command-line>\"\n"
    "	.file	2	\"/tmp/tmpxft_0000053e_00000000-8_matrixMul.cudafe2.gpu\"\n"
    "	.file	3	\"/usr/lib/gcc/x86_64-redhat-linux/4.5.1/include/stddef.h\"\n"
    "	.file	4	\"/usr/local/cuda/include/crt/device_runtime.h\"\n"
    "	.file	5	\"/usr/local/cuda/include/host_defines.h\"\n"
    "	.file	6	\"/usr/local/cuda/include/builtin_types.h\"\n"
    "	.file	7	\"/usr/local/cuda/include/device_types.h\"\n"
    "	.file	8	\"/usr/local/cuda/include/driver_types.h\"\n"
    "	.file	9	\"/usr/local/cuda/include/surface_types.h\"\n"
    "	.file	10	\"/usr/local/cuda/include/texture_types.h\"\n"
    "	.file	11	\"/usr/local/cuda/include/vector_types.h\"\n"
    "	.file	12	\"/usr/local/cuda/include/device_launch_parameters.h\"\n"
    "	.file	13	\"/usr/local/cuda/include/crt/storage_class.h\"\n"
    "	.file	14	\"matrixMulkernel.cu\"\n"
    "	.file	15	\"/usr/local/cuda/include/common_functions.h\"\n"
    "	.file	16	\"/usr/local/cuda/include/math_functions.h\"\n"
    "	.file	17	\"/usr/local/cuda/include/math_constants.h\"\n"
    "	.file	18	\"/usr/local/cuda/include/device_functions.h\"\n"
    "	.file	19	\"/usr/local/cuda/include/sm_11_atomic_functions.h\"\n"
    "	.file	20	\"/usr/local/cuda/include/sm_12_atomic_functions.h\"\n"
    "	.file	21	\"/usr/local/cuda/include/sm_13_double_functions.h\"\n"
    "	.file	22	\"/usr/local/cuda/include/sm_20_atomic_functions.h\"\n"
    "	.file	23	\"/usr/local/cuda/include/sm_20_intrinsics.h\"\n"
    "	.file	24	\"/usr/local/cuda/include/surface_functions.h\"\n"
    "	.file	25	\"/usr/local/cuda/include/texture_fetch_functions.h\"\n"
    "	.file	26	\"/usr/local/cuda/include/math_functions_dbl_ptx1.h\"\n"
    "\n"
    "\n"
    "	.entry _Z11matrixMulDSPfS_S_ii (\n"
    "		.param .u64 __cudaparm__Z11matrixMulDSPfS_S_ii_C,\n"
    "		.param .u64 __cudaparm__Z11matrixMulDSPfS_S_ii_A,\n"
    "		.param .u64 __cudaparm__Z11matrixMulDSPfS_S_ii_B,\n"
    "		.param .s32 __cudaparm__Z11matrixMulDSPfS_S_ii_wA,\n"
    "		.param .s32 __cudaparm__Z11matrixMulDSPfS_S_ii_wB)\n"
    "	{\n"
    "	.reg .u32 %r<34>;\n"
    "	.reg .u64 %rd<29>;\n"
    "	.reg .f32 %f<100>;\n"
    "	.reg .pred %p<4>;\n"
    "	.shared .align 4 .b8 __cuda___cuda_local_var_37736_38_non_const_Bs32[4096];\n"
    "	.shared .align 4 .b8 __cuda___cuda_local_var_37735_36_non_const_As4128[4096];\n"
    "	.loc	14	89	0\n"
    "$LDWbegin__Z11matrixMulDSPfS_S_ii:\n"
    "	.loc	14	124	0\n"
    "	cvt.s32.u16 	%r1, %ctaid.x;\n"
    "	mul24.lo.s32 	%r2, %r1, 32;\n"
    "	cvt.s32.u16 	%r3, %ctaid.y;\n"
    "	ld.param.s32 	%r4, [__cudaparm__Z11matrixMulDSPfS_S_ii_wA];\n"
    "	mul.lo.s32 	%r5, %r3, %r4;\n"
    "	mul.lo.s32 	%r6, %r5, 32;\n"
    "	add.s32 	%r7, %r6, %r4;\n"
    "	sub.s32 	%r8, %r7, 1;\n"
    "	cvt.s32.u16 	%r9, %tid.x;\n"
    "	cvt.s32.u16 	%r10, %tid.y;\n"
    "	ld.param.s32 	%r11, [__cudaparm__Z11matrixMulDSPfS_S_ii_wB];\n"
    "	setp.lt.s32 	%p1, %r8, %r6;\n"
    "	@%p1 bra 	$Lt_0_3330;\n"
    "	mov.u64 	%rd1, __cuda___cuda_local_var_37736_38_non_const_Bs32;\n"
    "	mov.u64 	%rd2, __cuda___cuda_local_var_37735_36_non_const_As4128;\n"
    "	ld.param.s32 	%r4, [__cudaparm__Z11matrixMulDSPfS_S_ii_wA];\n"
    "	add.s32 	%r12, %r4, 31;\n"
    "	shr.s32 	%r13, %r12, 31;\n"
    "	mov.s32 	%r14, 31;\n"
    "	and.b32 	%r15, %r13, %r14;\n"
    "	add.s32 	%r16, %r15, %r12;\n"
    "	shr.s32 	%r17, %r16, 5;\n"
    "	ld.param.s32 	%r11, [__cudaparm__Z11matrixMulDSPfS_S_ii_wB];\n"
    "	mul.lo.s32 	%r18, %r10, %r11;\n"
    "	mul.lo.s32 	%r19, %r10, %r4;\n"
    "	cvt.s64.s32 	%rd3, %r9;\n"
    "	cvt.s64.s32 	%rd4, %r10;\n"
    "	add.s32 	%r20, %r19, %r6;\n"
    "	add.s32 	%r21, %r9, %r20;\n"
    "	mul.wide.s32 	%rd5, %r9, 4;\n"
    "	add.u64 	%rd6, %rd1, %rd5;\n"
    "	mul.wide.s32 	%rd7, %r10, 128;\n"
    "	add.u64 	%rd8, %rd2, %rd7;\n"
    "	mul.wide.s32 	%rd9, %r10, 32;\n"
    "	add.u64 	%rd10, %rd3, %rd9;\n"
    "	mul.lo.u64 	%rd11, %rd10, 4;\n"
    "	add.s32 	%r22, %r19, %r8;\n"
    "	mul.lo.s32 	%r23, %r11, 32;\n"
    "	cvt.s64.s32 	%rd12, %r23;\n"
    "	mul.wide.s32 	%rd13, %r23, 4;\n"
    "	add.u64 	%rd14, %rd11, %rd2;\n"
    "	add.u64 	%rd15, %rd11, %rd1;\n"
    "	add.s32 	%r24, %r22, %r9;\n"
    "	ld.param.u64 	%rd16, [__cudaparm__Z11matrixMulDSPfS_S_ii_B];\n"
    "	add.s32 	%r25, %r18, %r2;\n"
    "	add.s32 	%r26, %r9, %r25;\n"
    "	cvt.s64.s32 	%rd17, %r26;\n"
    "	mul.wide.s32 	%rd18, %r26, 4;\n"
    "	add.u64 	%rd19, %rd16, %rd18;\n"
    "	ld.param.u64 	%rd20, [__cudaparm__Z11matrixMulDSPfS_S_ii_A];\n"
    "	cvt.s64.s32 	%rd21, %r21;\n"
    "	mul.wide.s32 	%rd22, %r21, 4;\n"
    "	add.u64 	%rd23, %rd20, %rd22;\n"
    "	mov.f32 	%f1, 0f00000000;     	// 0\n"
    "	mov.s32 	%r27, %r17;\n"
    "$Lt_0_2818:\n"
    " //<loop> Loop body line 124, nesting depth: 1, estimated iterations: unknown\n"
    "	.loc	14	136	0\n"
    "	ld.global.f32 	%f2, [%rd23+0];\n"
    "	st.shared.f32 	[%rd14+0], %f2;\n"
    "	.loc	14	137	0\n"
    "	ld.global.f32 	%f3, [%rd19+0];\n"
    "	st.shared.f32 	[%rd15+0], %f3;\n"
    "	.loc	14	140	0\n"
    "	bar.sync 	0;\n"
    "	.loc	14	150	0\n"
    "	ld.shared.f32 	%f4, [%rd8+0];\n"
    "	ld.shared.f32 	%f5, [%rd6+0];\n"
    "	mad.f32 	%f6, %f4, %f5, %f1;\n"
    "	ld.shared.f32 	%f7, [%rd8+4];\n"
    "	ld.shared.f32 	%f8, [%rd6+128];\n"
    "	mad.f32 	%f9, %f7, %f8, %f6;\n"
    "	ld.shared.f32 	%f10, [%rd8+8];\n"
    "	ld.shared.f32 	%f11, [%rd6+256];\n"
    "	mad.f32 	%f12, %f10, %f11, %f9;\n"
    "	ld.shared.f32 	%f13, [%rd8+12];\n"
    "	ld.shared.f32 	%f14, [%rd6+384];\n"
    "	mad.f32 	%f15, %f13, %f14, %f12;\n"
    "	ld.shared.f32 	%f16, [%rd8+16];\n"
    "	ld.shared.f32 	%f17, [%rd6+512];\n"
    "	mad.f32 	%f18, %f16, %f17, %f15;\n"
    "	ld.shared.f32 	%f19, [%rd8+20];\n"
    "	ld.shared.f32 	%f20, [%rd6+640];\n"
    "	mad.f32 	%f21, %f19, %f20, %f18;\n"
    "	ld.shared.f32 	%f22, [%rd8+24];\n"
    "	ld.shared.f32 	%f23, [%rd6+768];\n"
    "	mad.f32 	%f24, %f22, %f23, %f21;\n"
    "	ld.shared.f32 	%f25, [%rd8+28];\n"
    "	ld.shared.f32 	%f26, [%rd6+896];\n"
    "	mad.f32 	%f27, %f25, %f26, %f24;\n"
    "	ld.shared.f32 	%f28, [%rd8+32];\n"
    "	ld.shared.f32 	%f29, [%rd6+1024];\n"
    "	mad.f32 	%f30, %f28, %f29, %f27;\n"
    "	ld.shared.f32 	%f31, [%rd8+36];\n"
    "	ld.shared.f32 	%f32, [%rd6+1152];\n"
    "	mad.f32 	%f33, %f31, %f32, %f30;\n"
    "	ld.shared.f32 	%f34, [%rd8+40];\n"
    "	ld.shared.f32 	%f35, [%rd6+1280];\n"
    "	mad.f32 	%f36, %f34, %f35, %f33;\n"
    "	ld.shared.f32 	%f37, [%rd8+44];\n"
    "	ld.shared.f32 	%f38, [%rd6+1408];\n"
    "	mad.f32 	%f39, %f37, %f38, %f36;\n"
    "	ld.shared.f32 	%f40, [%rd8+48];\n"
    "	ld.shared.f32 	%f41, [%rd6+1536];\n"
    "	mad.f32 	%f42, %f40, %f41, %f39;\n"
    "	ld.shared.f32 	%f43, [%rd8+52];\n"
    "	ld.shared.f32 	%f44, [%rd6+1664];\n"
    "	mad.f32 	%f45, %f43, %f44, %f42;\n"
    "	ld.shared.f32 	%f46, [%rd8+56];\n"
    "	ld.shared.f32 	%f47, [%rd6+1792];\n"
    "	mad.f32 	%f48, %f46, %f47, %f45;\n"
    "	ld.shared.f32 	%f49, [%rd8+60];\n"
    "	ld.shared.f32 	%f50, [%rd6+1920];\n"
    "	mad.f32 	%f51, %f49, %f50, %f48;\n"
    "	ld.shared.f32 	%f52, [%rd8+64];\n"
    "	ld.shared.f32 	%f53, [%rd6+2048];\n"
    "	mad.f32 	%f54, %f52, %f53, %f51;\n"
    "	ld.shared.f32 	%f55, [%rd8+68];\n"
    "	ld.shared.f32 	%f56, [%rd6+2176];\n"
    "	mad.f32 	%f57, %f55, %f56, %f54;\n"
    "	ld.shared.f32 	%f58, [%rd8+72];\n"
    "	ld.shared.f32 	%f59, [%rd6+2304];\n"
    "	mad.f32 	%f60, %f58, %f59, %f57;\n"
    "	ld.shared.f32 	%f61, [%rd8+76];\n"
    "	ld.shared.f32 	%f62, [%rd6+2432];\n"
    "	mad.f32 	%f63, %f61, %f62, %f60;\n"
    "	ld.shared.f32 	%f64, [%rd8+80];\n"
    "	ld.shared.f32 	%f65, [%rd6+2560];\n"
    "	mad.f32 	%f66, %f64, %f65, %f63;\n"
    "	ld.shared.f32 	%f67, [%rd8+84];\n"
    "	ld.shared.f32 	%f68, [%rd6+2688];\n"
    "	mad.f32 	%f69, %f67, %f68, %f66;\n"
    "	ld.shared.f32 	%f70, [%rd8+88];\n"
    "	ld.shared.f32 	%f71, [%rd6+2816];\n"
    "	mad.f32 	%f72, %f70, %f71, %f69;\n"
    "	ld.shared.f32 	%f73, [%rd8+92];\n"
    "	ld.shared.f32 	%f74, [%rd6+2944];\n"
    "	mad.f32 	%f75, %f73, %f74, %f72;\n"
    "	ld.shared.f32 	%f76, [%rd8+96];\n"
    "	ld.shared.f32 	%f77, [%rd6+3072];\n"
    "	mad.f32 	%f78, %f76, %f77, %f75;\n"
    "	ld.shared.f32 	%f79, [%rd8+100];\n"
    "	ld.shared.f32 	%f80, [%rd6+3200];\n"
    "	mad.f32 	%f81, %f79, %f80, %f78;\n"
    "	ld.shared.f32 	%f82, [%rd8+104];\n"
    "	ld.shared.f32 	%f83, [%rd6+3328];\n"
    "	mad.f32 	%f84, %f82, %f83, %f81;\n"
    "	ld.shared.f32 	%f85, [%rd8+108];\n"
    "	ld.shared.f32 	%f86, [%rd6+3456];\n"
    "	mad.f32 	%f87, %f85, %f86, %f84;\n"
    "	ld.shared.f32 	%f88, [%rd8+112];\n"
    "	ld.shared.f32 	%f89, [%rd6+3584];\n"
    "	mad.f32 	%f90, %f88, %f89, %f87;\n"
    "	ld.shared.f32 	%f91, [%rd8+116];\n"
    "	ld.shared.f32 	%f92, [%rd6+3712];\n"
    "	mad.f32 	%f93, %f91, %f92, %f90;\n"
    "	ld.shared.f32 	%f94, [%rd8+120];\n"
    "	ld.shared.f32 	%f95, [%rd6+3840];\n"
    "	mad.f32 	%f96, %f94, %f95, %f93;\n"
    "	ld.shared.f32 	%f97, [%rd8+124];\n"
    "	ld.shared.f32 	%f98, [%rd6+3968];\n"
    "	mad.f32 	%f1, %f97, %f98, %f96;\n"
    "	.loc	14	155	0\n"
    "	bar.sync 	0;\n"
    "	.loc	14	124	0\n"
    "	add.u64 	%rd19, %rd13, %rd19;\n"
    "	add.s32 	%r21, %r21, 32;\n"
    "	add.u64 	%rd23, %rd23, 128;\n"
    "	setp.le.s32 	%p2, %r21, %r24;\n"
    "	@%p2 bra 	$Lt_0_2818;\n"
    "	bra.uni 	$Lt_0_2306;\n"
    "$Lt_0_3330:\n"
    "	ld.param.s32 	%r11, [__cudaparm__Z11matrixMulDSPfS_S_ii_wB];\n"
    "	mul.lo.s32 	%r18, %r10, %r11;\n"
    "	mov.f32 	%f1, 0f00000000;     	// 0\n"
    "$Lt_0_2306:\n"
    "	.loc	14	161	0\n"
    "	ld.param.u64 	%rd24, [__cudaparm__Z11matrixMulDSPfS_S_ii_C];\n"
    "	mul.lo.s32 	%r28, %r11, %r3;\n"
    "	add.s32 	%r29, %r1, %r28;\n"
    "	mul.lo.s32 	%r30, %r29, 32;\n"
    "	add.s32 	%r31, %r18, %r30;\n"
    "	add.s32 	%r32, %r9, %r31;\n"
    "	cvt.s64.s32 	%rd25, %r32;\n"
    "	mul.wide.s32 	%rd26, %r32, 4;\n"
    "	add.u64 	%rd27, %rd24, %rd26;\n"
    "	st.global.f32 	[%rd27+0], %f1;\n"
    "	.loc	14	162	0\n"
    "	exit;\n"
    "$LDWend__Z11matrixMulDSPfS_S_ii:\n"
    "	} // _Z11matrixMulDSPfS_S_ii\n"
    "\n";
#pragma dscuda endofptx
#include "dscuda.h"
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
