static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda-6.0/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2014-03-13\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_00006b0d_00000000-9_vecadd.cpp3.i (/tmp/ccBI#.N85Oiu)\n"
    "	//-----------------------------------------------------------\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Options:\n"
    "	//-----------------------------------------------------------\n"
    "	//  Target:ptx, ISA:sm_10, Endian:little, Pointer Size:32\n"
    "	//  -O3	(Optimization level)\n"
    "	//  -g0	(Debug level)\n"
    "	//  -m2	(Report advisories)\n"
    "	//-----------------------------------------------------------\n"
    "\n"
    "	.file	1	\"<command-line>\"\n"
    "	.file	2	\"/usr/include/stdc-predef.h\"\n"
    "	.file	3	\"/tmp/tmpxft_00006b0d_00000000-8_vecadd.cudafe2.gpu\"\n"
    "	.file	4	\"/usr/lib/gcc/i686-linux-gnu/4.8/include/stddef.h\"\n"
    "	.file	5	\"/usr/local/cuda/include/crt/device_runtime.h\"\n"
    "	.file	6	\"/usr/local/cuda/include/host_defines.h\"\n"
    "	.file	7	\"/usr/local/cuda/include/builtin_types.h\"\n"
    "	.file	8	\"/usr/local/cuda/include/device_types.h\"\n"
    "	.file	9	\"/usr/local/cuda/include/driver_types.h\"\n"
    "	.file	10	\"/usr/local/cuda/include/surface_types.h\"\n"
    "	.file	11	\"/usr/local/cuda/include/texture_types.h\"\n"
    "	.file	12	\"/usr/local/cuda/include/vector_types.h\"\n"
    "	.file	13	\"/usr/local/cuda/include/device_launch_parameters.h\"\n"
    "	.file	14	\"/usr/local/cuda/include/crt/storage_class.h\"\n"
    "	.file	15	\"vecadd.cuh\"\n"
    "	.file	16	\"/usr/local/cuda/include/common_functions.h\"\n"
    "	.file	17	\"/usr/local/cuda/include/math_functions.h\"\n"
    "	.file	18	\"/usr/local/cuda/include/math_constants.h\"\n"
    "	.file	19	\"/usr/local/cuda/include/device_functions.h\"\n"
    "	.file	20	\"/usr/local/cuda/include/sm_11_atomic_functions.h\"\n"
    "	.file	21	\"/usr/local/cuda/include/sm_12_atomic_functions.h\"\n"
    "	.file	22	\"/usr/local/cuda/include/sm_13_double_functions.h\"\n"
    "	.file	23	\"/usr/local/cuda/include/sm_20_atomic_functions.h\"\n"
    "	.file	24	\"/usr/local/cuda/include/sm_32_atomic_functions.h\"\n"
    "	.file	25	\"/usr/local/cuda/include/sm_35_atomic_functions.h\"\n"
    "	.file	26	\"/usr/local/cuda/include/sm_20_intrinsics.h\"\n"
    "	.file	27	\"/usr/local/cuda/include/sm_30_intrinsics.h\"\n"
    "	.file	28	\"/usr/local/cuda/include/sm_32_intrinsics.h\"\n"
    "	.file	29	\"/usr/local/cuda/include/sm_35_intrinsics.h\"\n"
    "	.file	30	\"/usr/local/cuda/include/surface_functions.h\"\n"
    "	.file	31	\"/usr/local/cuda/include/texture_fetch_functions.h\"\n"
    "	.file	32	\"/usr/local/cuda/include/texture_indirect_functions.h\"\n"
    "	.file	33	\"/usr/local/cuda/include/surface_indirect_functions.h\"\n"
    "	.file	34	\"/usr/local/cuda/include/math_functions_dbl_ptx1.h\"\n"
    "\n"
    "\n"
    "	.entry _Z6vecAddPfS_S_ (\n"
    "		.param .u32 __cudaparm__Z6vecAddPfS_S__a,\n"
    "		.param .u32 __cudaparm__Z6vecAddPfS_S__b,\n"
    "		.param .u32 __cudaparm__Z6vecAddPfS_S__c)\n"
    "	{\n"
    "	.reg .u16 %rh<4>;\n"
    "	.reg .u32 %r<12>;\n"
    "	.reg .f32 %f<5>;\n"
    "	.loc	15	2	0\n"
    "$LDWbegin__Z6vecAddPfS_S_:\n"
    "	.loc	15	5	0\n"
    "	mov.u16 	%rh1, %ctaid.x;\n"
    "	mov.u16 	%rh2, %ntid.x;\n"
    "	mul.wide.u16 	%r1, %rh1, %rh2;\n"
    "	cvt.u32.u16 	%r2, %tid.x;\n"
    "	add.u32 	%r3, %r2, %r1;\n"
    "	mul.lo.u32 	%r4, %r3, 4;\n"
    "	ld.param.u32 	%r5, [__cudaparm__Z6vecAddPfS_S__a];\n"
    "	add.u32 	%r6, %r5, %r4;\n"
    "	ld.global.f32 	%f1, [%r6+0];\n"
    "	ld.param.u32 	%r7, [__cudaparm__Z6vecAddPfS_S__b];\n"
    "	add.u32 	%r8, %r7, %r4;\n"
    "	ld.global.f32 	%f2, [%r8+0];\n"
    "	add.f32 	%f3, %f1, %f2;\n"
    "	ld.param.u32 	%r9, [__cudaparm__Z6vecAddPfS_S__c];\n"
    "	add.u32 	%r10, %r9, %r4;\n"
    "	st.global.f32 	[%r10+0], %f3;\n"
    "	.loc	15	6	0\n"
    "	exit;\n"
    "$LDWend__Z6vecAddPfS_S_:\n"
    "	} // _Z6vecAddPfS_S_\n"
    "\n";
#pragma dscuda endofptx
#include "dscuda.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cutil.h>
#include <cutil_inline.h>

#include "vecadd.cuh"

#define N (8)

int
main(void)
{
	int i, t;
    float a[N], b[N], c[N];

    float *d_a, *d_b, *d_c;
    CUDA_SAFE_CALL(cudaMalloc((void**) &d_a, sizeof(float) * N));
    CUDA_SAFE_CALL(cudaMalloc((void**) &d_b, sizeof(float) * N));
    CUDA_SAFE_CALL(cudaMalloc((void**) &d_c, sizeof(float) * N));

    for (t = 0; t < 3; t++) {
        printf("try %d\n", t);
        for (i = 0; i < N; i++) {
            a[i] = rand()%64;
            b[i] = rand()%64;
        }
        CUDA_SAFE_CALL(cudaMemcpy(d_a, a, sizeof(float) * N, cudaMemcpyHostToDevice));
        CUDA_SAFE_CALL(cudaMemcpy(d_b, b, sizeof(float) * N, cudaMemcpyHostToDevice));
        int nth = 4;
        dim3 threads(nth, 1, 1);
        dim3 grids((N+nth-1)/nth, 1, 1);
        vecAdd<<<grids, threads>>>(d_a, d_b, d_c);
        CUDA_SAFE_CALL(cudaMemcpy(c, d_c, sizeof(float) * N, cudaMemcpyDeviceToHost));
        for (i = 0; i < N; i++) {
            printf("% 6.2f + % 6.2f = % 7.2f",
                   a[i], b[i], c[i]);
            if (a[i] + b[i] != c[i]) printf("   NG");
            printf("\n");
        }
        printf("\n");
    }

    exit(0);
}
