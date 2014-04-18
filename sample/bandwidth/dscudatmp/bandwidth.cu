static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda-4.1/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2012-01-12\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_0000474d_00000000-9_bandwidth.cpp3.i (/tmp/ccBI#.mh2XED)\n"
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
    "	.file	2	\"/tmp/tmpxft_0000474d_00000000-8_bandwidth.cudafe2.gpu\"\n"
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
#include <unistd.h>
#include <cutil.h>
#include <cutil_inline.h>
#include <sys/time.h>
#ifdef _OPENMP
#include <omp.h>
#endif // _OPENMP

#define MAXDEV 32
static const double MEGA  = 1e6;
static const double MICRO = 1e-6;

cudaError_t cudaMemcpyToAlldev(int ndev, void **dst, const void *src, size_t count, enum cudaMemcpyKind kind);

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
bcastperf(int argc, char **argv)
{
    int maxsize = 1024 * 1024 * 10.0;
    int i, j;
    size_t size;
    double sized;
    double now = 0.0, dt = 0.0;
    double ratio = 2.5;
    double nloop = 2e8;
    char *src = (char *)malloc(sizeof(char) * maxsize);
    char *dst[MAXDEV];
    int ndev;
    static int nthread = 0;

    cutilSafeCall(cudaGetDeviceCount(&ndev));
    printf("# %d device%s found.\n", ndev, ndev > 1 ? "s" : "");

    for (i = 0; i < ndev; i++) {
        cudaSetDevice(i);
        cutilSafeCall(cudaMalloc((void**) &dst[i], sizeof(char) * maxsize));
    }
    printf("\n#\n# cudaMemcpy (HostToDevice)\n");
    printf("# broadcast to %d servers.\n#\n", ndev);

    for (sized = 4096; sized < maxsize; sized *= ratio) {
        //    for ( nloop = 2e8, sized = 4096 * 1; ; ) { // !!!
        size = (size_t)sized;

	get_cputime(&now, &dt);
	for (j = 0; j < nloop/size; j++) {
#if 1
#pragma omp parallel for
	    for (i = 0; i < ndev; i++) {
#ifdef _OPENMP
                if (nthread == 0) {
                    nthread = omp_get_num_threads();
                    fprintf(stderr, "nthread:%d\n", nthread);
                }
#endif // _OPENMP
  	        cudaSetDevice(i);
                cudaMemcpy(dst[i], src, size, cudaMemcpyHostToDevice);
	    }
#else
            cudaMemcpyToAlldev(ndev, (void **)dst, src, size, cudaMemcpyHostToDevice);
#endif
	}
        cudaDeviceSynchronize();
	get_cputime(&now, &dt);
	printf("%d byte    %f sec    %f MB/s\n",
               size, dt, nloop/MEGA/dt);
	fflush(stdout);
    }
}


static void
sendperf(int argc, char **argv)
{
    int maxsize = 1024 * 1024 * 10.0;
    int i, j;
    size_t size;
    double sized;
    double now = 0.0, dt = 0.0;
    double ratio = 2.5;
    double nloop = 2e8;
    char *src[MAXDEV];
    char *dst[MAXDEV];
    int ndev;
    cutilSafeCall(cudaGetDeviceCount(&ndev));

    ndev = 1; // !!!

    printf("# %d device%s found.\n", ndev, ndev > 1 ? "s" : "");
    for (i = 0; i < ndev; i++) {
        cudaSetDevice(i);
        cutilSafeCall(cudaMalloc((void**) &dst[i], sizeof(char) * maxsize));
	src[i] = (char *)malloc(sizeof(char) * maxsize);
    }
    printf("\n#\n# cudaMemcpy (HostToDevice)\n#\n");

#if 1
    nloop = 2e8;
    for (sized = 4096; sized < maxsize; sized *= ratio) {

        size = (size_t)sized;

	get_cputime(&now, &dt);
	for (j = 0; j < nloop/size; j++) {
	    for (i = 0; i < ndev; i++) {
  	        cudaSetDevice(i);
                cudaMemcpy(dst[i], src[i], size, cudaMemcpyHostToDevice);
	    }
	}
        cudaDeviceSynchronize();
	get_cputime(&now, &dt);

#if 0 // with estimated RPC overhead.
	double throughput = 1700.0; // MB/s
	double latency    = 60.0; // us
	double ibsec = nloop / (throughput * MEGA) + latency * MICRO * nloop / size;
	printf("%d byte    %f sec    %f MB/s    %f ib_sec  %f MB/s\n",
	       size, lt, nloop/MEGA/lt, ibsec, nloop/MEGA/(lt + ibsec));
#else
	  printf("%d byte    %f sec    %f MB/s\n",
	  size, dt, nloop/MEGA/dt);
#endif
	fflush(stdout);
    }

#else
          size = 40;
          for (i = 0; i < ndev; i++) {
              for (j = 0; j < size; j++) {
                  src[i][j] = j;
              }
          }
          for (i = 0; i < ndev; i++) {
              cudaSetDevice(i);
              cudaMemcpy(dst[i], src[i], size, cudaMemcpyHostToDevice);
          }
#endif
}

static void
receiveperf(int argc, char **argv)
{
    int maxsize = 1024 * 1024 * 10.0;
    int i, j;
    size_t size;
    double sized;
    double now = 0.0, dt = 0.0;
    double ratio = 2.5;
    double nloop = 2e8;
    char *src[MAXDEV];
    char *dst[MAXDEV];
    int ndev;
    cutilSafeCall(cudaGetDeviceCount(&ndev));

    ndev = 1; // !!!

    printf("# %d device%s found.\n", ndev, ndev > 1 ? "s" : "");
    for (i = 0; i < ndev; i++) {
        cudaSetDevice(i);
        cutilSafeCall(cudaMalloc((void**) &src[i], sizeof(char) * maxsize));
	dst[i] = (char *)malloc(sizeof(char) * maxsize);
    }
    printf("\n#\n# cudaMemcpy (DeviceToHost)\n#\n");


    nloop = 2e9;
    for (sized = 4096; sized < maxsize; sized *= ratio) {

        size = (size_t)sized;

	get_cputime(&now, &dt);
	for (j = 0; j < nloop/size; j++) {
	    for (i = 0; i < ndev; i++) {
  	        cudaSetDevice(i);
                cudaMemcpy(dst[i], src[i], size, cudaMemcpyDeviceToHost);
	    }
	}
        cudaDeviceSynchronize();
	get_cputime(&now, &dt);
	printf("%d byte    %f sec    %f MB/s\n",
               size, dt, nloop/MEGA/dt);
	fflush(stdout);
    }
}

int
main(int argc, char **argv)
{
    int ndev;
    cutilSafeCall(cudaGetDeviceCount(&ndev));

    if (1 < ndev) {
        bcastperf(argc, argv);
    }
    sendperf(argc, argv);
    receiveperf(argc, argv);

    fprintf(stderr, "going to quit...\n");
    sleep(1);
    exit(0);
}