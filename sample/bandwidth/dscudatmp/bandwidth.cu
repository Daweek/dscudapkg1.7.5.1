static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda-6.0/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2014-03-13\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_00004525_00000000-9_bandwidth.cpp3.i (/tmp/ccBI#.XDM1kw)\n"
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
    "	.file	3	\"/tmp/tmpxft_00004525_00000000-8_bandwidth.cudafe2.gpu\"\n"
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
    "\n";
#pragma dscuda endofptx
#include "dscuda.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <cutil.h>
#include <cutil_inline.h>

#define MAXDEV 2
#define NLOOP 8e8
#define PKG	  1024
#define MAXSIZE 1024*1024*300
static const double MEGA  = 1e6;
static const double MICRO = 1e-6;

static void
get_cputime(double *nowp, double *deltap)
{
    struct timeval t;
    double now0;

    gettimeofday(&t, NULL);
    now0 = t.tv_sec + t.tv_usec/1000000.0;
    *deltap = now0 - *nowp;
    *nowp   = now0;
}

static void
sendperf(int argc, char **argv)
{
    int maxsize = MAXSIZE;
    int i, j;
    size_t size;
    double sized;
    double now = 0.0, dt = 0.0;
    double ratio = 2;
    double nloop = NLOOP;
    char *src[MAXDEV];
    char *dst[MAXDEV];
    int ndev;

    ndev = 1; // !!!

    printf("# %d device%s found.\n", ndev, ndev > 1 ? "s" : "");
    for (i = 0; i < ndev; i++) {
        cutilSafeCall(cudaMalloc((void**) &dst[i], sizeof(char) * maxsize));
        src[i] = (char *)malloc(sizeof(char) * maxsize);
    }
    printf("\n#\n# cudaMemcpy (HostToDevice)\n#\n");

    for (sized = PKG; sized < maxsize; sized *= ratio) {
        size = (size_t)sized;
        get_cputime(&now, &dt);
        for (j = 0; j < nloop/size; j++) {
        	for (i = 0; i < ndev; i++) {
        		cudaMemcpy(dst[i], src[i], size, cudaMemcpyHostToDevice);
        	}
        }
        cudaDeviceSynchronize();
        get_cputime(&now, &dt);
        printf("%d byte    %f sec    %f MB/s\n", size, dt, nloop/MEGA/dt);
    }
    cutilSafeCall(cudaFree(dst[0]));
}

static void
receiveperf(int argc, char **argv)
{
    int maxsize = MAXSIZE;
    int i, j;
    size_t size;
    double sized;
    double now = 0.0, dt = 0.0;
    double ratio = 2;
    double nloop = NLOOP;
    char *src[MAXDEV];
    char *dst[MAXDEV];
    int ndev;

    ndev = 1; // !!!

    printf("# %d device%s found.\n", ndev, ndev > 1 ? "s" : "");
    for (i = 0; i < ndev; i++) {
    	cutilSafeCall(cudaMalloc((void**) &src[i], sizeof(char) * maxsize));
    	dst[i] = (char *)malloc(sizeof(char) * maxsize);
    }
    printf("\n#\n# cudaMemcpy (DeviceToHost)\n#\n");

    for (sized = PKG; sized < maxsize; sized *= ratio) {
    	size = (size_t)sized;
		get_cputime(&now, &dt);
		for (j = 0; j < nloop/size; j++) {
			for (i = 0; i < ndev; i++) {
				cudaMemcpy(dst[i], src[i], size, cudaMemcpyDeviceToHost);
		}
	}
		cudaDeviceSynchronize();
		get_cputime(&now, &dt);
		printf("%d byte    %f sec    %f MB/s\n",size, dt, nloop/MEGA/dt);
	}
    cutilSafeCall(cudaFree(src[0]));
}

int main(int argc, char **argv)
{
	fprintf(stderr,"Starting Bandwidth Test...\n");
	printf("Info:\nMax size:%d Byte\nPKGsize:%d Byte\nLOOP:%d\n\n",(int)MAXSIZE,(int)PKG,(int)NLOOP);
    sendperf(argc, argv);
    receiveperf(argc, argv);

    fprintf(stderr, "Finishing Bandwidth Test...\n");
    return 0;
}
