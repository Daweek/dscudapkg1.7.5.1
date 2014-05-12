#pragma begin dscuda.h
#ifndef _DSCUDA_H
#define _DSCUDA_H

#include <stdint.h>
#include <limits.h>
#include <cuda_runtime_api.h>
#include <builtin_types.h>
#include <driver_types.h>
#include <cuda_texture_types.h>
#include <texture_types.h>
#pragma begin dscudadefs.h
#ifndef _DSCUDADEFS_H
#define _DSCUDADEFS_H

#define RC_NSERVERMAX  4    
#define RC_NDEVICEMAX  4    
#define RC_NP2PMAX    (RC_NSERVERMAX * RC_NSERVERMAX)    
#define RC_NREDUNDANCYMAX 4 
#define RC_NVDEVMAX 64      
#define RC_NPTHREADMAX 64   

#define RC_BUFSIZE (1024*1024) 
#define RC_NKMODULEMAX 128  
#define RC_NKFUNCMAX   128  
#define RC_NSYMBOLMAX   128  
#define RC_NKARGMAX    64   
#define RC_KARGLEN     64   
#define RC_KMODULENAMELEN 128   
#define RC_KNAMELEN      128   
#define RC_KMODULEIMAGELEN (1024*1024*2)   
#define RC_SNAMELEN       128   

#define RC_CACHE_MODULE (1) 
#define RC_CLIENT_CACHE_LIFETIME (30) 
#define RC_SERVER_CACHE_LIFETIME (RC_CLIENT_CACHE_LIFETIME+30) 

#define RC_SUPPORT_PAGELOCK (0)  
#define RC_SUPPORT_STREAM (0)
#define RC_SUPPORT_CONCURRENT_EXEC (0)

#define RC_DAEMON_IP_PORT  (65432)
#define RC_SERVER_IP_PORT  (RC_DAEMON_IP_PORT+1)

#endif 
#pragma end dscudadefs.h
#pragma begin dscudamacros.h
#ifndef DSCUDA_MACROS_H
#define DSCUDA_MACROS_H

#define WARN(lv, fmt, args...) if (lv <= dscudaWarnLevel()) fprintf(stderr, fmt, ## args);

#define WARNONCE(lv, fmt, args...) if (lv <= dscudaWarnLevel()) { \
        static int firstcall = 1;                      \
        if (firstcall) {                                          \
            firstcall = 0;                                        \
            fprintf(stderr, fmt, ## args);                        \
        }                                                         \
    }

int dscudaWarnLevel(void);
void dscudaSetWarnLevel(int level);

#endif 
#pragma end dscudamacros.h
#pragma begin ibvdefs.h
#ifndef IBVDEFS_H
#define IBVDEFS_H

#ifndef TCP_ONLY

#include <rdma/rdma_cma.h>

#define TEST_NZ(x) do { if ( (x)) {WARN(0, #x " failed (returned non-zero).\n" ); exit(EXIT_FAILURE); } } while (0)
#define TEST_Z(x)  do { if (!(x)) {WARN(0, #x " failed (returned zero/null).\n"); exit(EXIT_FAILURE); } } while (0)


#define RC_NWR_PER_POST (16) 

#define RC_SGE_SIZE (1024 * 1024 * 16) 

#define RC_WR_MAX (RC_NWR_PER_POST * 16) 

#define RC_RDMA_BUF_SIZE (RC_NWR_PER_POST * RC_SGE_SIZE) 


#if RC_RDMA_BUF_SIZE  < RC_KMODULEIMAGELEN
#error "RC_RDMA_BUF_SIZE too small."

#endif

#define RC_SERVER_IBV_CQ_SIZE (RC_WR_MAX)
#define RC_CLIENT_IBV_CQ_SIZE (65536)

#define RC_IBV_IP_PORT_BASE  (65432)
#define RC_IBV_TIMEOUT (500)  

struct message {
    struct ibv_mr mr[RC_NWR_PER_POST];
};

enum rdma_state_t {
    STATE_INIT,
    STATE_READY,
    STATE_BUSY,
};

typedef struct {
    
    struct rdma_cm_id *id;
    struct ibv_qp *qp;
    struct ibv_context *ibvctx;
    struct ibv_pd *pd;
    struct ibv_cq *cq;
    struct ibv_comp_channel *comp_channel;

    
    struct message *recv_msg;
    struct message *send_msg;

    
    char *rdma_local_region;
    char *rdma_remote_region;

    
    struct ibv_mr *recv_mr;
    struct ibv_mr *send_mr;
    struct ibv_mr peer_mr[RC_NWR_PER_POST];

    
    struct ibv_mr *rdma_local_mr[RC_NWR_PER_POST];
    struct ibv_mr *rdma_remote_mr[RC_NWR_PER_POST];

    
    pthread_t cq_poller_thread;
    int connected;
    enum rdma_state_t rdma_state;
    int rdma_nreq_pending;
    pthread_mutex_t rdma_mutex;
    void (*on_completion_handler)(struct ibv_wc *);
} IbvConnection;

void rdmaBuildConnection(struct rdma_cm_id *id, bool is_server);
void rdmaBuildParams(struct rdma_conn_param *params);
void rdmaDestroyConnection(IbvConnection *conn);
void rdmaWaitEvent(struct rdma_event_channel *ec, rdma_cm_event_type et, int (*handler)(struct rdma_cm_id *id));
void rdmaWaitReadyToKickoff(IbvConnection *conn);
void rdmaWaitReadyToDisconnect(IbvConnection *conn);
void rdmaKickoff(IbvConnection *conn, int length);
void rdmaPipelinedKickoff(IbvConnection *conn, int length, char *payload_buf, char *payload_src, int payload_size);
void rdmaSendMr(IbvConnection *conn);

int dscudaMyServerId(void);

#endif 

#endif 
#pragma end ibvdefs.h
#pragma begin tcpdefs.h
#ifndef TCPDEFS_H
#define TCPDEFS_H

typedef struct {
    int svrsock;
    int sendbufsize;
    int recvbufsize;
    char *sendbuf;
    char *recvbuf;
} TcpConnection;

#define RC_SOCKET_BUF_SIZE (1024 * 1024 * 32)

#endif 
#pragma end tcpdefs.h

typedef unsigned long RCadr;
typedef unsigned long RCstream;
typedef unsigned long RCevent;
typedef unsigned long RCipaddr;
typedef unsigned int RCsize;
typedef unsigned long RCpid;
typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int z;
} RCdim3;

typedef unsigned int RCchannelformat;

typedef struct {
    int normalized;
    int filterMode;
    int addressMode[3];
    RCchannelformat f;
    int w;
    int x;
    int y;
    int z;
} RCtexture;

enum RCargType {
    dscudaArgTypeP = 0,
    dscudaArgTypeI = 1,
    dscudaArgTypeF = 2,
    dscudaArgTypeV = 3
};

typedef struct {
    int type;
    union {
        unsigned long pointerval;
        unsigned int intval;
        float floatval;
        char customval[RC_KARGLEN];
    } val;
    unsigned int offset;
    unsigned int size;
} RCArg;

typedef char *RCbuf;

typedef enum {
    RCMethodNone = 0,
    RCMethodMemcpyH2D, 
    RCMethodMemcpyD2H, 
    RCMethodMemcpyD2D, 
    RCMethodMemset,
    RCMethodMalloc, 
    RCMethodFree,
    RCMethodGetErrorString,
    RCMethodGetDeviceProperties,
    RCMethodRuntimeGetVersion,
    RCMethodThreadSynchronize,
    RCMethodThreadExit,
    RCMethodDeviceSynchronize,
    RCMethodCreateChannelDesc,
    RCMethodDeviceSetLimit,
    RCMethodDeviceSetSharedMemConfig,

    
    RCMethodDscudaMemcpyToSymbolH2D,
    RCMethodDscudaMemcpyToSymbolD2D,
    RCMethodDscudaMemcpyFromSymbolD2H,
    RCMethodDscudaMemcpyFromSymbolD2D,
    RCMethodDscudaMemcpyToSymbolAsyncH2D,
    RCMethodDscudaMemcpyToSymbolAsyncD2D,
    RCMethodDscudaMemcpyFromSymbolAsyncD2H,
    RCMethodDscudaMemcpyFromSymbolAsyncD2D,
    RCMethodDscudaLoadModule,
    RCMethodDscudaLaunchKernel,
    RCMethodDscudaBindTexture,
    RCMethodDscudaUnbindTexture,

    
    

    
    RCMethodLaunch,
    RCMethodConfigureCall,
    RCMethodSetupArgument,

    
    RCMethodDscudaRegisterFatBinary,
    RCMethodDscudaUnregisterFatBinary,
    RCMethodDscudaRegisterFunction,
    RCMethodDscudaRegisterVar,

    
    RCMethodDscudaSortByKey,

    RCMethodEnd, 

    
    RCMethodSetDevice,

} RCMethod;


typedef struct {
    RCMethod method;
    int payload;
} RCHdr;


typedef struct {
    RCMethod method;
    int payload;
    size_t count;
    RCadr dstadr;
    void *srcbuf;
} RCMemcpyH2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCMemcpyH2DReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    size_t count;
    RCadr srcadr;
} RCMemcpyD2HInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
    void *dstbuf;
} RCMemcpyD2HReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    size_t count;
    RCadr dstadr;
    RCadr srcadr;
} RCMemcpyD2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCMemcpyD2DReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int value;
    size_t count;
    RCadr devptr;
} RCMemsetInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCMemsetReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    size_t size;
} RCMallocInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
    RCadr devAdr;
} RCMallocReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    RCadr devAdr;
} RCFreeInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCFreeReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int device;
    cudaError_t err;
} RCGetErrorStringInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    char *errmsg;
} RCGetErrorStringReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int device;
} RCGetDevicePropertiesInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
    cudaDeviceProp prop;
} RCGetDevicePropertiesReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    char dummy[8];
} RCRuntimeGetVersionInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
    int version;
} RCRuntimeGetVersionReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    char dummy[8];
} RCThreadSynchronizeInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCThreadSynchronizeReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    char dummy[8];
} RCThreadExitInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCThreadExitReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    char dummy[8];
} RCDeviceSynchronizeInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDeviceSynchronizeReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int x, y, z, w;
    enum cudaChannelFormatKind f;
} RCCreateChannelDescInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaChannelFormatDesc desc;
} RCCreateChannelDescReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    cudaLimit limit;
    size_t value;
} RCDeviceSetLimitInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDeviceSetLimitReturnHdr;


#ifdef cudaSharedMemConfig
#define CUDA50ORLATER 1
#error AAAAA
#else
#define CUDA50ORLATER 0
#define cudaSharedMemConfig int 
#endif

typedef struct {
    RCMethod method;
    int payload;
    cudaSharedMemConfig config;
} RCDeviceSetSharedMemConfigInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDeviceSetSharedMemConfigReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
    void *srcbuf;
} RCDscudaMemcpyToSymbolH2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaMemcpyToSymbolH2DReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
    RCadr srcadr;
} RCDscudaMemcpyToSymbolD2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaMemcpyToSymbolD2DReturnHdr;



typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
} RCDscudaMemcpyFromSymbolD2HInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
    void *dstbuf;
} RCDscudaMemcpyFromSymbolD2HReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
    RCadr dstadr;
} RCDscudaMemcpyFromSymbolD2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaMemcpyFromSymbolD2DReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
    RCstream stream;
    void *src;
} RCDscudaMemcpyToSymbolAsyncH2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaMemcpyToSymbolAsyncH2DReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
    RCstream stream;
    RCadr srcadr;
} RCDscudaMemcpyToSymbolAsyncD2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaMemcpyToSymbolAsyncD2DReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
    RCstream stream;
} RCDscudaMemcpyFromSymbolAsyncD2HInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
    void *dst;
} RCDscudaMemcpyFromSymbolAsyncD2HReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char symbol[RC_SNAMELEN];
    size_t count;
    size_t offset;
    RCstream stream;
    RCadr dstadr;
} RCDscudaMemcpyFromSymbolAsyncD2DInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaMemcpyFromSymbolAsyncD2DReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    unsigned long long int ipaddr;
    unsigned long int pid;
    char modulename[RC_KMODULENAMELEN];
    void *moduleimage;
} RCDscudaLoadModuleInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
    int moduleid;
} RCDscudaLoadModuleReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    int kernelid;
    char kernelname[RC_KNAMELEN];
    unsigned int gdim[3];
    unsigned int bdim[3];
    unsigned int smemsize;
    RCstream stream;
    int narg;
    void *args;
} RCDscudaLaunchKernelInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaLaunchKernelReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char texname[RC_SNAMELEN];
    RCtexture texbuf;
    RCadr devptr;
    size_t size;
} RCDscudaBindTextureInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    size_t offset;
    cudaError_t err;
} RCDscudaBindTextureReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int moduleid;
    char texname[RC_SNAMELEN];
} RCDscudaUnbindTextureInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaUnbindTextureReturnHdr;



typedef struct {
    RCMethod method;
    int payload;
    RCadr func;
} RCLaunchInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCLaunchReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    unsigned int gdim[3];
    unsigned int bdim[3];
    unsigned int smemsize;
    RCstream stream;
} RCConfigureCallInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCConfigureCallReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int size;
    int offset;
    void *argbuf;
} RCSetupArgumentInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCSetupArgumentReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    size_t count;
    int m;
    int v;
    char f[256];
    void *fatbinbuf;
} RCDscudaRegisterFatBinaryInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    RCadr handle;
    cudaError_t err;
} RCDscudaRegisterFatBinaryReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    RCadr handle;
    cudaError_t err;
} RCDscudaUnregisterFatBinaryInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaUnregisterFatBinaryReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    RCadr handle;
    RCadr hfunc;
    char dfunc[RC_SNAMELEN];
    char dname[RC_SNAMELEN];
    int tlimit;
} RCDscudaRegisterFunctionInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaRegisterFunctionReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    RCadr handle;
    RCadr hvar;
    char dvar[RC_SNAMELEN];
    char dname[RC_SNAMELEN];
    int ext;
    int size;
    int constant;
    int global;
} RCDscudaRegisterVarInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaRegisterVarReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    int nitems;
    RCadr key;
    RCadr value;
} RCDscudaSortByKeyInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaSortByKeyReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    size_t count;
    RCadr srcadr;
    RCadr dstadr;
    unsigned int dstip;
    int port;
} RCDscudaSendP2PInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaSendP2PReturnHdr;


typedef struct {
    RCMethod method;
    int payload;
    unsigned int srcip;
    int port;
} RCDscudaRecvP2PInvokeHdr;

typedef struct {
    RCMethod method;
    int payload;
    cudaError_t err;
} RCDscudaRecvP2PReturnHdr;


typedef struct {
    RCMethod dummy;
    int payload;
    int size;
    size_t count;
    RCadr dstadr;
    void *srcbuf;
} P2PInvokeHdr;

typedef struct {
    RCMethod dummy;
    int payload;
    int size;
    cudaError_t err;
} P2PReturnHdr;

typedef struct {int m; int v; unsigned long long* d; char* f;} fatDeviceText_t;

enum {
    RC_REMOTECALL_TYPE_TCP,
    RC_REMOTECALL_TYPE_IBV,
};


char *dscudaMemcpyKindName(cudaMemcpyKind kind);

unsigned int dscudaServerNameToAddr(char *svrname);
unsigned int dscudaServerNameToDevid(char *svrname);
unsigned int dscudaServerIpStrToAddr(char *ipstr);
char *       dscudaAddrToServerIpStr(unsigned int addr);
int          dscudaAlignUp(int off, int align);
unsigned int dscudaRoundUp(unsigned int src, unsigned int by);
double       RCgetCputime(double *t0);


void *dscudaUvaOfAdr(void *adr, int devid);
int dscudaDevidOfUva(void *adr);
void *dscudaAdrOfUva(void *adr);
int dscudaNredundancy(void);
void dscudaSetAutoVerb(int verb);
int dscudaRemoteCallType(void);
void dscudaSetErrorHandler(void (*handler)(void *), void *handler_arg);
void dscudaGetMangledFunctionName(char *name, const char *funcif, const char *ptxdata);
int *dscudaLoadModule(char *srcname, char *strdata);
void dscudaLaunchKernelWrapper(int *moduleid, int kid, char *kname,
                               int *gdim, int *bdim, RCsize smemsize, RCstream stream,
                               int narg, RCArg *arg);

cudaError_t dscudaFuncGetAttributesWrapper(int *moduleid, struct cudaFuncAttributes *attr, const char *func);

cudaError_t dscudaMemcpyToSymbolWrapper(int *moduleid, const char *symbol, const void *src,
                                       size_t count, size_t offset = 0,
                                       enum cudaMemcpyKind kind = cudaMemcpyHostToDevice);

cudaError_t dscudaMemcpyToSymbolAsyncWrapper(int *moduleid, const char *symbol, const void *src,
					    size_t count, size_t offset = 0,
					    enum cudaMemcpyKind kind = cudaMemcpyHostToDevice, cudaStream_t stream = 0);

cudaError_t dscudaMemcpyFromSymbolWrapper(int *moduleid, void *dst, const char *symbol,
					 size_t count, size_t offset = 0,
					 enum cudaMemcpyKind kind = cudaMemcpyDeviceToHost);

cudaError_t dscudaMemcpyFromSymbolAsyncWrapper(int *moduleid, void *dst, const char *symbol,
					      size_t count, size_t offset = 0,
					      enum cudaMemcpyKind kind = cudaMemcpyDeviceToHost, cudaStream_t stream = 0);


cudaError_t dscudaBindTextureWrapper(int *moduleid, char *texname,
                                    size_t *offset,
                                    const struct textureReference *tex,
                                    const void *devPtr,
                                    const struct cudaChannelFormatDesc *desc,
                                    size_t size = UINT_MAX);
#if 0 
template<class T, int dim, enum cudaTextureReadMode readMode>
cudaError_t dscudaBindTextureWrapper(int *moduleid, char *texname,
                                    size_t *offset,
                                    const struct texture<T, dim, readMode> &tex,
                                    const void *devPtr,
                                    const struct cudaChannelFormatDesc &desc,
                                    size_t size = UINT_MAX)
{
    return dscudaBindTextureWrapper(moduleid, texname, offset, &tex, devPtr, &desc, size);
}

template<class T, int dim, enum cudaTextureReadMode readMode>
cudaError_t dscudaBindTextureWrapper(int *moduleid, char *texname,
                                    size_t *offset,
                                    const struct texture<T, dim, readMode> &tex,
                                    const void *devPtr,
                                    size_t size = UINT_MAX)
{
    return dscudaBindTextureWrapper(moduleid, texname, offset, &tex, devPtr, &tex.channelDesc, size);
}



cudaError_t dscudaBindTexture2DWrapper(int *moduleid, char *texname,
                                      size_t *offset,
                                      const struct textureReference *tex,
                                      const void *devPtr,
                                      const struct cudaChannelFormatDesc *desc,
                                      size_t width, size_t height, size_t pitch);

template<class T, int dim, enum cudaTextureReadMode readMode>
cudaError_t dscudaBindTexture2DWrapper(int *moduleid, char *texname,
                                      size_t *offset,
                                      const struct texture<T, dim, readMode> &tex,
                                      const void *devPtr,
                                      const struct cudaChannelFormatDesc &desc,
                                      size_t width, size_t height, size_t pitch)
{
    return dscudaBindTexture2DWrapper(moduleid, texname,
                                     offset, &tex, devPtr, &desc, width, height, pitch);
}

template<class T, int dim, enum cudaTextureReadMode readMode>
cudaError_t dscudaBindTexture2DWrapper(int *moduleid, char *texname,
                                      size_t *offset,
                                      const struct texture<T, dim, readMode> &tex,
                                      const void *devPtr,
                                      size_t width, size_t height, size_t pitch)
{
    return dscudaBindTexture2DWrapper(moduleid, texname,
                                     offset, &tex, devPtr, &tex.channelDesc, width, height, pitch);
}

cudaError_t dscudaBindTextureToArrayWrapper(int *moduleid, char *texname,
                                           const struct textureReference *tex,
                                           const struct cudaArray * array,
                                           const struct cudaChannelFormatDesc *desc);

template<class T, int dim, enum cudaTextureReadMode readMode>
cudaError_t dscudaBindTextureToArrayWrapper(int *moduleid, char *texname,
                                           const struct texture<T, dim, readMode> &tex,
                                           const struct cudaArray * array,
                                           const struct cudaChannelFormatDesc & desc)
{
    return dscudaBindTextureToArrayWrapper(moduleid, texname, &tex, array, &desc);
}

template<class T, int dim, enum cudaTextureReadMode readMode>
cudaError_t dscudaBindTextureToArrayWrapper(int *moduleid, char *texname,
                                           const struct texture<T, dim, readMode> &tex,
                                           const struct cudaArray * array)
{
    struct cudaChannelFormatDesc desc;
    cudaError_t err = cudaGetChannelDesc(&desc, array);
    return err == cudaSuccess ? dscudaBindTextureToArrayWrapper(moduleid, texname, &tex, array, &desc) : err;
}

#endif 

cudaError_t dscudaSortByKey(const int nitems, uint64_t *key, int *value);

#endif 
#pragma end dscuda.h
static char *Ptxdata = 
    "	.version 1.4\n"
    "	.target sm_10, map_f64_to_f32\n"
    "	// compiled with /usr/local/cuda-6.0/open64/lib//be\n"
    "	// nvopencc 4.1 built on 2014-03-13\n"
    "\n"
    "	//-----------------------------------------------------------\n"
    "	// Compiling /tmp/tmpxft_0000686b_00000000-9_mr3.cpp3.i (/tmp/ccBI#.UVHa1l)\n"
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
    "	.file	2	\"/tmp/tmpxft_0000686b_00000000-8_mr3.cudafe2.gpu\"\n"
    "	.file	3	\"mr3.cu\"\n"
    "	.file	4	\"/usr/lib/gcc/x86_64-linux-gnu/4.6/include/stddef.h\"\n"
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
    "	.const .align 4 .b8 c_matrix[96] = {0,0,160,63,143,194,21,64,138,228,73,64,209,59,149,61,138,32,14,61,0,0,128,63,0,0,128,63,236,81,48,64,138,228,73,64,174,184,248,62,50,87,26,63,0,0,128,191,0,0,128,63,236,81,48,64,138,228,73,64,174,184,248,62,50,87,26,63,0,0,128,191,0,0,64,63,72,225,74,64,138,228,73,64,176,0,161,64,89,178,33,65,0,0,128,63};\n"
    "\n"
    "	.entry nacl_kernel_if2 (\n"
    "		.param .u64 __cudaparm_nacl_kernel_if2_x,\n"
    "		.param .s32 __cudaparm_nacl_kernel_if2_n,\n"
    "		.param .s32 __cudaparm_nacl_kernel_if2_nat,\n"
    "		.param .f32 __cudaparm_nacl_kernel_if2_xmax,\n"
    "		.param .u64 __cudaparm_nacl_kernel_if2_fvec)\n"
    "	{\n"
    "	.reg .u16 %rh<3>;\n"
    "	.reg .u32 %r<99>;\n"
    "	.reg .u64 %rd<67>;\n"
    "	.reg .f32 %f<1124>;\n"
    "	.reg .pred %p<21>;\n"
    "	.shared .align 4 .b8 __cuda___cuda_local_var_16245_33_non_const_s_fi32[3072];\n"
    "	.shared .align 4 .b8 __cuda___cuda_local_var_16244_35_non_const_s_xj3104[4096];\n"
    "	.loc	3	94	0\n"
    "$LDWbegin_nacl_kernel_if2:\n"
    "	.loc	3	106	0\n"
    "	cvt.s32.u16 	%r1, %tid.x;\n"
    "	cvt.s64.s32 	%rd1, %r1;\n"
    "	mov.u64 	%rd2, __cuda___cuda_local_var_16245_33_non_const_s_fi32;\n"
    "	mul.wide.s32 	%rd3, %r1, 12;\n"
    "	add.u64 	%rd4, %rd2, %rd3;\n"
    "	mov.f32 	%f1, 0f00000000;     	// 0\n"
    "	st.shared.f32 	[%rd4+0], %f1;\n"
    "	mov.f32 	%f2, 0f00000000;     	// 0\n"
    "	st.shared.f32 	[%rd4+4], %f2;\n"
    "	mov.f32 	%f3, 0f00000000;     	// 0\n"
    "	st.shared.f32 	[%rd4+8], %f3;\n"
    "	.loc	3	107	0\n"
    "	and.b32 	%r2, %r1, 15;\n"
    "	mov.u16 	%rh1, %ctaid.x;\n"
    "	mul.wide.u16 	%r3, %rh1, 16;\n"
    "	add.u32 	%r4, %r2, %r3;\n"
    "	ld.param.u64 	%rd5, [__cudaparm_nacl_kernel_if2_x];\n"
    "	cvt.s64.s32 	%rd6, %r4;\n"
    "	mul.wide.s32 	%rd7, %r4, 16;\n"
    "	add.u64 	%rd8, %rd5, %rd7;\n"
    "	ld.global.f32 	%f4, [%rd8+0];\n"
    "	mov.f32 	%f5, %f4;\n"
    "	ld.global.f32 	%f6, [%rd8+4];\n"
    "	mov.f32 	%f7, %f6;\n"
    "	ld.global.f32 	%f8, [%rd8+8];\n"
    "	mov.f32 	%f9, %f8;\n"
    "	.loc	3	108	0\n"
    "	ld.global.s32 	%r5, [%rd8+12];\n"
    "	ld.param.s32 	%r6, [__cudaparm_nacl_kernel_if2_nat];\n"
    "	mul.lo.s32 	%r7, %r5, %r6;\n"
    "	ld.param.s32 	%r8, [__cudaparm_nacl_kernel_if2_n];\n"
    "	shr.s32 	%r9, %r8, 31;\n"
    "	mov.s32 	%r10, 255;\n"
    "	and.b32 	%r11, %r9, %r10;\n"
    "	add.s32 	%r12, %r11, %r8;\n"
    "	shr.s32 	%r13, %r12, 8;\n"
    "	shr.s32 	%r14, %r1, 31;\n"
    "	mov.s32 	%r15, 15;\n"
    "	and.b32 	%r16, %r14, %r15;\n"
    "	add.s32 	%r17, %r16, %r1;\n"
    "	shr.s32 	%r18, %r17, 4;\n"
    "	mul.lo.s32 	%r19, %r13, 256;\n"
    "	mov.u32 	%r20, 0;\n"
    "	setp.le.s32 	%p1, %r19, %r20;\n"
    "	@%p1 bra 	$Lt_0_23042;\n"
    "	mov.u64 	%rd9, __cuda___cuda_local_var_16244_35_non_const_s_xj3104;\n"
    "	ld.param.f32 	%f10, [__cudaparm_nacl_kernel_if2_xmax];\n"
    "	rcp.approx.f32 	%f11, %f10;\n"
    "	add.s32 	%r21, %r19, 255;\n"
    "	shr.s32 	%r22, %r21, 31;\n"
    "	mov.s32 	%r23, 255;\n"
    "	and.b32 	%r24, %r22, %r23;\n"
    "	add.s32 	%r25, %r24, %r21;\n"
    "	shr.s32 	%r26, %r25, 8;\n"
    "	mov.s32 	%r27, 271;\n"
    "	sub.s32 	%r28, %r27, %r18;\n"
    "	mul.lo.u64 	%rd10, %rd1, 16;\n"
    "	add.u64 	%rd11, %rd9, %rd10;\n"
    "	shr.s32 	%r29, %r28, 31;\n"
    "	mov.s32 	%r30, 15;\n"
    "	and.b32 	%r31, %r29, %r30;\n"
    "	add.s32 	%r32, %r31, %r28;\n"
    "	shr.s32 	%r33, %r32, 4;\n"
    "	shr.s32 	%r34, %r33, 31;\n"
    "	mov.s32 	%r35, 15;\n"
    "	and.b32 	%r36, %r34, %r35;\n"
    "	add.s32 	%r37, %r36, %r33;\n"
    "	shr.s32 	%r38, %r37, 4;\n"
    "	mul.lo.s32 	%r39, %r38, 256;\n"
    "	add.s32 	%r40, %r18, %r39;\n"
    "	setp.lt.s32 	%p2, %r18, %r40;\n"
    "	mov.s32 	%r41, 0;\n"
    "	mov.u64 	%rd12, c_matrix;\n"
    "	mov.s32 	%r42, %r26;\n"
    "$Lt_0_23554:\n"
    " //<loop> Loop body line 108, nesting depth: 1, estimated iterations: unknown\n"
    "	.loc	3	113	0\n"
    "	bar.sync 	0;\n"
    "	.loc	3	114	0\n"
    "	add.s32 	%r43, %r41, %r1;\n"
    "	cvt.s64.s32 	%rd13, %r43;\n"
    "	mul.wide.s32 	%rd14, %r43, 16;\n"
    "	add.u64 	%rd15, %rd5, %rd14;\n"
    "	ld.global.f32 	%f12, [%rd15+0];\n"
    "	st.shared.f32 	[%rd11+0], %f12;\n"
    "	ld.global.f32 	%f13, [%rd15+4];\n"
    "	st.shared.f32 	[%rd11+4], %f13;\n"
    "	ld.global.f32 	%f14, [%rd15+8];\n"
    "	st.shared.f32 	[%rd11+8], %f14;\n"
    "	ld.global.s32 	%r44, [%rd15+12];\n"
    "	st.shared.s32 	[%rd11+12], %r44;\n"
    "	.loc	3	115	0\n"
    "	bar.sync 	0;\n"
    "	.loc	3	24	0\n"
    "	mov.s32 	%r45, %r18;\n"
    "	@!%p2 bra 	$Lt_0_23810;\n"
    "	add.s32 	%r46, %r39, 255;\n"
    "	shr.s32 	%r47, %r46, 31;\n"
    "	mov.s32 	%r48, 255;\n"
    "	and.b32 	%r49, %r47, %r48;\n"
    "	add.s32 	%r50, %r49, %r46;\n"
    "	shr.s32 	%r51, %r50, 8;\n"
    "	cvt.s64.s32 	%rd16, %r18;\n"
    "	mul.wide.s32 	%rd17, %r18, 16;\n"
    "	ld.shared.f32 	%f15, [%rd4+0];\n"
    "	ld.shared.f32 	%f16, [%rd4+4];\n"
    "	ld.shared.f32 	%f17, [%rd4+8];\n"
    "	mov.s32 	%r52, %r51;\n"
    "$Lt_0_24322:\n"
    " //<loop> Loop body line 24, nesting depth: 2, estimated iterations: unknown\n"
    "	.loc	3	118	0\n"
    "	add.u64 	%rd18, %rd17, %rd9;\n"
    "	ld.shared.s32 	%r53, [%rd18+12];\n"
    "	add.s32 	%r54, %r53, %r7;\n"
    "	.loc	3	58	0\n"
    "	add.u64 	%rd19, %rd17, %rd9;\n"
    "	ld.shared.f32 	%f18, [%rd19+0];\n"
    "	mov.f32 	%f19, %f5;\n"
    "	sub.f32 	%f20, %f19, %f18;\n"
    "	mov.f32 	%f21, %f20;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f22, %f11, %f21;\n"
    "	cvt.rni.f32.f32 	%f23, %f22;\n"
    "	mul.f32 	%f24, %f10, %f23;\n"
    "	sub.f32 	%f25, %f20, %f24;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f27, 0f00000000;    	// 0\n"
    "	mad.f32 	%f28, %f25, %f25, %f27;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f29, [%rd19+4];\n"
    "	mov.f32 	%f30, %f7;\n"
    "	sub.f32 	%f31, %f30, %f29;\n"
    "	mov.f32 	%f32, %f31;\n"
    "	mov.f32 	%f33, %f32;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f34, %f11, %f32;\n"
    "	cvt.rni.f32.f32 	%f35, %f34;\n"
    "	mul.f32 	%f36, %f10, %f35;\n"
    "	sub.f32 	%f37, %f31, %f36;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f39, %f37, %f37, %f28;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f40, [%rd19+8];\n"
    "	mov.f32 	%f41, %f9;\n"
    "	sub.f32 	%f42, %f41, %f40;\n"
    "	mov.f32 	%f43, %f42;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f44, %f42, %f11;\n"
    "	cvt.rni.f32.f32 	%f45, %f44;\n"
    "	mul.f32 	%f46, %f10, %f45;\n"
    "	sub.f32 	%f47, %f42, %f46;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f39;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd20, %r54;\n"
    "	mul.wide.s32 	%rd21, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd21;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f55, 0f3fb8aa3b;    	// 1.4427\n"
    "	mul.f32 	%f56, %f54, %f55;\n"
    "	cvt.rzi.f32.f32 	%f57, %f56;\n"
    "	mov.f32 	%f58, %f57;\n"
    "	mov.f32 	%f59, 0fbf317200;    	// -0.693146\n"
    "	mov.f32 	%f60, %f59;\n"
    "	mov.f32 	%f61, %f54;\n"
    "	mad.f32 %f62, %f58, %f60, %f61;\n"
    "	mov.f32 	%f63, %f62;\n"
    "	mov.f32 	%f64, %f57;\n"
    "	mov.f32 	%f65, 0fb5bfbe8e;    	// -1.42861e-06\n"
    "	mov.f32 	%f66, %f65;\n"
    "	mov.f32 	%f67, %f63;\n"
    "	mad.f32 %f68, %f64, %f66, %f67;\n"
    "	mov.f32 	%f63, %f68;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f69, 0f3fb8aa3b;    	// 1.4427\n"
    "	mul.f32 	%f70, %f63, %f69;\n"
    "	mov.f32 	%f71, %f70;\n"
    "	ex2.approx.ftz.f32 %f72,%f71;\n"
    "	mov.f32 	%f73, %f72;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f74, 0f00000000;    	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f74;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f83, 0f40c00000;    	// 6\n"
    "	mul.f32 	%f84, %f77, %f83;\n"
    "	mov.f32 	%f85, 0f3c70319e;    	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f85;\n"
    "	mov.f32 	%f87, 0f41000000;    	// 8\n"
    "	mul.f32 	%f88, %f79, %f87;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f92, 0f42d20000;    	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f92;\n"
    "	mov.f32 	%f93, 0fc2d20000;    	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f93;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f96, 0f00000000;    	// 0\n"
    "	selp.f32 	%f97, %f96, %f95, %p5;\n"
    "	mov.f32 	%f98, 0f7f800000;    	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f98, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f102, %f75, %f101;\n"
    "	sub.f32 	%f103, %f102, %f90;\n"
    "	mul.f32 	%f104, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f104;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f107, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f107, %f106, %p3;\n"
    "	mov.f32 	%f109, %f26;\n"
    "	mad.f32 	%f15, %f109, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f110, %f38;\n"
    "	mad.f32 	%f16, %f110, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f111, %f48;\n"
    "	mad.f32 	%f17, %f111, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r55, [%rd18+268];\n"
    "	add.s32 	%r54, %r55, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f112, [%rd19+256];\n"
    "	mov.f32 	%f113, %f5;\n"
    "	sub.f32 	%f114, %f113, %f112;\n"
    "	mov.f32 	%f115, %f114;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f116, %f11, %f115;\n"
    "	cvt.rni.f32.f32 	%f117, %f116;\n"
    "	mul.f32 	%f118, %f10, %f117;\n"
    "	sub.f32 	%f25, %f114, %f118;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f119, 0f00000000;   	// 0\n"
    "	mad.f32 	%f120, %f25, %f25, %f119;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f121, [%rd19+260];\n"
    "	mov.f32 	%f122, %f7;\n"
    "	sub.f32 	%f123, %f122, %f121;\n"
    "	mov.f32 	%f124, %f123;\n"
    "	mov.f32 	%f125, %f124;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f126, %f11, %f124;\n"
    "	cvt.rni.f32.f32 	%f127, %f126;\n"
    "	mul.f32 	%f128, %f10, %f127;\n"
    "	sub.f32 	%f37, %f123, %f128;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f129, %f37, %f37, %f120;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f130, [%rd19+264];\n"
    "	mov.f32 	%f131, %f9;\n"
    "	sub.f32 	%f132, %f131, %f130;\n"
    "	mov.f32 	%f133, %f132;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f134, %f132, %f11;\n"
    "	cvt.rni.f32.f32 	%f135, %f134;\n"
    "	mul.f32 	%f136, %f10, %f135;\n"
    "	sub.f32 	%f47, %f132, %f136;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f129;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd23, %r54;\n"
    "	mul.wide.s32 	%rd24, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd24;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f137, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f138, %f54, %f137;\n"
    "	cvt.rzi.f32.f32 	%f57, %f138;\n"
    "	mov.f32 	%f139, %f57;\n"
    "	mov.f32 	%f140, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f141, %f140;\n"
    "	mov.f32 	%f142, %f54;\n"
    "	mad.f32 %f143, %f139, %f141, %f142;\n"
    "	mov.f32 	%f63, %f143;\n"
    "	mov.f32 	%f144, %f57;\n"
    "	mov.f32 	%f145, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f146, %f145;\n"
    "	mov.f32 	%f147, %f63;\n"
    "	mad.f32 %f148, %f144, %f146, %f147;\n"
    "	mov.f32 	%f63, %f148;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f149, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f150, %f63, %f149;\n"
    "	mov.f32 	%f151, %f150;\n"
    "	ex2.approx.ftz.f32 %f152,%f151;\n"
    "	mov.f32 	%f73, %f152;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f153, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f153;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f154, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f154;\n"
    "	mov.f32 	%f155, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f155;\n"
    "	mov.f32 	%f156, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f156;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f157, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f157;\n"
    "	mov.f32 	%f158, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f158;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f159, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f159, %f95, %p5;\n"
    "	mov.f32 	%f160, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f160, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f161, %f75, %f101;\n"
    "	sub.f32 	%f103, %f161, %f90;\n"
    "	mul.f32 	%f162, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f162;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f163, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f163, %f106, %p3;\n"
    "	mov.f32 	%f164, %f26;\n"
    "	mad.f32 	%f15, %f164, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f165, %f38;\n"
    "	mad.f32 	%f16, %f165, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f166, %f48;\n"
    "	mad.f32 	%f17, %f166, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r56, [%rd18+524];\n"
    "	add.s32 	%r54, %r56, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f167, [%rd19+512];\n"
    "	mov.f32 	%f168, %f5;\n"
    "	sub.f32 	%f169, %f168, %f167;\n"
    "	mov.f32 	%f170, %f169;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f171, %f11, %f170;\n"
    "	cvt.rni.f32.f32 	%f172, %f171;\n"
    "	mul.f32 	%f173, %f10, %f172;\n"
    "	sub.f32 	%f25, %f169, %f173;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f174, 0f00000000;   	// 0\n"
    "	mad.f32 	%f175, %f25, %f25, %f174;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f176, [%rd19+516];\n"
    "	mov.f32 	%f177, %f7;\n"
    "	sub.f32 	%f178, %f177, %f176;\n"
    "	mov.f32 	%f179, %f178;\n"
    "	mov.f32 	%f180, %f179;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f181, %f11, %f179;\n"
    "	cvt.rni.f32.f32 	%f182, %f181;\n"
    "	mul.f32 	%f183, %f10, %f182;\n"
    "	sub.f32 	%f37, %f178, %f183;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f184, %f37, %f37, %f175;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f185, [%rd19+520];\n"
    "	mov.f32 	%f186, %f9;\n"
    "	sub.f32 	%f187, %f186, %f185;\n"
    "	mov.f32 	%f188, %f187;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f189, %f187, %f11;\n"
    "	cvt.rni.f32.f32 	%f190, %f189;\n"
    "	mul.f32 	%f191, %f10, %f190;\n"
    "	sub.f32 	%f47, %f187, %f191;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f184;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd25, %r54;\n"
    "	mul.wide.s32 	%rd26, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd26;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f192, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f193, %f54, %f192;\n"
    "	cvt.rzi.f32.f32 	%f57, %f193;\n"
    "	mov.f32 	%f194, %f57;\n"
    "	mov.f32 	%f195, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f196, %f195;\n"
    "	mov.f32 	%f197, %f54;\n"
    "	mad.f32 %f198, %f194, %f196, %f197;\n"
    "	mov.f32 	%f63, %f198;\n"
    "	mov.f32 	%f199, %f57;\n"
    "	mov.f32 	%f200, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f201, %f200;\n"
    "	mov.f32 	%f202, %f63;\n"
    "	mad.f32 %f203, %f199, %f201, %f202;\n"
    "	mov.f32 	%f63, %f203;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f204, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f205, %f63, %f204;\n"
    "	mov.f32 	%f206, %f205;\n"
    "	ex2.approx.ftz.f32 %f207,%f206;\n"
    "	mov.f32 	%f73, %f207;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f208, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f208;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f209, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f209;\n"
    "	mov.f32 	%f210, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f210;\n"
    "	mov.f32 	%f211, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f211;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f212, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f212;\n"
    "	mov.f32 	%f213, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f213;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f214, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f214, %f95, %p5;\n"
    "	mov.f32 	%f215, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f215, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f216, %f75, %f101;\n"
    "	sub.f32 	%f103, %f216, %f90;\n"
    "	mul.f32 	%f217, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f217;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f218, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f218, %f106, %p3;\n"
    "	mov.f32 	%f219, %f26;\n"
    "	mad.f32 	%f15, %f219, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f220, %f38;\n"
    "	mad.f32 	%f16, %f220, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f221, %f48;\n"
    "	mad.f32 	%f17, %f221, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r57, [%rd18+780];\n"
    "	add.s32 	%r54, %r57, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f222, [%rd19+768];\n"
    "	mov.f32 	%f223, %f5;\n"
    "	sub.f32 	%f224, %f223, %f222;\n"
    "	mov.f32 	%f225, %f224;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f226, %f11, %f225;\n"
    "	cvt.rni.f32.f32 	%f227, %f226;\n"
    "	mul.f32 	%f228, %f10, %f227;\n"
    "	sub.f32 	%f25, %f224, %f228;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f229, 0f00000000;   	// 0\n"
    "	mad.f32 	%f230, %f25, %f25, %f229;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f231, [%rd19+772];\n"
    "	mov.f32 	%f232, %f7;\n"
    "	sub.f32 	%f233, %f232, %f231;\n"
    "	mov.f32 	%f234, %f233;\n"
    "	mov.f32 	%f235, %f234;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f236, %f11, %f234;\n"
    "	cvt.rni.f32.f32 	%f237, %f236;\n"
    "	mul.f32 	%f238, %f10, %f237;\n"
    "	sub.f32 	%f37, %f233, %f238;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f239, %f37, %f37, %f230;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f240, [%rd19+776];\n"
    "	mov.f32 	%f241, %f9;\n"
    "	sub.f32 	%f242, %f241, %f240;\n"
    "	mov.f32 	%f243, %f242;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f244, %f242, %f11;\n"
    "	cvt.rni.f32.f32 	%f245, %f244;\n"
    "	mul.f32 	%f246, %f10, %f245;\n"
    "	sub.f32 	%f47, %f242, %f246;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f239;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd27, %r54;\n"
    "	mul.wide.s32 	%rd28, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd28;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f247, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f248, %f54, %f247;\n"
    "	cvt.rzi.f32.f32 	%f57, %f248;\n"
    "	mov.f32 	%f249, %f57;\n"
    "	mov.f32 	%f250, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f251, %f250;\n"
    "	mov.f32 	%f252, %f54;\n"
    "	mad.f32 %f253, %f249, %f251, %f252;\n"
    "	mov.f32 	%f63, %f253;\n"
    "	mov.f32 	%f254, %f57;\n"
    "	mov.f32 	%f255, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f256, %f255;\n"
    "	mov.f32 	%f257, %f63;\n"
    "	mad.f32 %f258, %f254, %f256, %f257;\n"
    "	mov.f32 	%f63, %f258;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f259, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f260, %f63, %f259;\n"
    "	mov.f32 	%f261, %f260;\n"
    "	ex2.approx.ftz.f32 %f262,%f261;\n"
    "	mov.f32 	%f73, %f262;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f263, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f263;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f264, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f264;\n"
    "	mov.f32 	%f265, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f265;\n"
    "	mov.f32 	%f266, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f266;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f267, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f267;\n"
    "	mov.f32 	%f268, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f268;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f269, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f269, %f95, %p5;\n"
    "	mov.f32 	%f270, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f270, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f271, %f75, %f101;\n"
    "	sub.f32 	%f103, %f271, %f90;\n"
    "	mul.f32 	%f272, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f272;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f273, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f273, %f106, %p3;\n"
    "	mov.f32 	%f274, %f26;\n"
    "	mad.f32 	%f15, %f274, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f275, %f38;\n"
    "	mad.f32 	%f16, %f275, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f276, %f48;\n"
    "	mad.f32 	%f17, %f276, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r58, [%rd18+1036];\n"
    "	add.s32 	%r54, %r58, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f277, [%rd19+1024];\n"
    "	mov.f32 	%f278, %f5;\n"
    "	sub.f32 	%f279, %f278, %f277;\n"
    "	mov.f32 	%f280, %f279;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f281, %f11, %f280;\n"
    "	cvt.rni.f32.f32 	%f282, %f281;\n"
    "	mul.f32 	%f283, %f10, %f282;\n"
    "	sub.f32 	%f25, %f279, %f283;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f284, 0f00000000;   	// 0\n"
    "	mad.f32 	%f285, %f25, %f25, %f284;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f286, [%rd19+1028];\n"
    "	mov.f32 	%f287, %f7;\n"
    "	sub.f32 	%f288, %f287, %f286;\n"
    "	mov.f32 	%f289, %f288;\n"
    "	mov.f32 	%f290, %f289;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f291, %f11, %f289;\n"
    "	cvt.rni.f32.f32 	%f292, %f291;\n"
    "	mul.f32 	%f293, %f10, %f292;\n"
    "	sub.f32 	%f37, %f288, %f293;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f294, %f37, %f37, %f285;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f295, [%rd19+1032];\n"
    "	mov.f32 	%f296, %f9;\n"
    "	sub.f32 	%f297, %f296, %f295;\n"
    "	mov.f32 	%f298, %f297;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f299, %f297, %f11;\n"
    "	cvt.rni.f32.f32 	%f300, %f299;\n"
    "	mul.f32 	%f301, %f10, %f300;\n"
    "	sub.f32 	%f47, %f297, %f301;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f294;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd29, %r54;\n"
    "	mul.wide.s32 	%rd30, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd30;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f302, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f303, %f54, %f302;\n"
    "	cvt.rzi.f32.f32 	%f57, %f303;\n"
    "	mov.f32 	%f304, %f57;\n"
    "	mov.f32 	%f305, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f306, %f305;\n"
    "	mov.f32 	%f307, %f54;\n"
    "	mad.f32 %f308, %f304, %f306, %f307;\n"
    "	mov.f32 	%f63, %f308;\n"
    "	mov.f32 	%f309, %f57;\n"
    "	mov.f32 	%f310, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f311, %f310;\n"
    "	mov.f32 	%f312, %f63;\n"
    "	mad.f32 %f313, %f309, %f311, %f312;\n"
    "	mov.f32 	%f63, %f313;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f314, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f315, %f63, %f314;\n"
    "	mov.f32 	%f316, %f315;\n"
    "	ex2.approx.ftz.f32 %f317,%f316;\n"
    "	mov.f32 	%f73, %f317;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f318, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f318;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f319, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f319;\n"
    "	mov.f32 	%f320, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f320;\n"
    "	mov.f32 	%f321, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f321;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f322, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f322;\n"
    "	mov.f32 	%f323, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f323;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f324, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f324, %f95, %p5;\n"
    "	mov.f32 	%f325, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f325, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f326, %f75, %f101;\n"
    "	sub.f32 	%f103, %f326, %f90;\n"
    "	mul.f32 	%f327, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f327;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f328, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f328, %f106, %p3;\n"
    "	mov.f32 	%f329, %f26;\n"
    "	mad.f32 	%f15, %f329, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f330, %f38;\n"
    "	mad.f32 	%f16, %f330, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f331, %f48;\n"
    "	mad.f32 	%f17, %f331, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r59, [%rd18+1292];\n"
    "	add.s32 	%r54, %r59, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f332, [%rd19+1280];\n"
    "	mov.f32 	%f333, %f5;\n"
    "	sub.f32 	%f334, %f333, %f332;\n"
    "	mov.f32 	%f335, %f334;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f336, %f11, %f335;\n"
    "	cvt.rni.f32.f32 	%f337, %f336;\n"
    "	mul.f32 	%f338, %f10, %f337;\n"
    "	sub.f32 	%f25, %f334, %f338;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f339, 0f00000000;   	// 0\n"
    "	mad.f32 	%f340, %f25, %f25, %f339;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f341, [%rd19+1284];\n"
    "	mov.f32 	%f342, %f7;\n"
    "	sub.f32 	%f343, %f342, %f341;\n"
    "	mov.f32 	%f344, %f343;\n"
    "	mov.f32 	%f345, %f344;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f346, %f11, %f344;\n"
    "	cvt.rni.f32.f32 	%f347, %f346;\n"
    "	mul.f32 	%f348, %f10, %f347;\n"
    "	sub.f32 	%f37, %f343, %f348;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f349, %f37, %f37, %f340;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f350, [%rd19+1288];\n"
    "	mov.f32 	%f351, %f9;\n"
    "	sub.f32 	%f352, %f351, %f350;\n"
    "	mov.f32 	%f353, %f352;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f354, %f352, %f11;\n"
    "	cvt.rni.f32.f32 	%f355, %f354;\n"
    "	mul.f32 	%f356, %f10, %f355;\n"
    "	sub.f32 	%f47, %f352, %f356;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f349;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd31, %r54;\n"
    "	mul.wide.s32 	%rd32, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd32;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f357, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f358, %f54, %f357;\n"
    "	cvt.rzi.f32.f32 	%f57, %f358;\n"
    "	mov.f32 	%f359, %f57;\n"
    "	mov.f32 	%f360, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f361, %f360;\n"
    "	mov.f32 	%f362, %f54;\n"
    "	mad.f32 %f363, %f359, %f361, %f362;\n"
    "	mov.f32 	%f63, %f363;\n"
    "	mov.f32 	%f364, %f57;\n"
    "	mov.f32 	%f365, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f366, %f365;\n"
    "	mov.f32 	%f367, %f63;\n"
    "	mad.f32 %f368, %f364, %f366, %f367;\n"
    "	mov.f32 	%f63, %f368;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f369, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f370, %f63, %f369;\n"
    "	mov.f32 	%f371, %f370;\n"
    "	ex2.approx.ftz.f32 %f372,%f371;\n"
    "	mov.f32 	%f73, %f372;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f373, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f373;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f374, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f374;\n"
    "	mov.f32 	%f375, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f375;\n"
    "	mov.f32 	%f376, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f376;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f377, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f377;\n"
    "	mov.f32 	%f378, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f378;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f379, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f379, %f95, %p5;\n"
    "	mov.f32 	%f380, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f380, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f381, %f75, %f101;\n"
    "	sub.f32 	%f103, %f381, %f90;\n"
    "	mul.f32 	%f382, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f382;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f383, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f383, %f106, %p3;\n"
    "	mov.f32 	%f384, %f26;\n"
    "	mad.f32 	%f15, %f384, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f385, %f38;\n"
    "	mad.f32 	%f16, %f385, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f386, %f48;\n"
    "	mad.f32 	%f17, %f386, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r60, [%rd18+1548];\n"
    "	add.s32 	%r54, %r60, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f387, [%rd19+1536];\n"
    "	mov.f32 	%f388, %f5;\n"
    "	sub.f32 	%f389, %f388, %f387;\n"
    "	mov.f32 	%f390, %f389;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f391, %f11, %f390;\n"
    "	cvt.rni.f32.f32 	%f392, %f391;\n"
    "	mul.f32 	%f393, %f10, %f392;\n"
    "	sub.f32 	%f25, %f389, %f393;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f394, 0f00000000;   	// 0\n"
    "	mad.f32 	%f395, %f25, %f25, %f394;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f396, [%rd19+1540];\n"
    "	mov.f32 	%f397, %f7;\n"
    "	sub.f32 	%f398, %f397, %f396;\n"
    "	mov.f32 	%f399, %f398;\n"
    "	mov.f32 	%f400, %f399;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f401, %f11, %f399;\n"
    "	cvt.rni.f32.f32 	%f402, %f401;\n"
    "	mul.f32 	%f403, %f10, %f402;\n"
    "	sub.f32 	%f37, %f398, %f403;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f404, %f37, %f37, %f395;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f405, [%rd19+1544];\n"
    "	mov.f32 	%f406, %f9;\n"
    "	sub.f32 	%f407, %f406, %f405;\n"
    "	mov.f32 	%f408, %f407;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f409, %f407, %f11;\n"
    "	cvt.rni.f32.f32 	%f410, %f409;\n"
    "	mul.f32 	%f411, %f10, %f410;\n"
    "	sub.f32 	%f47, %f407, %f411;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f404;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd33, %r54;\n"
    "	mul.wide.s32 	%rd34, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd34;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f412, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f413, %f54, %f412;\n"
    "	cvt.rzi.f32.f32 	%f57, %f413;\n"
    "	mov.f32 	%f414, %f57;\n"
    "	mov.f32 	%f415, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f416, %f415;\n"
    "	mov.f32 	%f417, %f54;\n"
    "	mad.f32 %f418, %f414, %f416, %f417;\n"
    "	mov.f32 	%f63, %f418;\n"
    "	mov.f32 	%f419, %f57;\n"
    "	mov.f32 	%f420, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f421, %f420;\n"
    "	mov.f32 	%f422, %f63;\n"
    "	mad.f32 %f423, %f419, %f421, %f422;\n"
    "	mov.f32 	%f63, %f423;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f424, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f425, %f63, %f424;\n"
    "	mov.f32 	%f426, %f425;\n"
    "	ex2.approx.ftz.f32 %f427,%f426;\n"
    "	mov.f32 	%f73, %f427;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f428, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f428;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f429, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f429;\n"
    "	mov.f32 	%f430, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f430;\n"
    "	mov.f32 	%f431, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f431;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f432, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f432;\n"
    "	mov.f32 	%f433, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f433;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f434, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f434, %f95, %p5;\n"
    "	mov.f32 	%f435, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f435, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f436, %f75, %f101;\n"
    "	sub.f32 	%f103, %f436, %f90;\n"
    "	mul.f32 	%f437, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f437;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f438, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f438, %f106, %p3;\n"
    "	mov.f32 	%f439, %f26;\n"
    "	mad.f32 	%f15, %f439, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f440, %f38;\n"
    "	mad.f32 	%f16, %f440, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f441, %f48;\n"
    "	mad.f32 	%f17, %f441, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r61, [%rd18+1804];\n"
    "	add.s32 	%r54, %r61, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f442, [%rd19+1792];\n"
    "	mov.f32 	%f443, %f5;\n"
    "	sub.f32 	%f444, %f443, %f442;\n"
    "	mov.f32 	%f445, %f444;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f446, %f11, %f445;\n"
    "	cvt.rni.f32.f32 	%f447, %f446;\n"
    "	mul.f32 	%f448, %f10, %f447;\n"
    "	sub.f32 	%f25, %f444, %f448;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f449, 0f00000000;   	// 0\n"
    "	mad.f32 	%f450, %f25, %f25, %f449;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f451, [%rd19+1796];\n"
    "	mov.f32 	%f452, %f7;\n"
    "	sub.f32 	%f453, %f452, %f451;\n"
    "	mov.f32 	%f454, %f453;\n"
    "	mov.f32 	%f455, %f454;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f456, %f11, %f454;\n"
    "	cvt.rni.f32.f32 	%f457, %f456;\n"
    "	mul.f32 	%f458, %f10, %f457;\n"
    "	sub.f32 	%f37, %f453, %f458;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f459, %f37, %f37, %f450;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f460, [%rd19+1800];\n"
    "	mov.f32 	%f461, %f9;\n"
    "	sub.f32 	%f462, %f461, %f460;\n"
    "	mov.f32 	%f463, %f462;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f464, %f462, %f11;\n"
    "	cvt.rni.f32.f32 	%f465, %f464;\n"
    "	mul.f32 	%f466, %f10, %f465;\n"
    "	sub.f32 	%f47, %f462, %f466;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f459;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd35, %r54;\n"
    "	mul.wide.s32 	%rd36, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd36;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f467, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f468, %f54, %f467;\n"
    "	cvt.rzi.f32.f32 	%f57, %f468;\n"
    "	mov.f32 	%f469, %f57;\n"
    "	mov.f32 	%f470, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f471, %f470;\n"
    "	mov.f32 	%f472, %f54;\n"
    "	mad.f32 %f473, %f469, %f471, %f472;\n"
    "	mov.f32 	%f63, %f473;\n"
    "	mov.f32 	%f474, %f57;\n"
    "	mov.f32 	%f475, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f476, %f475;\n"
    "	mov.f32 	%f477, %f63;\n"
    "	mad.f32 %f478, %f474, %f476, %f477;\n"
    "	mov.f32 	%f63, %f478;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f479, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f480, %f63, %f479;\n"
    "	mov.f32 	%f481, %f480;\n"
    "	ex2.approx.ftz.f32 %f482,%f481;\n"
    "	mov.f32 	%f73, %f482;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f483, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f483;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f484, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f484;\n"
    "	mov.f32 	%f485, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f485;\n"
    "	mov.f32 	%f486, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f486;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f487, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f487;\n"
    "	mov.f32 	%f488, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f488;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f489, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f489, %f95, %p5;\n"
    "	mov.f32 	%f490, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f490, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f491, %f75, %f101;\n"
    "	sub.f32 	%f103, %f491, %f90;\n"
    "	mul.f32 	%f492, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f492;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f493, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f493, %f106, %p3;\n"
    "	mov.f32 	%f494, %f26;\n"
    "	mad.f32 	%f15, %f494, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f495, %f38;\n"
    "	mad.f32 	%f16, %f495, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f496, %f48;\n"
    "	mad.f32 	%f17, %f496, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r62, [%rd18+2060];\n"
    "	add.s32 	%r54, %r62, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f497, [%rd19+2048];\n"
    "	mov.f32 	%f498, %f5;\n"
    "	sub.f32 	%f499, %f498, %f497;\n"
    "	mov.f32 	%f500, %f499;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f501, %f11, %f500;\n"
    "	cvt.rni.f32.f32 	%f502, %f501;\n"
    "	mul.f32 	%f503, %f10, %f502;\n"
    "	sub.f32 	%f25, %f499, %f503;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f504, 0f00000000;   	// 0\n"
    "	mad.f32 	%f505, %f25, %f25, %f504;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f506, [%rd19+2052];\n"
    "	mov.f32 	%f507, %f7;\n"
    "	sub.f32 	%f508, %f507, %f506;\n"
    "	mov.f32 	%f509, %f508;\n"
    "	mov.f32 	%f510, %f509;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f511, %f11, %f509;\n"
    "	cvt.rni.f32.f32 	%f512, %f511;\n"
    "	mul.f32 	%f513, %f10, %f512;\n"
    "	sub.f32 	%f37, %f508, %f513;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f514, %f37, %f37, %f505;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f515, [%rd19+2056];\n"
    "	mov.f32 	%f516, %f9;\n"
    "	sub.f32 	%f517, %f516, %f515;\n"
    "	mov.f32 	%f518, %f517;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f519, %f517, %f11;\n"
    "	cvt.rni.f32.f32 	%f520, %f519;\n"
    "	mul.f32 	%f521, %f10, %f520;\n"
    "	sub.f32 	%f47, %f517, %f521;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f514;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd37, %r54;\n"
    "	mul.wide.s32 	%rd38, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd38;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f522, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f523, %f54, %f522;\n"
    "	cvt.rzi.f32.f32 	%f57, %f523;\n"
    "	mov.f32 	%f524, %f57;\n"
    "	mov.f32 	%f525, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f526, %f525;\n"
    "	mov.f32 	%f527, %f54;\n"
    "	mad.f32 %f528, %f524, %f526, %f527;\n"
    "	mov.f32 	%f63, %f528;\n"
    "	mov.f32 	%f529, %f57;\n"
    "	mov.f32 	%f530, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f531, %f530;\n"
    "	mov.f32 	%f532, %f63;\n"
    "	mad.f32 %f533, %f529, %f531, %f532;\n"
    "	mov.f32 	%f63, %f533;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f534, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f535, %f63, %f534;\n"
    "	mov.f32 	%f536, %f535;\n"
    "	ex2.approx.ftz.f32 %f537,%f536;\n"
    "	mov.f32 	%f73, %f537;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f538, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f538;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f539, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f539;\n"
    "	mov.f32 	%f540, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f540;\n"
    "	mov.f32 	%f541, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f541;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f542, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f542;\n"
    "	mov.f32 	%f543, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f543;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f544, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f544, %f95, %p5;\n"
    "	mov.f32 	%f545, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f545, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f546, %f75, %f101;\n"
    "	sub.f32 	%f103, %f546, %f90;\n"
    "	mul.f32 	%f547, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f547;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f548, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f548, %f106, %p3;\n"
    "	mov.f32 	%f549, %f26;\n"
    "	mad.f32 	%f15, %f549, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f550, %f38;\n"
    "	mad.f32 	%f16, %f550, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f551, %f48;\n"
    "	mad.f32 	%f17, %f551, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r63, [%rd18+2316];\n"
    "	add.s32 	%r54, %r63, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f552, [%rd19+2304];\n"
    "	mov.f32 	%f553, %f5;\n"
    "	sub.f32 	%f554, %f553, %f552;\n"
    "	mov.f32 	%f555, %f554;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f556, %f11, %f555;\n"
    "	cvt.rni.f32.f32 	%f557, %f556;\n"
    "	mul.f32 	%f558, %f10, %f557;\n"
    "	sub.f32 	%f25, %f554, %f558;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f559, 0f00000000;   	// 0\n"
    "	mad.f32 	%f560, %f25, %f25, %f559;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f561, [%rd19+2308];\n"
    "	mov.f32 	%f562, %f7;\n"
    "	sub.f32 	%f563, %f562, %f561;\n"
    "	mov.f32 	%f564, %f563;\n"
    "	mov.f32 	%f565, %f564;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f566, %f11, %f564;\n"
    "	cvt.rni.f32.f32 	%f567, %f566;\n"
    "	mul.f32 	%f568, %f10, %f567;\n"
    "	sub.f32 	%f37, %f563, %f568;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f569, %f37, %f37, %f560;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f570, [%rd19+2312];\n"
    "	mov.f32 	%f571, %f9;\n"
    "	sub.f32 	%f572, %f571, %f570;\n"
    "	mov.f32 	%f573, %f572;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f574, %f572, %f11;\n"
    "	cvt.rni.f32.f32 	%f575, %f574;\n"
    "	mul.f32 	%f576, %f10, %f575;\n"
    "	sub.f32 	%f47, %f572, %f576;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f569;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd39, %r54;\n"
    "	mul.wide.s32 	%rd40, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd40;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f577, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f578, %f54, %f577;\n"
    "	cvt.rzi.f32.f32 	%f57, %f578;\n"
    "	mov.f32 	%f579, %f57;\n"
    "	mov.f32 	%f580, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f581, %f580;\n"
    "	mov.f32 	%f582, %f54;\n"
    "	mad.f32 %f583, %f579, %f581, %f582;\n"
    "	mov.f32 	%f63, %f583;\n"
    "	mov.f32 	%f584, %f57;\n"
    "	mov.f32 	%f585, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f586, %f585;\n"
    "	mov.f32 	%f587, %f63;\n"
    "	mad.f32 %f588, %f584, %f586, %f587;\n"
    "	mov.f32 	%f63, %f588;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f589, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f590, %f63, %f589;\n"
    "	mov.f32 	%f591, %f590;\n"
    "	ex2.approx.ftz.f32 %f592,%f591;\n"
    "	mov.f32 	%f73, %f592;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f593, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f593;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f594, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f594;\n"
    "	mov.f32 	%f595, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f595;\n"
    "	mov.f32 	%f596, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f596;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f597, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f597;\n"
    "	mov.f32 	%f598, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f598;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f599, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f599, %f95, %p5;\n"
    "	mov.f32 	%f600, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f600, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f601, %f75, %f101;\n"
    "	sub.f32 	%f103, %f601, %f90;\n"
    "	mul.f32 	%f602, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f602;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f603, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f603, %f106, %p3;\n"
    "	mov.f32 	%f604, %f26;\n"
    "	mad.f32 	%f15, %f604, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f605, %f38;\n"
    "	mad.f32 	%f16, %f605, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f606, %f48;\n"
    "	mad.f32 	%f17, %f606, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r64, [%rd18+2572];\n"
    "	add.s32 	%r54, %r64, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f607, [%rd19+2560];\n"
    "	mov.f32 	%f608, %f5;\n"
    "	sub.f32 	%f609, %f608, %f607;\n"
    "	mov.f32 	%f610, %f609;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f611, %f11, %f610;\n"
    "	cvt.rni.f32.f32 	%f612, %f611;\n"
    "	mul.f32 	%f613, %f10, %f612;\n"
    "	sub.f32 	%f25, %f609, %f613;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f614, 0f00000000;   	// 0\n"
    "	mad.f32 	%f615, %f25, %f25, %f614;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f616, [%rd19+2564];\n"
    "	mov.f32 	%f617, %f7;\n"
    "	sub.f32 	%f618, %f617, %f616;\n"
    "	mov.f32 	%f619, %f618;\n"
    "	mov.f32 	%f620, %f619;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f621, %f11, %f619;\n"
    "	cvt.rni.f32.f32 	%f622, %f621;\n"
    "	mul.f32 	%f623, %f10, %f622;\n"
    "	sub.f32 	%f37, %f618, %f623;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f624, %f37, %f37, %f615;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f625, [%rd19+2568];\n"
    "	mov.f32 	%f626, %f9;\n"
    "	sub.f32 	%f627, %f626, %f625;\n"
    "	mov.f32 	%f628, %f627;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f629, %f627, %f11;\n"
    "	cvt.rni.f32.f32 	%f630, %f629;\n"
    "	mul.f32 	%f631, %f10, %f630;\n"
    "	sub.f32 	%f47, %f627, %f631;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f624;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd41, %r54;\n"
    "	mul.wide.s32 	%rd42, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd42;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f632, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f633, %f54, %f632;\n"
    "	cvt.rzi.f32.f32 	%f57, %f633;\n"
    "	mov.f32 	%f634, %f57;\n"
    "	mov.f32 	%f635, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f636, %f635;\n"
    "	mov.f32 	%f637, %f54;\n"
    "	mad.f32 %f638, %f634, %f636, %f637;\n"
    "	mov.f32 	%f63, %f638;\n"
    "	mov.f32 	%f639, %f57;\n"
    "	mov.f32 	%f640, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f641, %f640;\n"
    "	mov.f32 	%f642, %f63;\n"
    "	mad.f32 %f643, %f639, %f641, %f642;\n"
    "	mov.f32 	%f63, %f643;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f644, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f645, %f63, %f644;\n"
    "	mov.f32 	%f646, %f645;\n"
    "	ex2.approx.ftz.f32 %f647,%f646;\n"
    "	mov.f32 	%f73, %f647;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f648, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f648;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f649, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f649;\n"
    "	mov.f32 	%f650, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f650;\n"
    "	mov.f32 	%f651, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f651;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f652, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f652;\n"
    "	mov.f32 	%f653, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f653;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f654, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f654, %f95, %p5;\n"
    "	mov.f32 	%f655, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f655, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f656, %f75, %f101;\n"
    "	sub.f32 	%f103, %f656, %f90;\n"
    "	mul.f32 	%f657, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f657;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f658, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f658, %f106, %p3;\n"
    "	mov.f32 	%f659, %f26;\n"
    "	mad.f32 	%f15, %f659, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f660, %f38;\n"
    "	mad.f32 	%f16, %f660, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f661, %f48;\n"
    "	mad.f32 	%f17, %f661, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r65, [%rd18+2828];\n"
    "	add.s32 	%r54, %r65, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f662, [%rd19+2816];\n"
    "	mov.f32 	%f663, %f5;\n"
    "	sub.f32 	%f664, %f663, %f662;\n"
    "	mov.f32 	%f665, %f664;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f666, %f11, %f665;\n"
    "	cvt.rni.f32.f32 	%f667, %f666;\n"
    "	mul.f32 	%f668, %f10, %f667;\n"
    "	sub.f32 	%f25, %f664, %f668;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f669, 0f00000000;   	// 0\n"
    "	mad.f32 	%f670, %f25, %f25, %f669;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f671, [%rd19+2820];\n"
    "	mov.f32 	%f672, %f7;\n"
    "	sub.f32 	%f673, %f672, %f671;\n"
    "	mov.f32 	%f674, %f673;\n"
    "	mov.f32 	%f675, %f674;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f676, %f11, %f674;\n"
    "	cvt.rni.f32.f32 	%f677, %f676;\n"
    "	mul.f32 	%f678, %f10, %f677;\n"
    "	sub.f32 	%f37, %f673, %f678;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f679, %f37, %f37, %f670;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f680, [%rd19+2824];\n"
    "	mov.f32 	%f681, %f9;\n"
    "	sub.f32 	%f682, %f681, %f680;\n"
    "	mov.f32 	%f683, %f682;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f684, %f682, %f11;\n"
    "	cvt.rni.f32.f32 	%f685, %f684;\n"
    "	mul.f32 	%f686, %f10, %f685;\n"
    "	sub.f32 	%f47, %f682, %f686;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f679;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd43, %r54;\n"
    "	mul.wide.s32 	%rd44, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd44;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f687, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f688, %f54, %f687;\n"
    "	cvt.rzi.f32.f32 	%f57, %f688;\n"
    "	mov.f32 	%f689, %f57;\n"
    "	mov.f32 	%f690, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f691, %f690;\n"
    "	mov.f32 	%f692, %f54;\n"
    "	mad.f32 %f693, %f689, %f691, %f692;\n"
    "	mov.f32 	%f63, %f693;\n"
    "	mov.f32 	%f694, %f57;\n"
    "	mov.f32 	%f695, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f696, %f695;\n"
    "	mov.f32 	%f697, %f63;\n"
    "	mad.f32 %f698, %f694, %f696, %f697;\n"
    "	mov.f32 	%f63, %f698;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f699, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f700, %f63, %f699;\n"
    "	mov.f32 	%f701, %f700;\n"
    "	ex2.approx.ftz.f32 %f702,%f701;\n"
    "	mov.f32 	%f73, %f702;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f703, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f703;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f704, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f704;\n"
    "	mov.f32 	%f705, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f705;\n"
    "	mov.f32 	%f706, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f706;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f707, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f707;\n"
    "	mov.f32 	%f708, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f708;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f709, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f709, %f95, %p5;\n"
    "	mov.f32 	%f710, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f710, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f711, %f75, %f101;\n"
    "	sub.f32 	%f103, %f711, %f90;\n"
    "	mul.f32 	%f712, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f712;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f713, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f713, %f106, %p3;\n"
    "	mov.f32 	%f714, %f26;\n"
    "	mad.f32 	%f15, %f714, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f715, %f38;\n"
    "	mad.f32 	%f16, %f715, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f716, %f48;\n"
    "	mad.f32 	%f17, %f716, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r66, [%rd18+3084];\n"
    "	add.s32 	%r54, %r66, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f717, [%rd19+3072];\n"
    "	mov.f32 	%f718, %f5;\n"
    "	sub.f32 	%f719, %f718, %f717;\n"
    "	mov.f32 	%f720, %f719;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f721, %f11, %f720;\n"
    "	cvt.rni.f32.f32 	%f722, %f721;\n"
    "	mul.f32 	%f723, %f10, %f722;\n"
    "	sub.f32 	%f25, %f719, %f723;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f724, 0f00000000;   	// 0\n"
    "	mad.f32 	%f725, %f25, %f25, %f724;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f726, [%rd19+3076];\n"
    "	mov.f32 	%f727, %f7;\n"
    "	sub.f32 	%f728, %f727, %f726;\n"
    "	mov.f32 	%f729, %f728;\n"
    "	mov.f32 	%f730, %f729;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f731, %f11, %f729;\n"
    "	cvt.rni.f32.f32 	%f732, %f731;\n"
    "	mul.f32 	%f733, %f10, %f732;\n"
    "	sub.f32 	%f37, %f728, %f733;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f734, %f37, %f37, %f725;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f735, [%rd19+3080];\n"
    "	mov.f32 	%f736, %f9;\n"
    "	sub.f32 	%f737, %f736, %f735;\n"
    "	mov.f32 	%f738, %f737;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f739, %f737, %f11;\n"
    "	cvt.rni.f32.f32 	%f740, %f739;\n"
    "	mul.f32 	%f741, %f10, %f740;\n"
    "	sub.f32 	%f47, %f737, %f741;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f734;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd45, %r54;\n"
    "	mul.wide.s32 	%rd46, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd46;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f742, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f743, %f54, %f742;\n"
    "	cvt.rzi.f32.f32 	%f57, %f743;\n"
    "	mov.f32 	%f744, %f57;\n"
    "	mov.f32 	%f745, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f746, %f745;\n"
    "	mov.f32 	%f747, %f54;\n"
    "	mad.f32 %f748, %f744, %f746, %f747;\n"
    "	mov.f32 	%f63, %f748;\n"
    "	mov.f32 	%f749, %f57;\n"
    "	mov.f32 	%f750, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f751, %f750;\n"
    "	mov.f32 	%f752, %f63;\n"
    "	mad.f32 %f753, %f749, %f751, %f752;\n"
    "	mov.f32 	%f63, %f753;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f754, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f755, %f63, %f754;\n"
    "	mov.f32 	%f756, %f755;\n"
    "	ex2.approx.ftz.f32 %f757,%f756;\n"
    "	mov.f32 	%f73, %f757;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f758, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f758;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f759, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f759;\n"
    "	mov.f32 	%f760, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f760;\n"
    "	mov.f32 	%f761, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f761;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f762, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f762;\n"
    "	mov.f32 	%f763, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f763;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f764, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f764, %f95, %p5;\n"
    "	mov.f32 	%f765, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f765, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f766, %f75, %f101;\n"
    "	sub.f32 	%f103, %f766, %f90;\n"
    "	mul.f32 	%f767, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f767;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f768, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f768, %f106, %p3;\n"
    "	mov.f32 	%f769, %f26;\n"
    "	mad.f32 	%f15, %f769, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f770, %f38;\n"
    "	mad.f32 	%f16, %f770, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f771, %f48;\n"
    "	mad.f32 	%f17, %f771, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r67, [%rd18+3340];\n"
    "	add.s32 	%r54, %r67, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f772, [%rd19+3328];\n"
    "	mov.f32 	%f773, %f5;\n"
    "	sub.f32 	%f774, %f773, %f772;\n"
    "	mov.f32 	%f775, %f774;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f776, %f11, %f775;\n"
    "	cvt.rni.f32.f32 	%f777, %f776;\n"
    "	mul.f32 	%f778, %f10, %f777;\n"
    "	sub.f32 	%f25, %f774, %f778;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f779, 0f00000000;   	// 0\n"
    "	mad.f32 	%f780, %f25, %f25, %f779;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f781, [%rd19+3332];\n"
    "	mov.f32 	%f782, %f7;\n"
    "	sub.f32 	%f783, %f782, %f781;\n"
    "	mov.f32 	%f784, %f783;\n"
    "	mov.f32 	%f785, %f784;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f786, %f11, %f784;\n"
    "	cvt.rni.f32.f32 	%f787, %f786;\n"
    "	mul.f32 	%f788, %f10, %f787;\n"
    "	sub.f32 	%f37, %f783, %f788;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f789, %f37, %f37, %f780;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f790, [%rd19+3336];\n"
    "	mov.f32 	%f791, %f9;\n"
    "	sub.f32 	%f792, %f791, %f790;\n"
    "	mov.f32 	%f793, %f792;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f794, %f792, %f11;\n"
    "	cvt.rni.f32.f32 	%f795, %f794;\n"
    "	mul.f32 	%f796, %f10, %f795;\n"
    "	sub.f32 	%f47, %f792, %f796;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f789;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd47, %r54;\n"
    "	mul.wide.s32 	%rd48, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd48;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f797, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f798, %f54, %f797;\n"
    "	cvt.rzi.f32.f32 	%f57, %f798;\n"
    "	mov.f32 	%f799, %f57;\n"
    "	mov.f32 	%f800, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f801, %f800;\n"
    "	mov.f32 	%f802, %f54;\n"
    "	mad.f32 %f803, %f799, %f801, %f802;\n"
    "	mov.f32 	%f63, %f803;\n"
    "	mov.f32 	%f804, %f57;\n"
    "	mov.f32 	%f805, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f806, %f805;\n"
    "	mov.f32 	%f807, %f63;\n"
    "	mad.f32 %f808, %f804, %f806, %f807;\n"
    "	mov.f32 	%f63, %f808;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f809, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f810, %f63, %f809;\n"
    "	mov.f32 	%f811, %f810;\n"
    "	ex2.approx.ftz.f32 %f812,%f811;\n"
    "	mov.f32 	%f73, %f812;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f813, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f813;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f814, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f814;\n"
    "	mov.f32 	%f815, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f815;\n"
    "	mov.f32 	%f816, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f816;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f817, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f817;\n"
    "	mov.f32 	%f818, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f818;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f819, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f819, %f95, %p5;\n"
    "	mov.f32 	%f820, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f820, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f821, %f75, %f101;\n"
    "	sub.f32 	%f103, %f821, %f90;\n"
    "	mul.f32 	%f822, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f822;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f823, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f823, %f106, %p3;\n"
    "	mov.f32 	%f824, %f26;\n"
    "	mad.f32 	%f15, %f824, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f825, %f38;\n"
    "	mad.f32 	%f16, %f825, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f826, %f48;\n"
    "	mad.f32 	%f17, %f826, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r68, [%rd18+3596];\n"
    "	add.s32 	%r54, %r68, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f827, [%rd19+3584];\n"
    "	mov.f32 	%f828, %f5;\n"
    "	sub.f32 	%f829, %f828, %f827;\n"
    "	mov.f32 	%f830, %f829;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f831, %f11, %f830;\n"
    "	cvt.rni.f32.f32 	%f832, %f831;\n"
    "	mul.f32 	%f833, %f10, %f832;\n"
    "	sub.f32 	%f25, %f829, %f833;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f834, 0f00000000;   	// 0\n"
    "	mad.f32 	%f835, %f25, %f25, %f834;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f836, [%rd19+3588];\n"
    "	mov.f32 	%f837, %f7;\n"
    "	sub.f32 	%f838, %f837, %f836;\n"
    "	mov.f32 	%f839, %f838;\n"
    "	mov.f32 	%f840, %f839;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f841, %f11, %f839;\n"
    "	cvt.rni.f32.f32 	%f842, %f841;\n"
    "	mul.f32 	%f843, %f10, %f842;\n"
    "	sub.f32 	%f37, %f838, %f843;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f844, %f37, %f37, %f835;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f845, [%rd19+3592];\n"
    "	mov.f32 	%f846, %f9;\n"
    "	sub.f32 	%f847, %f846, %f845;\n"
    "	mov.f32 	%f848, %f847;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f849, %f847, %f11;\n"
    "	cvt.rni.f32.f32 	%f850, %f849;\n"
    "	mul.f32 	%f851, %f10, %f850;\n"
    "	sub.f32 	%f47, %f847, %f851;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f844;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd49, %r54;\n"
    "	mul.wide.s32 	%rd50, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd50;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f852, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f853, %f54, %f852;\n"
    "	cvt.rzi.f32.f32 	%f57, %f853;\n"
    "	mov.f32 	%f854, %f57;\n"
    "	mov.f32 	%f855, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f856, %f855;\n"
    "	mov.f32 	%f857, %f54;\n"
    "	mad.f32 %f858, %f854, %f856, %f857;\n"
    "	mov.f32 	%f63, %f858;\n"
    "	mov.f32 	%f859, %f57;\n"
    "	mov.f32 	%f860, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f861, %f860;\n"
    "	mov.f32 	%f862, %f63;\n"
    "	mad.f32 %f863, %f859, %f861, %f862;\n"
    "	mov.f32 	%f63, %f863;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f864, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f865, %f63, %f864;\n"
    "	mov.f32 	%f866, %f865;\n"
    "	ex2.approx.ftz.f32 %f867,%f866;\n"
    "	mov.f32 	%f73, %f867;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f868, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f868;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f869, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f869;\n"
    "	mov.f32 	%f870, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f870;\n"
    "	mov.f32 	%f871, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f871;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f872, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f872;\n"
    "	mov.f32 	%f873, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f873;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f874, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f874, %f95, %p5;\n"
    "	mov.f32 	%f875, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f875, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f876, %f75, %f101;\n"
    "	sub.f32 	%f103, %f876, %f90;\n"
    "	mul.f32 	%f877, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f877;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f878, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f878, %f106, %p3;\n"
    "	mov.f32 	%f879, %f26;\n"
    "	mad.f32 	%f15, %f879, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f880, %f38;\n"
    "	mad.f32 	%f16, %f880, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f881, %f48;\n"
    "	mad.f32 	%f17, %f881, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r69, [%rd18+3852];\n"
    "	add.s32 	%r54, %r69, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f882, [%rd19+3840];\n"
    "	mov.f32 	%f883, %f5;\n"
    "	sub.f32 	%f884, %f883, %f882;\n"
    "	mov.f32 	%f885, %f884;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f886, %f11, %f885;\n"
    "	cvt.rni.f32.f32 	%f887, %f886;\n"
    "	mul.f32 	%f888, %f10, %f887;\n"
    "	sub.f32 	%f25, %f884, %f888;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f889, 0f00000000;   	// 0\n"
    "	mad.f32 	%f890, %f25, %f25, %f889;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f891, [%rd19+3844];\n"
    "	mov.f32 	%f892, %f7;\n"
    "	sub.f32 	%f893, %f892, %f891;\n"
    "	mov.f32 	%f894, %f893;\n"
    "	mov.f32 	%f895, %f894;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f896, %f11, %f894;\n"
    "	cvt.rni.f32.f32 	%f897, %f896;\n"
    "	mul.f32 	%f898, %f10, %f897;\n"
    "	sub.f32 	%f37, %f893, %f898;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f899, %f37, %f37, %f890;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f900, [%rd19+3848];\n"
    "	mov.f32 	%f901, %f9;\n"
    "	sub.f32 	%f902, %f901, %f900;\n"
    "	mov.f32 	%f903, %f902;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f904, %f902, %f11;\n"
    "	cvt.rni.f32.f32 	%f905, %f904;\n"
    "	mul.f32 	%f906, %f10, %f905;\n"
    "	sub.f32 	%f47, %f902, %f906;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f899;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd51, %r54;\n"
    "	mul.wide.s32 	%rd52, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd52;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f907, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f908, %f54, %f907;\n"
    "	cvt.rzi.f32.f32 	%f57, %f908;\n"
    "	mov.f32 	%f909, %f57;\n"
    "	mov.f32 	%f910, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f911, %f910;\n"
    "	mov.f32 	%f912, %f54;\n"
    "	mad.f32 %f913, %f909, %f911, %f912;\n"
    "	mov.f32 	%f63, %f913;\n"
    "	mov.f32 	%f914, %f57;\n"
    "	mov.f32 	%f915, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f916, %f915;\n"
    "	mov.f32 	%f917, %f63;\n"
    "	mad.f32 %f918, %f914, %f916, %f917;\n"
    "	mov.f32 	%f63, %f918;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f919, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f920, %f63, %f919;\n"
    "	mov.f32 	%f921, %f920;\n"
    "	ex2.approx.ftz.f32 %f922,%f921;\n"
    "	mov.f32 	%f73, %f922;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f923, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f923;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f924, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f924;\n"
    "	mov.f32 	%f925, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f925;\n"
    "	mov.f32 	%f926, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f926;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f927, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f927;\n"
    "	mov.f32 	%f928, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f928;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f929, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f929, %f95, %p5;\n"
    "	mov.f32 	%f930, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f930, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f931, %f75, %f101;\n"
    "	sub.f32 	%f103, %f931, %f90;\n"
    "	mul.f32 	%f932, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f932;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f933, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f933, %f106, %p3;\n"
    "	mov.f32 	%f934, %f26;\n"
    "	mad.f32 	%f15, %f934, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f935, %f38;\n"
    "	mad.f32 	%f16, %f935, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f936, %f48;\n"
    "	mad.f32 	%f17, %f936, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	add.s32 	%r45, %r45, 256;\n"
    "	add.u64 	%rd17, %rd17, 4096;\n"
    "	setp.gt.s32 	%p6, %r40, %r45;\n"
    "	@%p6 bra 	$Lt_0_24322;\n"
    "$Lt_0_23810:\n"
    "	mov.u32 	%r70, 255;\n"
    "	setp.gt.s32 	%p7, %r45, %r70;\n"
    "	@%p7 bra 	$Lt_0_24834;\n"
    "	mov.s32 	%r71, 271;\n"
    "	sub.s32 	%r72, %r71, %r45;\n"
    "	shr.s32 	%r73, %r72, 31;\n"
    "	mov.s32 	%r74, 15;\n"
    "	and.b32 	%r75, %r73, %r74;\n"
    "	add.s32 	%r76, %r75, %r72;\n"
    "	shr.s32 	%r77, %r76, 4;\n"
    "	cvt.s64.s32 	%rd53, %r45;\n"
    "	mul.wide.s32 	%rd17, %r45, 16;\n"
    "	add.u64 	%rd18, %rd17, %rd9;\n"
    "	add.u64 	%rd19, %rd17, %rd9;\n"
    "	ld.shared.f32 	%f15, [%rd4+0];\n"
    "	ld.shared.f32 	%f16, [%rd4+4];\n"
    "	ld.shared.f32 	%f17, [%rd4+8];\n"
    "	mov.s32 	%r78, %r77;\n"
    "$Lt_0_25346:\n"
    " //<loop> Loop body line 87, nesting depth: 2, estimated iterations: 15\n"
    "	.loc	3	118	0\n"
    "	ld.shared.s32 	%r79, [%rd18+12];\n"
    "	add.s32 	%r54, %r79, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f937, [%rd19+0];\n"
    "	mov.f32 	%f938, %f5;\n"
    "	sub.f32 	%f939, %f938, %f937;\n"
    "	mov.f32 	%f940, %f939;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f941, %f11, %f940;\n"
    "	cvt.rni.f32.f32 	%f942, %f941;\n"
    "	mul.f32 	%f943, %f10, %f942;\n"
    "	sub.f32 	%f25, %f939, %f943;\n"
    "	mov.f32 	%f26, %f25;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f944, 0f00000000;   	// 0\n"
    "	mad.f32 	%f945, %f25, %f25, %f944;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f946, [%rd19+4];\n"
    "	mov.f32 	%f947, %f7;\n"
    "	sub.f32 	%f948, %f947, %f946;\n"
    "	mov.f32 	%f949, %f948;\n"
    "	mov.f32 	%f950, %f949;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f951, %f11, %f949;\n"
    "	cvt.rni.f32.f32 	%f952, %f951;\n"
    "	mul.f32 	%f953, %f10, %f952;\n"
    "	sub.f32 	%f37, %f948, %f953;\n"
    "	mov.f32 	%f38, %f37;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f954, %f37, %f37, %f945;\n"
    "	.loc	3	58	0\n"
    "	ld.shared.f32 	%f955, [%rd19+8];\n"
    "	mov.f32 	%f956, %f9;\n"
    "	sub.f32 	%f957, %f956, %f955;\n"
    "	mov.f32 	%f958, %f957;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f959, %f957, %f11;\n"
    "	cvt.rni.f32.f32 	%f960, %f959;\n"
    "	mul.f32 	%f961, %f10, %f960;\n"
    "	sub.f32 	%f47, %f957, %f961;\n"
    "	mov.f32 	%f48, %f47;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f49, %f47, %f47, %f954;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f50, %f49;\n"
    "	cvt.s64.s32 	%rd54, %r54;\n"
    "	mul.wide.s32 	%rd55, %r54, 24;\n"
    "	add.u64 	%rd22, %rd12, %rd55;\n"
    "	ld.const.f32 	%f51, [%rd22+8];\n"
    "	ld.const.f32 	%f52, [%rd22+4];\n"
    "	sub.f32 	%f53, %f52, %f50;\n"
    "	mul.f32 	%f54, %f51, %f53;\n"
    "	mov.f32 	%f962, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f963, %f54, %f962;\n"
    "	cvt.rzi.f32.f32 	%f57, %f963;\n"
    "	mov.f32 	%f964, %f57;\n"
    "	mov.f32 	%f965, 0fbf317200;   	// -0.693146\n"
    "	mov.f32 	%f966, %f965;\n"
    "	mov.f32 	%f967, %f54;\n"
    "	mad.f32 %f968, %f964, %f966, %f967;\n"
    "	mov.f32 	%f63, %f968;\n"
    "	mov.f32 	%f969, %f57;\n"
    "	mov.f32 	%f970, 0fb5bfbe8e;   	// -1.42861e-06\n"
    "	mov.f32 	%f971, %f970;\n"
    "	mov.f32 	%f972, %f63;\n"
    "	mad.f32 %f973, %f969, %f971, %f972;\n"
    "	mov.f32 	%f63, %f973;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f974, 0f3fb8aa3b;   	// 1.4427\n"
    "	mul.f32 	%f975, %f63, %f974;\n"
    "	mov.f32 	%f976, %f975;\n"
    "	ex2.approx.ftz.f32 %f977,%f976;\n"
    "	mov.f32 	%f73, %f977;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f978, 0f00000000;   	// 0\n"
    "	setp.eq.f32 	%p3, %f49, %f978;\n"
    "	rcp.approx.f32 	%f75, %f50;\n"
    "	mul.f32 	%f76, %f75, %f75;\n"
    "	ld.const.f32 	%f77, [%rd22+12];\n"
    "	ld.const.f32 	%f78, [%rd22+0];\n"
    "	ld.const.f32 	%f79, [%rd22+16];\n"
    "	ld.const.f32 	%f80, [%rd22+20];\n"
    "	mul.f32 	%f81, %f76, %f76;\n"
    "	mul.f32 	%f82, %f75, %f76;\n"
    "	mov.f32 	%f979, 0f40c00000;   	// 6\n"
    "	mul.f32 	%f84, %f77, %f979;\n"
    "	mov.f32 	%f980, 0f3c70319e;   	// 0.0146603\n"
    "	mul.f32 	%f86, %f78, %f980;\n"
    "	mov.f32 	%f981, 0f41000000;   	// 8\n"
    "	mul.f32 	%f88, %f79, %f981;\n"
    "	mul.f32 	%f89, %f81, %f81;\n"
    "	mul.f32 	%f90, %f84, %f89;\n"
    "	mul.f32 	%f91, %f88, %f89;\n"
    "	mov.f32 	%f982, 0f42d20000;   	// 105\n"
    "	setp.gt.f32 	%p4, %f54, %f982;\n"
    "	mov.f32 	%f983, 0fc2d20000;   	// -105\n"
    "	setp.lt.f32 	%p5, %f54, %f983;\n"
    "	ex2.approx.f32 	%f94, %f57;\n"
    "	mul.f32 	%f95, %f94, %f73;\n"
    "	mov.f32 	%f984, 0f00000000;   	// 0\n"
    "	selp.f32 	%f97, %f984, %f95, %p5;\n"
    "	mov.f32 	%f985, 0f7f800000;   	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f99, %f985, %f97, %p4;\n"
    "	mul.f32 	%f100, %f86, %f99;\n"
    "	mul.f32 	%f101, %f51, %f100;\n"
    "	mul.f32 	%f986, %f75, %f101;\n"
    "	sub.f32 	%f103, %f986, %f90;\n"
    "	mul.f32 	%f987, %f76, %f91;\n"
    "	sub.f32 	%f105, %f103, %f987;\n"
    "	mad.f32 	%f106, %f80, %f82, %f105;\n"
    "	mov.f32 	%f988, 0f00000000;   	// 0\n"
    "	selp.f32 	%f108, %f988, %f106, %p3;\n"
    "	mov.f32 	%f989, %f26;\n"
    "	mad.f32 	%f15, %f989, %f108, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f990, %f38;\n"
    "	mad.f32 	%f16, %f990, %f108, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f991, %f48;\n"
    "	mad.f32 	%f17, %f991, %f108, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	add.s32 	%r45, %r45, 16;\n"
    "	add.u64 	%rd19, %rd19, 256;\n"
    "	add.u64 	%rd18, %rd18, 256;\n"
    "	mov.u32 	%r80, 255;\n"
    "	setp.le.s32 	%p8, %r45, %r80;\n"
    "	@%p8 bra 	$Lt_0_25346;\n"
    "$Lt_0_24834:\n"
    "	add.s32 	%r41, %r41, 256;\n"
    "	setp.gt.s32 	%p9, %r19, %r41;\n"
    "	@%p9 bra 	$Lt_0_23554;\n"
    "$Lt_0_23042:\n"
    "	mov.u64 	%rd12, c_matrix;\n"
    "	.loc	3	120	0\n"
    "	add.s32 	%r81, %r18, %r19;\n"
    "	mov.s32 	%r41, %r81;\n"
    "	setp.ge.s32 	%p10, %r81, %r8;\n"
    "	@%p10 bra 	$Lt_0_26114;\n"
    "	sub.s32 	%r82, %r8, %r81;\n"
    "	add.s32 	%r83, %r82, 15;\n"
    "	shr.s32 	%r84, %r83, 31;\n"
    "	mov.s32 	%r85, 15;\n"
    "	and.b32 	%r86, %r84, %r85;\n"
    "	add.s32 	%r87, %r86, %r83;\n"
    "	shr.s32 	%r88, %r87, 4;\n"
    "	ld.param.f32 	%f10, [__cudaparm_nacl_kernel_if2_xmax];\n"
    "	rcp.approx.f32 	%f11, %f10;\n"
    "	cvt.s64.s32 	%rd56, %r81;\n"
    "	mul.wide.s32 	%rd57, %r81, 16;\n"
    "	add.u64 	%rd58, %rd5, %rd57;\n"
    "	ld.shared.f32 	%f15, [%rd4+0];\n"
    "	ld.shared.f32 	%f16, [%rd4+4];\n"
    "	ld.shared.f32 	%f17, [%rd4+8];\n"
    "	mov.s32 	%r89, %r88;\n"
    "$Lt_0_26626:\n"
    " //<loop> Loop body line 120, nesting depth: 1, estimated iterations: unknown\n"
    "	.loc	3	121	0\n"
    "	ld.global.s32 	%r90, [%rd58+12];\n"
    "	add.s32 	%r91, %r90, %r7;\n"
    "	.loc	3	58	0\n"
    "	ld.global.f32 	%f992, [%rd58+0];\n"
    "	mov.f32 	%f993, %f5;\n"
    "	sub.f32 	%f994, %f993, %f992;\n"
    "	mov.f32 	%f995, %f994;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f996, %f11, %f995;\n"
    "	cvt.rni.f32.f32 	%f997, %f996;\n"
    "	.loc	3	120	0\n"
    "	ld.param.f32 	%f10, [__cudaparm_nacl_kernel_if2_xmax];\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f998, %f10, %f997;\n"
    "	sub.f32 	%f999, %f994, %f998;\n"
    "	mov.f32 	%f1000, %f999;\n"
    "	.loc	3	60	0\n"
    "	mov.f32 	%f1001, 0f00000000;  	// 0\n"
    "	mad.f32 	%f1002, %f999, %f999, %f1001;\n"
    "	.loc	3	58	0\n"
    "	ld.global.f32 	%f1003, [%rd58+4];\n"
    "	mov.f32 	%f1004, %f7;\n"
    "	sub.f32 	%f1005, %f1004, %f1003;\n"
    "	mov.f32 	%f1006, %f1005;\n"
    "	mov.f32 	%f1007, %f1006;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f1008, %f11, %f1006;\n"
    "	cvt.rni.f32.f32 	%f1009, %f1008;\n"
    "	mul.f32 	%f1010, %f10, %f1009;\n"
    "	sub.f32 	%f1011, %f1005, %f1010;\n"
    "	mov.f32 	%f1012, %f1011;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f1013, %f1011, %f1011, %f1002;\n"
    "	.loc	3	58	0\n"
    "	ld.global.f32 	%f1014, [%rd58+8];\n"
    "	mov.f32 	%f1015, %f9;\n"
    "	sub.f32 	%f1016, %f1015, %f1014;\n"
    "	mov.f32 	%f1017, %f1016;\n"
    "	.loc	3	59	0\n"
    "	mul.f32 	%f1018, %f1016, %f11;\n"
    "	cvt.rni.f32.f32 	%f1019, %f1018;\n"
    "	mul.f32 	%f1020, %f10, %f1019;\n"
    "	sub.f32 	%f1021, %f1016, %f1020;\n"
    "	mov.f32 	%f1022, %f1021;\n"
    "	.loc	3	60	0\n"
    "	mad.f32 	%f1023, %f1021, %f1021, %f1013;\n"
    "	.loc	16	9180	0\n"
    "	sqrt.approx.f32 	%f1024, %f1023;\n"
    "	cvt.s64.s32 	%rd59, %r91;\n"
    "	mul.wide.s32 	%rd60, %r91, 24;\n"
    "	add.u64 	%rd61, %rd12, %rd60;\n"
    "	ld.const.f32 	%f1025, [%rd61+8];\n"
    "	ld.const.f32 	%f1026, [%rd61+4];\n"
    "	sub.f32 	%f1027, %f1026, %f1024;\n"
    "	mul.f32 	%f1028, %f1025, %f1027;\n"
    "	mov.f32 	%f1029, 0f3fb8aa3b;  	// 1.4427\n"
    "	mul.f32 	%f1030, %f1028, %f1029;\n"
    "	cvt.rzi.f32.f32 	%f1031, %f1030;\n"
    "	mov.f32 	%f1032, %f1031;\n"
    "	mov.f32 	%f1033, 0fbf317200;  	// -0.693146\n"
    "	mov.f32 	%f1034, %f1033;\n"
    "	mov.f32 	%f1035, %f1028;\n"
    "	mad.f32 %f1036, %f1032, %f1034, %f1035;\n"
    "	mov.f32 	%f63, %f1036;\n"
    "	mov.f32 	%f1037, %f1031;\n"
    "	mov.f32 	%f1038, 0fb5bfbe8e;  	// -1.42861e-06\n"
    "	mov.f32 	%f1039, %f1038;\n"
    "	mov.f32 	%f1040, %f63;\n"
    "	mad.f32 %f1041, %f1037, %f1039, %f1040;\n"
    "	mov.f32 	%f63, %f1041;\n"
    "	.loc	16	9209	0\n"
    "	mov.f32 	%f1042, 0f3fb8aa3b;  	// 1.4427\n"
    "	mul.f32 	%f1043, %f63, %f1042;\n"
    "	mov.f32 	%f1044, %f1043;\n"
    "	ex2.approx.ftz.f32 %f1045,%f1044;\n"
    "	mov.f32 	%f73, %f1045;\n"
    "	.loc	3	87	0\n"
    "	mov.f32 	%f1046, 0f00000000;  	// 0\n"
    "	setp.eq.f32 	%p11, %f1023, %f1046;\n"
    "	rcp.approx.f32 	%f1047, %f1024;\n"
    "	mul.f32 	%f1048, %f1047, %f1047;\n"
    "	ld.const.f32 	%f1049, [%rd61+12];\n"
    "	ld.const.f32 	%f1050, [%rd61+0];\n"
    "	ld.const.f32 	%f1051, [%rd61+16];\n"
    "	ld.const.f32 	%f1052, [%rd61+20];\n"
    "	mul.f32 	%f1053, %f1048, %f1048;\n"
    "	mul.f32 	%f1054, %f1047, %f1048;\n"
    "	mov.f32 	%f1055, 0f40c00000;  	// 6\n"
    "	mul.f32 	%f1056, %f1049, %f1055;\n"
    "	mov.f32 	%f1057, 0f3c70319e;  	// 0.0146603\n"
    "	mul.f32 	%f1058, %f1050, %f1057;\n"
    "	mov.f32 	%f1059, 0f41000000;  	// 8\n"
    "	mul.f32 	%f1060, %f1051, %f1059;\n"
    "	mul.f32 	%f1061, %f1053, %f1053;\n"
    "	mul.f32 	%f1062, %f1056, %f1061;\n"
    "	mul.f32 	%f1063, %f1060, %f1061;\n"
    "	mov.f32 	%f1064, 0f42d20000;  	// 105\n"
    "	setp.gt.f32 	%p12, %f1028, %f1064;\n"
    "	mov.f32 	%f1065, 0fc2d20000;  	// -105\n"
    "	setp.lt.f32 	%p13, %f1028, %f1065;\n"
    "	ex2.approx.f32 	%f1066, %f1031;\n"
    "	mul.f32 	%f1067, %f1066, %f73;\n"
    "	mov.f32 	%f1068, 0f00000000;  	// 0\n"
    "	selp.f32 	%f1069, %f1068, %f1067, %p13;\n"
    "	mov.f32 	%f1070, 0f7f800000;  	// ((1.0F)/(0.0F))\n"
    "	selp.f32 	%f1071, %f1070, %f1069, %p12;\n"
    "	mul.f32 	%f1072, %f1058, %f1071;\n"
    "	mul.f32 	%f1073, %f1025, %f1072;\n"
    "	mul.f32 	%f1074, %f1047, %f1073;\n"
    "	sub.f32 	%f1075, %f1074, %f1062;\n"
    "	mul.f32 	%f1076, %f1048, %f1063;\n"
    "	sub.f32 	%f1077, %f1075, %f1076;\n"
    "	mad.f32 	%f1078, %f1052, %f1054, %f1077;\n"
    "	mov.f32 	%f1079, 0f00000000;  	// 0\n"
    "	selp.f32 	%f1080, %f1079, %f1078, %p11;\n"
    "	mov.f32 	%f1081, %f1000;\n"
    "	mad.f32 	%f15, %f1081, %f1080, %f15;\n"
    "	st.shared.f32 	[%rd4+0], %f15;\n"
    "	mov.f32 	%f1082, %f1012;\n"
    "	mad.f32 	%f16, %f1082, %f1080, %f16;\n"
    "	st.shared.f32 	[%rd4+4], %f16;\n"
    "	mov.f32 	%f1083, %f1022;\n"
    "	mad.f32 	%f17, %f1083, %f1080, %f17;\n"
    "	st.shared.f32 	[%rd4+8], %f17;\n"
    "	add.s32 	%r41, %r41, 16;\n"
    "	add.u64 	%rd58, %rd58, 256;\n"
    "	setp.gt.s32 	%p14, %r8, %r41;\n"
    "	@%p14 bra 	$Lt_0_26626;\n"
    "$Lt_0_26114:\n"
    "	.loc	3	128	0\n"
    "	bar.sync 	0;\n"
    "	mov.u32 	%r92, 127;\n"
    "	setp.gt.s32 	%p15, %r1, %r92;\n"
    "	@%p15 bra 	$Lt_0_27138;\n"
    "	.loc	3	129	0\n"
    "	ld.shared.f32 	%f1084, [%rd4+0];\n"
    "	ld.shared.f32 	%f1085, [%rd4+1536];\n"
    "	add.f32 	%f1086, %f1084, %f1085;\n"
    "	st.shared.f32 	[%rd4+0], %f1086;\n"
    "	ld.shared.f32 	%f1087, [%rd4+4];\n"
    "	ld.shared.f32 	%f1088, [%rd4+1540];\n"
    "	add.f32 	%f1089, %f1087, %f1088;\n"
    "	st.shared.f32 	[%rd4+4], %f1089;\n"
    "	ld.shared.f32 	%f1090, [%rd4+8];\n"
    "	ld.shared.f32 	%f1091, [%rd4+1544];\n"
    "	add.f32 	%f1092, %f1090, %f1091;\n"
    "	st.shared.f32 	[%rd4+8], %f1092;\n"
    "$Lt_0_27138:\n"
    "	.loc	3	132	0\n"
    "	bar.sync 	0;\n"
    "	mov.u32 	%r93, 63;\n"
    "	setp.gt.s32 	%p16, %r1, %r93;\n"
    "	@%p16 bra 	$Lt_0_27650;\n"
    "	.loc	3	133	0\n"
    "	ld.shared.f32 	%f1093, [%rd4+0];\n"
    "	ld.shared.f32 	%f1094, [%rd4+768];\n"
    "	add.f32 	%f1095, %f1093, %f1094;\n"
    "	st.shared.f32 	[%rd4+0], %f1095;\n"
    "	ld.shared.f32 	%f1096, [%rd4+4];\n"
    "	ld.shared.f32 	%f1097, [%rd4+772];\n"
    "	add.f32 	%f1098, %f1096, %f1097;\n"
    "	st.shared.f32 	[%rd4+4], %f1098;\n"
    "	ld.shared.f32 	%f1099, [%rd4+8];\n"
    "	ld.shared.f32 	%f1100, [%rd4+776];\n"
    "	add.f32 	%f1101, %f1099, %f1100;\n"
    "	st.shared.f32 	[%rd4+8], %f1101;\n"
    "$Lt_0_27650:\n"
    "	.loc	3	136	0\n"
    "	bar.sync 	0;\n"
    "	mov.u32 	%r94, 31;\n"
    "	setp.gt.s32 	%p17, %r1, %r94;\n"
    "	@%p17 bra 	$Lt_0_28162;\n"
    "	.loc	3	137	0\n"
    "	ld.shared.f32 	%f1102, [%rd4+0];\n"
    "	ld.shared.f32 	%f1103, [%rd4+384];\n"
    "	add.f32 	%f1104, %f1102, %f1103;\n"
    "	st.shared.f32 	[%rd4+0], %f1104;\n"
    "	ld.shared.f32 	%f1105, [%rd4+4];\n"
    "	ld.shared.f32 	%f1106, [%rd4+388];\n"
    "	add.f32 	%f1107, %f1105, %f1106;\n"
    "	st.shared.f32 	[%rd4+4], %f1107;\n"
    "	ld.shared.f32 	%f1108, [%rd4+8];\n"
    "	ld.shared.f32 	%f1109, [%rd4+392];\n"
    "	add.f32 	%f1110, %f1108, %f1109;\n"
    "	st.shared.f32 	[%rd4+8], %f1110;\n"
    "$Lt_0_28162:\n"
    "	mov.u32 	%r95, 15;\n"
    "	setp.gt.s32 	%p18, %r1, %r95;\n"
    "	@%p18 bra 	$Lt_0_28674;\n"
    "	.loc	3	140	0\n"
    "	ld.shared.f32 	%f1111, [%rd4+0];\n"
    "	ld.shared.f32 	%f1112, [%rd4+192];\n"
    "	add.f32 	%f1113, %f1111, %f1112;\n"
    "	st.shared.f32 	[%rd4+0], %f1113;\n"
    "	ld.shared.f32 	%f1114, [%rd4+4];\n"
    "	ld.shared.f32 	%f1115, [%rd4+196];\n"
    "	add.f32 	%f1116, %f1114, %f1115;\n"
    "	st.shared.f32 	[%rd4+4], %f1116;\n"
    "	ld.shared.f32 	%f1117, [%rd4+8];\n"
    "	ld.shared.f32 	%f1118, [%rd4+200];\n"
    "	add.f32 	%f1119, %f1117, %f1118;\n"
    "	st.shared.f32 	[%rd4+8], %f1119;\n"
    "$Lt_0_28674:\n"
    "	mov.u32 	%r96, 0;\n"
    "	setp.ne.s32 	%p19, %r18, %r96;\n"
    "	@%p19 bra 	$Lt_0_29186;\n"
    "	.loc	3	154	0\n"
    "	ld.param.u64 	%rd62, [__cudaparm_nacl_kernel_if2_fvec];\n"
    "	mul.lo.s32 	%r97, %r4, 3;\n"
    "	cvt.s64.s32 	%rd63, %r97;\n"
    "	mul.wide.s32 	%rd64, %r97, 4;\n"
    "	add.u64 	%rd65, %rd62, %rd64;\n"
    "	ld.shared.f32 	%f1120, [%rd4+0];\n"
    "	st.global.f32 	[%rd65+0], %f1120;\n"
    "	ld.shared.f32 	%f1121, [%rd4+4];\n"
    "	st.global.f32 	[%rd65+4], %f1121;\n"
    "	ld.shared.f32 	%f1122, [%rd4+8];\n"
    "	st.global.f32 	[%rd65+8], %f1122;\n"
    "$Lt_0_29186:\n"
    "	.loc	3	155	0\n"
    "	exit;\n"
    "$LDWend_nacl_kernel_if2:\n"
    "	} // nacl_kernel_if2\n"
    "\n";
#pragma dscuda endofptx
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cutil.h>

#define NMAX      8192
#define NTHRE      256
#define ATYPE       32
#define ATYPE2    (ATYPE * ATYPE)
#define NDIVBIT      4         
#define NDIV      (1<<NDIVBIT) 
#define NTHRE2    (NTHRE/NDIV) 



#define D2F_AND_COPY(n,host_mem,device_mem,float_mem) \
  for(int i=0;i<(n);i++) ((float *)(float_mem))[i]=(host_mem)[i];\
  CUDA_SAFE_CALL(cudaMalloc((void **)&(device_mem),sizeof(float)*(n)));\
  CUDA_SAFE_CALL(cudaMemcpy((device_mem),(float_mem),sizeof(float)*(n),cudaMemcpyHostToDevice));

typedef struct {
  float r[3];
  int atype;
} VG_XVEC;

typedef struct {
  float pol;
  float sigm;
  float ipotro;
  float pc;
  float pd;
  float zz;
} VG_MATRIX;


/*
__constant__ VG_MATRIX c_matrix[4]={
[0].pol=1.250000,[0].sigm=2.340000,[0].ipotro=3.154574,[0].pc=0.072868,[0].pd=0.034699,[0].zz=1.000000,
[1].pol=1.000000,[1].sigm=2.755000,[1].ipotro=3.154574,[1].pc=0.485784,[1].pd=0.602893,[1].zz=-1.000000,
[2].pol=1.000000,[2].sigm=2.755000,[2].ipotro=3.154574,[2].pc=0.485784,[2].pd=0.602893,[2].zz=-1.000000,
[3].pol=0.750000,[3].sigm=3.170000,[3].ipotro=3.154574,[3].pc=5.031334,[3].pd=10.106042,[3].zz=1.000000,
};

static int Dev=-1;


__device__ __inline__
void inter_if(float xj[3], float xi[3], float fi[3],
           int t, float xmax, float xmax1)
{
  int k;
  float dn2,r,inr,inr2,inr4,inr8,d3,dr[3];
  float pb=(float)(0.338e-19/(14.39*1.60219e-19)),dphir;

  dn2 = 0.0f;
  for(k=0; k<3; k++){
    dr[k]  = xi[k] - xj[k];
    dr[k] -= rintf(dr[k] * xmax1) * xmax;
    dn2   += dr[k] * dr[k];
  }
  r     = sqrtf(dn2);
#if 1
  inr   = 1.0f / r;
#elif 0
  if(dn2 != 0.0f) inr   = 1.0f / r;
  else            inr   = 0.0f;
#elif 0
  if(dn2 == 0.0f) inr   = 0.0f;
  else            inr   = 1.0f / r;
#else
  inr   = 1.0f / r;
  if(dn2 == 0.0f) inr   = 0.0f;
#endif
  inr2  = inr  * inr;
  inr4  = inr2 * inr2;
  inr8  = inr4 * inr4;
  d3    = pb * c_matrix[t].pol * expf( (c_matrix[t].sigm - r)
                               * c_matrix[t].ipotro);
  dphir = ( d3 * c_matrix[t].ipotro * inr
          - 6.0f * c_matrix[t].pc * inr8
          - 8.0f * c_matrix[t].pd * inr8 * inr2
          + inr2 * inr * c_matrix[t].zz );
#if 1
  if(dn2 == 0.0f) dphir = 0.0f;
#endif
  for(k=0; k<3; k++) fi[k] += dphir * dr[k];
}


*/

/*
 * stub for remote call to nacl_kernel_if2.
 */
void
dscudanacl_kernel_if2(dim3 _gdim, dim3 _bdim, size_t _smemsize, cudaStream_t _stream , VG_XVEC *x, int n, int nat,
                 float xmax, float *fvec)
{
    int _narg = 5;
    int _grid[3], _block[3];
    RCArg _arg[5], *_argp;
    int _off = 0;
    int _rcargc = 0;
    void *_devptr;
    static char mangledname_[512] = {0,};
    if (!mangledname_[0]) {
        if (0) {
          dscudaGetMangledFunctionName(mangledname_, __PRETTY_FUNCTION__, Ptxdata);
        }
        else {
          char buf_[256];
          sprintf(buf_, "%s", __FUNCTION__);
          strcpy(mangledname_, buf_ + strlen("dscuda")); // obtain original function name.
        }
//        WARN(3, "mangled name : %s\n", mangledname_);
    }


    // a pointer to a device-address 'dscudaAdrOfUva((void *)x)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)x);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;


    // an integer 'n'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _off = dscudaAlignUp(_off, __alignof(int));
    _argp->type = dscudaArgTypeI;
    _argp->offset = _off;
    _argp->val.intval = n;
    _argp->size = sizeof(int);
    _off += _argp->size;


    // an integer 'nat'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _off = dscudaAlignUp(_off, __alignof(int));
    _argp->type = dscudaArgTypeI;
    _argp->offset = _off;
    _argp->val.intval = nat;
    _argp->size = sizeof(int);
    _off += _argp->size;


    // a float 'xmax'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _off = dscudaAlignUp(_off, __alignof(float));
    _argp->type = dscudaArgTypeF;
    _argp->offset = _off;
    _argp->val.floatval = xmax;
    _argp->size = sizeof(float);
    _off += _argp->size;


    // a pointer to a device-address 'dscudaAdrOfUva((void *)fvec)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)fvec);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;

        _grid[0] = _gdim.x; _grid[1] = _gdim.y; _grid[2] = _gdim.z;
        _block[0] = _bdim.x; _block[1] = _bdim.y; _block[2] = _gdim.z;
        dscudaLaunchKernelWrapper(dscudaLoadModule("./dscudatmp/mr3.cu.ptx", Ptxdata), 0, mangledname_,
                                 _grid, _block, _smemsize, (RCstream)_stream,
                                 _narg, _arg);
    }
 extern "C" 
void nacl_kernel_if2 (VG_XVEC *x, int n, int nat,
                 float xmax, float *fvec)
{
    /* nop */
}




extern "C"
void MR3calcnacl(double x[], int n, int atype[], int nat,
		 double pol[], double sigm[], double ipotro[],
		 double pc[], double pd[], double zz[],
		 int tblno, double xmax, int periodicflag,
		 double force[])
{
  int i,j;
  static VG_XVEC *d_x=NULL;
  static float *d_force=NULL;
  float xmaxf;
  VG_MATRIX *matrix=(VG_MATRIX *)force;
  static VG_XVEC   *vec=NULL;
  if((periodicflag & 1)==0) xmax*=2.0;
  xmaxf=xmax;
  static float *forcef=NULL;
  static int n_bak=0;

#if 0
  vec=(VG_XVEC *)force;
#endif

  
  if(sizeof(double)*n*3<sizeof(VG_MATRIX)*nat*nat){
    fprintf(stderr,"** error : n*3<nat*nat **\n");
    exit(1);
  }
  if(nat>ATYPE){
    fprintf(stderr,"** error : nat is too large **\n");
    exit(1);
  }

  if(n!=n_bak){
    
    int nalloc;
    static int nalloc_bak=0;
    if(n>NMAX) nalloc=n;
    else       nalloc=NMAX;
    if(nalloc!=nalloc_bak){
      
      
      CUDA_SAFE_CALL(cudaMalloc((void**)&d_x,sizeof(VG_XVEC)*(nalloc+NTHRE2)));
      CUDA_SAFE_CALL(cudaMalloc((void**)&d_force,sizeof(float)*(nalloc+NTHRE2)*3));

#if 1
      free(vec);
      if((vec=(VG_XVEC *)malloc(sizeof(VG_XVEC)*(nalloc+NTHRE2)))==NULL){
	fprintf(stderr,"** error : can't malloc vec **\n");
	exit(1);
      }
#endif
      free(forcef);
      if((forcef=(float *)malloc(sizeof(float)*nalloc*3))==NULL){
	fprintf(stderr,"** error : can't malloc forcef **\n");
	exit(1);
      }
      bzero(forcef,sizeof(float)*nalloc*3);
      nalloc_bak=nalloc;
    }

    
    for(i=0;i<nat;i++){
      for(j=0;j<nat;j++){
	matrix[i*nat+j].pol=(float)(pol[i*nat+j]);
	
	matrix[i*nat+j].sigm=(float)(sigm[i*nat+j]);
	
	matrix[i*nat+j].ipotro=(float)(ipotro[i*nat+j]);
	
	matrix[i*nat+j].pc=(float)(pc[i*nat+j]);
	
	matrix[i*nat+j].pd=(float)(pd[i*nat+j]);
	
	matrix[i*nat+j].zz=(float)(zz[i*nat+j]);
	
      }
    }




    

    n_bak=n;
  }

  for(i=0;i<(n+NTHRE2-1)/NTHRE2*NTHRE2;i++){
    if(i<n){
      for(j=0;j<3;j++){
	vec[i].r[j]=x[i*3+j];
      }
      vec[i].atype=atype[i];
    }
    else{
      for(j=0;j<3;j++){
	vec[i].r[j]=0.0f;
      }
      vec[i].atype=0;
    }
  }
  CUDA_SAFE_CALL(cudaMemcpy(d_x,vec,sizeof(VG_XVEC)*((n+NTHRE2-1)/NTHRE2*NTHRE2),
  			    cudaMemcpyHostToDevice));

  
  dim3 threads(NTHRE);
  dim3 grid((n*NDIV+NTHRE-1)/NTHRE);
  dscudanacl_kernel_if2(grid, threads , 0, 0, d_x, n, nat, xmaxf, d_force);

  

  
  CUDA_SAFE_CALL(cudaMemcpy(forcef,d_force,sizeof(float)*n*3,cudaMemcpyDeviceToHost));
  for(i=0;i<n;i++) for(j=0;j<3;j++) force[i*3+j]=forcef[i*3+j];
}


