static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda-4.1/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2012-01-12\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_00001d0b_00000000-9_p2p.cpp3.i (/tmp/ccBI#.F1D2le)\n"
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
    "	.file	2	\"/tmp/tmpxft_00001d0b_00000000-8_p2p.cudafe2.gpu\"\n"
    "	.file	3	\"/usr/lib/gcc/x86_64-linux-gnu/4.4.7/include/stddef.h\"\n"
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
    "	.file	14	\"/usr/local/cuda/include/common_functions.h\"\n"
    "	.file	15	\"/usr/local/cuda/include/math_functions.h\"\n"
    "	.file	16	\"/usr/local/cuda/include/math_constants.h\"\n"
    "	.file	17	\"/usr/local/cuda/include/device_functions.h\"\n"
    "	.file	18	\"/usr/local/cuda/include/sm_11_atomic_functions.h\"\n"
    "	.file	19	\"/usr/local/cuda/include/sm_12_atomic_functions.h\"\n"
    "	.file	20	\"/usr/local/cuda/include/sm_13_double_functions.h\"\n"
    "	.file	21	\"/usr/local/cuda/include/sm_20_atomic_functions.h\"\n"
    "	.file	22	\"/usr/local/cuda/include/sm_20_intrinsics.h\"\n"
    "	.file	23	\"/usr/local/cuda/include/surface_functions.h\"\n"
    "	.file	24	\"/usr/local/cuda/include/texture_fetch_functions.h\"\n"
    "	.file	25	\"/usr/local/cuda/include/math_functions_dbl_ptx1.h\"\n"
    "\n"
    "\n";
#pragma dscuda endofptx
#include "dscuda.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include <cutil.h>
#include <cutil_inline.h>

static const double MEGA  = 1e6;

enum {
    COPY_BY_MEMCPY,
    COPY_BY_MEMCPYPEER,
};

// warn CUDA API errors, but do not exit.
#define unsafeCall(err)           __unsafeCall   (err, __FILE__, __LINE__)

static inline void
__unsafeCall(cudaError err, const char *file, const int line)
{
    if (cudaSuccess != err) {
        fprintf(stderr, "%s(%i) : cudaSafeCall() Runtime API error %d: %s.\n",
                file, line, (int)err, cudaGetErrorString(err));
    }
}

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

int
main(int argc, char **argv)
{
    double ratio = 2.5;
    double nloop = 2e8;
    double sized;
    double now, dt;
    int maxsize = 1024 * 1024 * 10.0;
    int i, j, size;
    int *bufA, *bufB;
    int *dbufA, *dbufB;
    int srcdev, dstdev;
    int copyapi;
    char copyapistr[128];

    if (argc < 4) {
        fprintf(stderr,
                "copies an array of int from one device to another.\n"
                "usage: %s <c|p> <src_dev> <dst_dev>\n"
                "           'c' for cudaMemcpy()\n"
                "           'p' for cudaMemcpyPeer()\n",
                argv[0]);
        exit(1);
    }
    switch (argv[1][0]) {
      case 'c':
        copyapi = COPY_BY_MEMCPY;
        sprintf(copyapistr, "cudaMemcpy() ");
        break;
      case 'p':
        copyapi = COPY_BY_MEMCPYPEER;
        sprintf(copyapistr, "cudaMemcpyPeer() ");
        break;
      default:
        fprintf(stderr, "arg1 should be 'c' or 'p'.\n");
        exit(1);
    }

    srcdev = atoi(argv[2]);
    dstdev = atoi(argv[3]);
    fprintf(stderr, "%s from device %d to device %d.\n",
            copyapistr, srcdev, dstdev);

    bufA = (int *)malloc(maxsize);
    bufB = (int *)malloc(maxsize);

    cutilSafeCall(cudaSetDevice(srcdev));
    cudaMalloc((void**) &dbufA, maxsize);
    cutilSafeCall(cudaSetDevice(dstdev));
    cudaMalloc((void**) &dbufB, maxsize);

    // set randomly generated data to the source device.
    for (i = 0; i < maxsize / sizeof(int); i++) {
        bufA[i] = rand() % 64;
        bufB[i] = rand() % 64;
    }
    cutilSafeCall(cudaSetDevice(srcdev));
    cutilSafeCall(cudaMemcpy(dbufA, bufA, maxsize, cudaMemcpyHostToDevice));
    cutilSafeCall(cudaSetDevice(dstdev));

    // copy data from the source device to the destination device.
    if (copyapi == COPY_BY_MEMCPY) {
        cutilSafeCall(cudaMemcpy(dbufB, dbufA, maxsize, cudaMemcpyDefault));
    }
    else {
        unsafeCall(cudaMemcpyPeer(dbufB, dstdev, dbufA, srcdev, maxsize));
    }

    // send the data back from the destination device to the host.
    cutilSafeCall(cudaMemcpy(bufB, dbufB, maxsize, cudaMemcpyDeviceToHost));

    // comparing the result with the original to check the correctness of
    // cudaMemcpy() / cudaMemcpyPeer()
    for (i = 0; i < maxsize / sizeof(int); i++) {
        if (bufA[i] != bufB[i]) {
            fprintf(stderr, "NG\n");
            fprintf(stderr, "bufA[%d]:0x%08x\n", i, bufA[i]);
            fprintf(stderr, "bufB[%d]:0x%08x\n", i, bufB[i]);
            exit(1);
        }
    }
    fprintf(stderr, "OK\n");

    // measure the bandwidth.
    for (sized = 4096; sized < maxsize; sized *= ratio) {
        //    for ( nloop = 2e8, sized = 4096 * 1; ; ) { // !!!
        size = (size_t)sized;

	get_cputime(&now, &dt);
	for (j = 0; j < nloop/size; j++) {

            if (copyapi == COPY_BY_MEMCPY) {
                cudaMemcpy(dbufB, dbufA, size, cudaMemcpyDefault);
            }
            else {
                cudaMemcpyPeer(dbufB, dstdev, dbufA, srcdev, size);
            }
        }
        cudaDeviceSynchronize();
	get_cputime(&now, &dt);
	printf("%d byte    %f sec    %f MB/s\n",
               size, dt, nloop/MEGA/dt);
	fflush(stdout);
    }

    exit(0);
}
