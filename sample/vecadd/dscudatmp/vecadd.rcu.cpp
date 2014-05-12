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
    "	// Compiling /tmp/tmpxft_0000740a_00000000-9_vecadd.cpp3.i (/tmp/ccBI#.4xwPWa)\n"
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
    "	.file	2	\"/tmp/tmpxft_0000740a_00000000-8_vecadd.cudafe2.gpu\"\n"
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
    "	.file	14	\"vecadd.cuh\"\n"
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
    "\n"
    "	.entry _Z6vecAddPfS_S_ (\n"
    "		.param .u64 __cudaparm__Z6vecAddPfS_S__a,\n"
    "		.param .u64 __cudaparm__Z6vecAddPfS_S__b,\n"
    "		.param .u64 __cudaparm__Z6vecAddPfS_S__c)\n"
    "	{\n"
    "	.reg .u16 %rh<4>;\n"
    "	.reg .u32 %r<5>;\n"
    "	.reg .u64 %rd<10>;\n"
    "	.reg .f32 %f<5>;\n"
    "	.loc	14	2	0\n"
    "$LDWbegin__Z6vecAddPfS_S_:\n"
    "	.loc	14	5	0\n"
    "	cvt.u32.u16 	%r1, %tid.x;\n"
    "	mov.u16 	%rh1, %ctaid.x;\n"
    "	mov.u16 	%rh2, %ntid.x;\n"
    "	mul.wide.u16 	%r2, %rh1, %rh2;\n"
    "	add.u32 	%r3, %r1, %r2;\n"
    "	cvt.s64.s32 	%rd1, %r3;\n"
    "	mul.wide.s32 	%rd2, %r3, 4;\n"
    "	ld.param.u64 	%rd3, [__cudaparm__Z6vecAddPfS_S__a];\n"
    "	add.u64 	%rd4, %rd3, %rd2;\n"
    "	ld.global.f32 	%f1, [%rd4+0];\n"
    "	ld.param.u64 	%rd5, [__cudaparm__Z6vecAddPfS_S__b];\n"
    "	add.u64 	%rd6, %rd5, %rd2;\n"
    "	ld.global.f32 	%f2, [%rd6+0];\n"
    "	add.f32 	%f3, %f1, %f2;\n"
    "	ld.param.u64 	%rd7, [__cudaparm__Z6vecAddPfS_S__c];\n"
    "	add.u64 	%rd8, %rd7, %rd2;\n"
    "	st.global.f32 	[%rd8+0], %f3;\n"
    "	.loc	14	6	0\n"
    "	exit;\n"
    "$LDWend__Z6vecAddPfS_S_:\n"
    "	} // _Z6vecAddPfS_S_\n"
    "\n"
    "	.entry _Z6vecMulPfS_fS_iPi (\n"
    "		.param .u64 __cudaparm__Z6vecMulPfS_fS_iPi_a,\n"
    "		.param .u64 __cudaparm__Z6vecMulPfS_fS_iPi_b,\n"
    "		.param .f32 __cudaparm__Z6vecMulPfS_fS_iPi_c,\n"
    "		.param .u64 __cudaparm__Z6vecMulPfS_fS_iPi_d,\n"
    "		.param .s32 __cudaparm__Z6vecMulPfS_fS_iPi_e,\n"
    "		.param .u64 __cudaparm__Z6vecMulPfS_fS_iPi_f)\n"
    "	{\n"
    "	.reg .u32 %r<5>;\n"
    "	.reg .u64 %rd<12>;\n"
    "	.reg .f32 %f<10>;\n"
    "	.loc	14	9	0\n"
    "$LDWbegin__Z6vecMulPfS_fS_iPi:\n"
    "	.loc	14	12	0\n"
    "	cvt.s32.u16 	%r1, %tid.x;\n"
    "	cvt.s64.s32 	%rd1, %r1;\n"
    "	mul.wide.s32 	%rd2, %r1, 4;\n"
    "	ld.param.u64 	%rd3, [__cudaparm__Z6vecMulPfS_fS_iPi_f];\n"
    "	add.u64 	%rd4, %rd3, %rd2;\n"
    "	ld.global.s32 	%r2, [%rd4+0];\n"
    "	cvt.rn.f32.s32 	%f1, %r2;\n"
    "	ld.param.s32 	%r3, [__cudaparm__Z6vecMulPfS_fS_iPi_e];\n"
    "	cvt.rn.f32.s32 	%f2, %r3;\n"
    "	ld.param.f32 	%f3, [__cudaparm__Z6vecMulPfS_fS_iPi_c];\n"
    "	ld.param.u64 	%rd5, [__cudaparm__Z6vecMulPfS_fS_iPi_a];\n"
    "	add.u64 	%rd6, %rd5, %rd2;\n"
    "	ld.global.f32 	%f4, [%rd6+0];\n"
    "	ld.param.u64 	%rd7, [__cudaparm__Z6vecMulPfS_fS_iPi_b];\n"
    "	add.u64 	%rd8, %rd7, %rd2;\n"
    "	ld.global.f32 	%f5, [%rd8+0];\n"
    "	mad.f32 	%f6, %f4, %f5, %f3;\n"
    "	add.f32 	%f7, %f2, %f6;\n"
    "	add.f32 	%f8, %f1, %f7;\n"
    "	ld.param.u64 	%rd9, [__cudaparm__Z6vecMulPfS_fS_iPi_d];\n"
    "	add.u64 	%rd10, %rd9, %rd2;\n"
    "	st.global.f32 	[%rd10+0], %f8;\n"
    "	.loc	14	13	0\n"
    "	exit;\n"
    "$LDWend__Z6vecMulPfS_fS_iPi:\n"
    "	} // _Z6vecMulPfS_fS_iPi\n"
    "\n";
#pragma dscuda endofptx
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cutil.h>
#include <cutil_inline.h>

#pragma begin vecadd.cuh

/*
 * stub for remote call to vecAdd.
 */
void
dscudavecAdd(dim3 _gdim, dim3 _bdim, size_t _smemsize, cudaStream_t _stream , float *a, float *b, float *c)
{
    int _narg = 3;
    int _grid[3], _block[3];
    RCArg _arg[3], *_argp;
    int _off = 0;
    int _rcargc = 0;
    void *_devptr;
    static char mangledname_[512] = {0,};
    if (!mangledname_[0]) {
        if (1) {
          dscudaGetMangledFunctionName(mangledname_, __PRETTY_FUNCTION__, Ptxdata);
        }
        else {
          char buf_[256];
          sprintf(buf_, "%s", __FUNCTION__);
          strcpy(mangledname_, buf_ + strlen("dscuda")); // obtain original function name.
        }
//        WARN(3, "mangled name : %s\n", mangledname_);
    }


    // a pointer to a device-address 'dscudaAdrOfUva((void *)a)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)a);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;


    // a pointer to a device-address 'dscudaAdrOfUva((void *)b)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)b);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;


    // a pointer to a device-address 'dscudaAdrOfUva((void *)c)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)c);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;

        _grid[0] = _gdim.x; _grid[1] = _gdim.y; _grid[2] = _gdim.z;
        _block[0] = _bdim.x; _block[1] = _bdim.y; _block[2] = _gdim.z;
        dscudaLaunchKernelWrapper(dscudaLoadModule("./dscudatmp/vecadd.cu.ptx", Ptxdata), 0, mangledname_,
                                 _grid, _block, _smemsize, (RCstream)_stream,
                                 _narg, _arg);
    }
  void
vecAdd (float *a, float *b, float *c)
{
    /* nop */
}




/*
 * stub for remote call to vecMul.
 */
void
dscudavecMul(dim3 _gdim, dim3 _bdim, size_t _smemsize, cudaStream_t _stream , float *a, float *b, float c, float *d, int e, int * f)
{
    int _narg = 6;
    int _grid[3], _block[3];
    RCArg _arg[6], *_argp;
    int _off = 0;
    int _rcargc = 0;
    void *_devptr;
    static char mangledname_[512] = {0,};
    if (!mangledname_[0]) {
        if (1) {
          dscudaGetMangledFunctionName(mangledname_, __PRETTY_FUNCTION__, Ptxdata);
        }
        else {
          char buf_[256];
          sprintf(buf_, "%s", __FUNCTION__);
          strcpy(mangledname_, buf_ + strlen("dscuda")); // obtain original function name.
        }
//        WARN(3, "mangled name : %s\n", mangledname_);
    }


    // a pointer to a device-address 'dscudaAdrOfUva((void *)a)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)a);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;


    // a pointer to a device-address 'dscudaAdrOfUva((void *)b)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)b);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;


    // a float 'c'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _off = dscudaAlignUp(_off, __alignof(float));
    _argp->type = dscudaArgTypeF;
    _argp->offset = _off;
    _argp->val.floatval = c;
    _argp->size = sizeof(float);
    _off += _argp->size;


    // a pointer to a device-address 'dscudaAdrOfUva((void *)d)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)d);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;


    // an integer 'e'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _off = dscudaAlignUp(_off, __alignof(int));
    _argp->type = dscudaArgTypeI;
    _argp->offset = _off;
    _argp->val.intval = e;
    _argp->size = sizeof(int);
    _off += _argp->size;


    // a pointer to a device-address 'dscudaAdrOfUva((void *)f)'.
    _argp = _arg + _rcargc;
    _rcargc++;
    _devptr = (void*)(size_t)dscudaAdrOfUva((void *)f);
    _off = dscudaAlignUp(_off, __alignof(_devptr));
    _argp->type = dscudaArgTypeP;
    _argp->offset = _off;
    _argp->val.pointerval = (RCadr)_devptr;
    _argp->size = sizeof(_devptr);
    _off += _argp->size;

        _grid[0] = _gdim.x; _grid[1] = _gdim.y; _grid[2] = _gdim.z;
        _block[0] = _bdim.x; _block[1] = _bdim.y; _block[2] = _gdim.z;
        dscudaLaunchKernelWrapper(dscudaLoadModule("./dscudatmp/vecadd.cu.ptx", Ptxdata), 1, mangledname_,
                                 _grid, _block, _smemsize, (RCstream)_stream,
                                 _narg, _arg);
    }
  void
vecMul (float *a, float *b, float c, float *d, int e, int * f)
{
    /* nop */
}



#pragma end vecadd.cuh

#define N (8)

int
main(void)
{
    int i, t;
    float a[N], b[N], c[N];

    float *d_a, *d_b, *d_c;
    cutilSafeCall(cudaMalloc((void**) &d_a, sizeof(float) * N));
    cutilSafeCall(cudaMalloc((void**) &d_b, sizeof(float) * N));
    cutilSafeCall(cudaMalloc((void**) &d_c, sizeof(float) * N));

    for (t = 0; t < 3; t++) {
        printf("try %d\n", t);
        for (i = 0; i < N; i++) {
            a[i] = rand()%64;
            b[i] = rand()%64;
        }
        cutilSafeCall(cudaMemcpy(d_a, a, sizeof(float) * N, cudaMemcpyHostToDevice));
        cutilSafeCall(cudaMemcpy(d_b, b, sizeof(float) * N, cudaMemcpyHostToDevice));
        int nth = 4;
        dim3 threads(nth, 1, 1);
        dim3 grids((N+nth-1)/nth, 1, 1);
        dscudavecAdd(grids, threads, 0, 0, d_a, d_b, d_c);
        cutilSafeCall(cudaMemcpy(c, d_c, sizeof(float) * N, cudaMemcpyDeviceToHost));    
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
