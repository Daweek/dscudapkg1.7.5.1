static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda-6.0/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2014-03-13\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_000006d2_00000000-9_broadcast.cpp3.i (/tmp/ccBI#.172qxP)\n"
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
    "	.file	2	\"/tmp/tmpxft_000006d2_00000000-8_broadcast.cudafe2.gpu\"\n"
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
    "	.file	14	\"/usr/local/cuda/include/common_functions.h\"\n"
    "	.file	15	\"/usr/local/cuda/include/math_functions.h\"\n"
    "	.file	16	\"/usr/local/cuda/include/math_constants.h\"\n"
    "	.file	17	\"/usr/local/cuda/include/device_functions.h\"\n"
    "	.file	18	\"/usr/local/cuda/include/sm_11_atomic_functions.h\"\n"
    "	.file	19	\"/usr/local/cuda/include/sm_12_atomic_functions.h\"\n"
    "	.file	20	\"/usr/local/cuda/include/sm_13_double_functions.h\"\n"
    "	.file	21	\"/usr/local/cuda/include/sm_20_atomic_functions.h\"\n"
    "	.file	22	\"/usr/local/cuda/include/sm_32_atomic_functions.h\"\n"
    "	.file	23	\"/usr/local/cuda/include/sm_35_atomic_functions.h\"\n"
    "	.file	24	\"/usr/local/cuda/include/sm_20_intrinsics.h\"\n"
    "	.file	25	\"/usr/local/cuda/include/sm_30_intrinsics.h\"\n"
    "	.file	26	\"/usr/local/cuda/include/sm_32_intrinsics.h\"\n"
    "	.file	27	\"/usr/local/cuda/include/sm_35_intrinsics.h\"\n"
    "	.file	28	\"/usr/local/cuda/include/surface_functions.h\"\n"
    "	.file	29	\"/usr/local/cuda/include/texture_fetch_functions.h\"\n"
    "	.file	30	\"/usr/local/cuda/include/texture_indirect_functions.h\"\n"
    "	.file	31	\"/usr/local/cuda/include/surface_indirect_functions.h\"\n"
    "	.file	32	\"/usr/local/cuda/include/math_functions_dbl_ptx1.h\"\n"
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

cudaError_t cudaMemcpyToAlldev(int ndev, void **dst, const void *src, size_t count, enum cudaMemcpyKind kind);

static void
get_cputime(double *splittime, double *laptime)
{
    struct timeval x;

    gettimeofday(&x, NULL);

    *splittime = x.tv_sec + x.tv_usec/1000000.0 - *laptime;
    *laptime = x.tv_sec + x.tv_usec/1000000.0;
}

static void
bcastperf(int argc, char **argv)
{
    int maxsize = 1024 * 1024 * 10.0;
    int i, j;
    size_t size0 = 4096, size;
    double sized;
    double lt = 0.0, st = 0.0;
    double ratio = 2.0;
    double nloop = 2e8;
    char *src = (char *)malloc(sizeof(char) * maxsize);
    char *dst[MAXDEV];
    int ndev0 = 1, ndev, ndevmax;
    static int nthread = 0;

    if (1 < argc) {
        ndev0 = atoi(argv[1]);
    }
    if (2 < argc) {
        size0 = atoi(argv[2]);
    }
    cutilSafeCall(cudaGetDeviceCount(&ndevmax));
    printf("# %d device%s found.\n", ndevmax, ndevmax > 1 ? "s" : "");

    for (i = 0; i < ndevmax; i++) {
        cudaSetDevice(i);
        cutilSafeCall(cudaMalloc((void**) &dst[i], sizeof(char) * maxsize));
    }
    printf("\n#\n# cudaMemcpy (HostToDevice)\n");
    printf("# broadcast to %d..%d servers.\n#\n", ndev0, ndevmax);

    for (sized = size0; sized < maxsize; sized *= ratio) {
        //    for ( nloop = 2e8, sized = 4096 * 1; ; ) { // !!!
        size = (size_t)sized;

        for (ndev = ndev0; ndev <= ndevmax; ndev++) { // # of devices broadcast to.
            get_cputime(&lt, &st);
#pragma omp parallel for private(j)
            for (i = 0; i < ndev; i++) {
#ifdef _OPENMP
                if (nthread == 0) {
                    nthread = omp_get_num_threads();
                    fprintf(stderr, "nthread:%d\n", nthread);
                }
#endif // _OPENMP
                for (j = 0; j < nloop/size; j++) { // # of iterations.
                    cudaSetDevice(i);
                    cudaMemcpy(dst[i], src, size, cudaMemcpyHostToDevice);
                } // i
                cudaDeviceSynchronize();
            } // j
            get_cputime(&lt, &st);
            printf("%d devices %d byte    %f sec    %f MB/s   %f MB/s\n",
                   ndev, size, lt, nloop/MEGA/lt, nloop/MEGA/lt*ndev);
            fflush(stdout);
	} // ndev
    } // sized
}

int
main(int argc, char **argv)
{
    bcastperf(argc, argv);
    fprintf(stderr, "going to quit...\n");
    sleep(1);
    exit(0);
}
