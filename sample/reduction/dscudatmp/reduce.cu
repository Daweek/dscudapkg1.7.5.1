static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda-6.0/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2014-03-13\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_00001a01_00000000-9_reduce.cpp3.i (/tmp/ccBI#.LKNZh2)\n"
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
    "	.file	2	\"/tmp/tmpxft_00001a01_00000000-8_reduce.cudafe2.gpu\"\n"
    "	.file	3	\"/usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h\"\n"
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
    "	.file	14	\"reduce.cuh\"\n"
    "	.file	15	\"/usr/local/cuda/include/common_functions.h\"\n"
    "	.file	16	\"/usr/local/cuda/include/math_functions.h\"\n"
    "	.file	17	\"/usr/local/cuda/include/math_constants.h\"\n"
    "	.file	18	\"/usr/local/cuda/include/device_functions.h\"\n"
    "	.file	19	\"/usr/local/cuda/include/sm_11_atomic_functions.h\"\n"
    "	.file	20	\"/usr/local/cuda/include/sm_12_atomic_functions.h\"\n"
    "	.file	21	\"/usr/local/cuda/include/sm_13_double_functions.h\"\n"
    "	.file	22	\"/usr/local/cuda/include/sm_20_atomic_functions.h\"\n"
    "	.file	23	\"/usr/local/cuda/include/sm_32_atomic_functions.h\"\n"
    "	.file	24	\"/usr/local/cuda/include/sm_35_atomic_functions.h\"\n"
    "	.file	25	\"/usr/local/cuda/include/sm_20_intrinsics.h\"\n"
    "	.file	26	\"/usr/local/cuda/include/sm_30_intrinsics.h\"\n"
    "	.file	27	\"/usr/local/cuda/include/sm_32_intrinsics.h\"\n"
    "	.file	28	\"/usr/local/cuda/include/sm_35_intrinsics.h\"\n"
    "	.file	29	\"/usr/local/cuda/include/surface_functions.h\"\n"
    "	.file	30	\"/usr/local/cuda/include/texture_fetch_functions.h\"\n"
    "	.file	31	\"/usr/local/cuda/include/texture_indirect_functions.h\"\n"
    "	.file	32	\"/usr/local/cuda/include/surface_indirect_functions.h\"\n"
    "	.file	33	\"/usr/local/cuda/include/math_functions_dbl_ptx1.h\"\n"
    "\n"
    "	.extern	.shared .align 4 .b8 __smem[];\n"
    "\n"
    "	.entry _Z6reduceiPiS_ (\n"
    "		.param .s32 __cudaparm__Z6reduceiPiS__n,\n"
    "		.param .u64 __cudaparm__Z6reduceiPiS__g_idata,\n"
    "		.param .u64 __cudaparm__Z6reduceiPiS__g_odata)\n"
    "	{\n"
    "	.reg .u32 %r<20>;\n"
    "	.reg .u64 %rd<17>;\n"
    "	.reg .pred %p<7>;\n"
    "	.loc	14	2	0\n"
    "$LDWbegin__Z6reduceiPiS_:\n"
    "	cvt.u32.u16 	%r1, %ntid.x;\n"
    "	cvt.u32.u16 	%r2, %ctaid.x;\n"
    "	mul.lo.u32 	%r3, %r1, %r2;\n"
    "	cvt.u32.u16 	%r4, %tid.x;\n"
    "	add.u32 	%r5, %r3, %r4;\n"
    "	ld.param.s32 	%r6, [__cudaparm__Z6reduceiPiS__n];\n"
    "	setp.le.u32 	%p1, %r6, %r5;\n"
    "	@%p1 bra 	$Lt_0_3842;\n"
    "	.loc	14	11	0\n"
    "	ld.param.u64 	%rd1, [__cudaparm__Z6reduceiPiS__g_idata];\n"
    "	cvt.u64.u32 	%rd2, %r5;\n"
    "	mul.wide.u32 	%rd3, %r5, 4;\n"
    "	add.u64 	%rd4, %rd1, %rd3;\n"
    "	ld.global.s32 	%r7, [%rd4+0];\n"
    "	bra.uni 	$Lt_0_3586;\n"
    "$Lt_0_3842:\n"
    "	mov.s32 	%r7, 0;\n"
    "$Lt_0_3586:\n"
    "	mov.u64 	%rd5, __smem;\n"
    "	cvt.u64.u32 	%rd6, %r4;\n"
    "	mul.wide.u32 	%rd7, %r4, 4;\n"
    "	add.u64 	%rd8, %rd5, %rd7;\n"
    "	st.shared.s32 	[%rd8+0], %r7;\n"
    "	.loc	14	13	0\n"
    "	bar.sync 	0;\n"
    "	mov.u32 	%r8, 1;\n"
    "	setp.le.u32 	%p2, %r1, %r8;\n"
    "	@%p2 bra 	$Lt_0_4098;\n"
    "	mov.u32 	%r9, 1;\n"
    "$Lt_0_4610:\n"
    "	mul.lo.u32 	%r10, %r9, 2;\n"
    "	rem.u32 	%r11, %r4, %r10;\n"
    "	mov.u32 	%r12, 0;\n"
    "	setp.ne.u32 	%p3, %r11, %r12;\n"
    "	@%p3 bra 	$Lt_0_4866;\n"
    "	.loc	14	20	0\n"
    "	ld.shared.s32 	%r13, [%rd8+0];\n"
    "	add.u32 	%r14, %r9, %r4;\n"
    "	cvt.u64.u32 	%rd9, %r14;\n"
    "	mul.wide.u32 	%rd10, %r14, 4;\n"
    "	add.u64 	%rd11, %rd5, %rd10;\n"
    "	ld.shared.s32 	%r15, [%rd11+0];\n"
    "	add.s32 	%r16, %r13, %r15;\n"
    "	st.shared.s32 	[%rd8+0], %r16;\n"
    "$Lt_0_4866:\n"
    "	.loc	14	22	0\n"
    "	bar.sync 	0;\n"
    "	.loc	14	17	0\n"
    "	mov.s32 	%r9, %r10;\n"
    "	setp.lt.u32 	%p4, %r10, %r1;\n"
    "	@%p4 bra 	$Lt_0_4610;\n"
    "$Lt_0_4098:\n"
    "	mov.u32 	%r17, 0;\n"
    "	setp.ne.u32 	%p5, %r4, %r17;\n"
    "	@%p5 bra 	$Lt_0_5634;\n"
    "	.loc	14	27	0\n"
    "	ld.shared.s32 	%r18, [__smem+0];\n"
    "	ld.param.u64 	%rd12, [__cudaparm__Z6reduceiPiS__g_odata];\n"
    "	cvt.u64.u32 	%rd13, %r2;\n"
    "	mul.wide.u32 	%rd14, %r2, 4;\n"
    "	add.u64 	%rd15, %rd12, %rd14;\n"
    "	st.global.s32 	[%rd15+0], %r18;\n"
    "$Lt_0_5634:\n"
    "	.loc	14	29	0\n"
    "	exit;\n"
    "$LDWend__Z6reduceiPiS_:\n"
    "	} // _Z6reduceiPiS_\n"
    "\n";
#pragma dscuda endofptx
#include "dscuda.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cutil.h>
#include <cutil_inline.h>

#include "reduce.cuh"

#define NTHREAD    (64)
#define NBLOCKMAX  (65536)

int
main(int argc, char **argv)
{
    int i, nelem, nblock;
    int sum, h_sum;
    int *h_idata, *h_odata;
    int *d_idata, *d_odata;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <# of elements>\n", argv[0]);
        exit(1);
    }
    nelem = atoi(argv[1]);
    nblock = (nelem - 1) / NTHREAD + 1;
    if (NBLOCKMAX < nblock) {
        fprintf(stderr, "# of elements exceeds the limit (=%d).\n", NTHREAD * NBLOCKMAX);
        exit(1);
    }
    fprintf(stderr, "nelem:%d  nthread:%d  nblock:%d\n", nelem, NTHREAD, nblock);

    h_idata = (int *)malloc(sizeof(int) * nelem);
    h_odata = (int *)malloc(sizeof(int) * nblock);
    cutilSafeCall(cudaMalloc((void**) &d_idata, sizeof(int) * nelem));
    cutilSafeCall(cudaMalloc((void**) &d_odata, sizeof(int) * nblock));

    h_sum = 0;
    for (i = 0; i < nelem; i++) {
        h_idata[i] = lrand48() % (1 << 8);
        h_sum += h_idata[i];
    }
    cutilSafeCall(cudaMemcpy(d_idata, h_idata, sizeof(int) * nelem, cudaMemcpyHostToDevice));

    for (i = 0; i < nblock; i++) {
        h_odata[i] = 0;
    }
    cutilSafeCall(cudaMemcpy(d_odata, h_odata, sizeof(int) * nblock, cudaMemcpyHostToDevice));

    dim3 threads(NTHREAD, 1, 1);
    dim3 grids(nblock, 1, 1);
    int smemsize = sizeof(int) * NTHREAD;

    reduce<<<grids, threads, smemsize>>>(nelem, d_idata, d_odata);

    cutilSafeCall(cudaMemcpy(h_odata, d_odata, sizeof(int) * nblock, cudaMemcpyDeviceToHost));    

    sum = 0;
    for (i = 0; i < nblock; i++) {
        fprintf(stderr, "block[%d]:%d\n", i, h_odata[i]);
        sum += h_odata[i];
    }
    printf("  sum: %d\n", sum);
    printf("h_sum: %d\n", h_sum);
}
