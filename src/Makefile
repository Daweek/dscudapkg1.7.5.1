# General Makefile to manage DSCUDA libraries and binaries
# By Dr. Kawai and Edgar J.
#Makefiles Includes..................
include ../script/aux.mk

#General Paths........................
#CUDA..............................
CUDA_PATH       ?= /usr/local/cuda
CUDA_SDKPATH    ?= $(CUDA_PATH)/samples
NVCC            ?= $(CUDA_PATH)/bin/nvcc

#DSCUDA............................
DSCUDA_PATH     ?= /usr/local/DSCUDA/dscudapkg$(DSCUDA_VER)
DSCUDA_SDKPATH  ?= $(DSCUDA_PATH)/sample
DSCUDA_CPP      ?= $(DSCUDA_PATH)/bin/dscudacpp
#Others............................
CXX             := g++ 
CC				:= gcc
#Libraries...........................
ifeq ($(OS_SIZE),64)
  OS_SIZE_LIB   := lib64
  CUTIL_SIZE    := x86_64
endif
ifeq ($(OS_SIZE),32)
  OS_SIZE_LIB   := lib 
  CUTIL_SIZE    := i386 
endif

#CUDA........................
CUDA_LIB_DIR    := -L$(CUDA_PATH)/$(OS_SIZE_LIB) -L$(CUDA_SDKPATH)/C/lib -L$(CUDA_SDKPATH)/CUDALibraries/common/lib \
                -L$(CUDA_SDKPATH)/shared/lib
CUDA_LIB        := -lcudart -lcutil_$(CUTIL_SIZE) 
#DSCUDA......................
DSCUDA_LIB_DIR  := -L$(DSCUDA_PATH)/lib
DSCUDA_LIB      := -ldscuda_
#Others......................
GL_LIB          := -lglut -lGL -lGLU

#Includes............................
#CUDA
CUDA_INC        := -I. -I$(CUDA_PATH)/include -I$(CUDA_SDKPATH)/common/inc -I$(CUDA_SDKPATH)/shared/inc
#CUDA_INC       := -I. -I$(CUDA_PATH)/include -I$(DSCUDA_PATH)/include/common/inc
#DSCUDA......................
DSCUDA_INC      := -I.
DSCUDA_HDR			:= dscuda.h dscudadefs.h dscudamacros.h 
CLIENTBASE     	:= dscudaverb.h libdscuda.h libdscuda.cu dscudaverb.o
#Others......................
GL_INC          := -I$(CUDA_SDKPATH)/shared/inc

#Specific Flags......................
#C and C++ Flags.............
PERF_C			:= -O0 -g -ffast-math -funroll-loops
#CUDA........................
CUDA_FLAGS      := -use_fast_math
PICOPT			:= -Xcompiler -fPIC
#DSCUDA......................
CONNECT_FLAG    := -DRPC_ONLY=1
CONNECT_LIB     := $(DSCUDA_PATH)/lib/libdscuda_tcp.a
PERF_FLAG       :=

#Others......................
OPENMP          := -Xcompiler -fopenmp
BINCOMPAT				:= -DBINCOMPATIBLE=1
#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
#--------------------------------------
#.........GLOBAL VARIABLES.............
LIB             := $(DSCUDA_LIB)
LIBDIR          := $(CUDA_LIB_DIR) $(DSCUDA_LIB_DIR)
INC             := $(CUDA_INC) $(DSCUDA_INC)
FLAGS           := $(CONNECT_FLAG) $(PERF_FLAG)

############################################################################
#Targets................................
TARGET         	:= dscudad_ibv dscudasvr_ibv libdscuda_ibv.a libdscuda_tcp.a libcudart.so.3 libdscuda_ibv.so libdscuda_tcp.so
TARGET_TCPONLY 	:= dscudad_tcp dscudasvr_tcp libdscuda_tcp.a libcudart.so.3
DSCUDAHDR      	:= dscuda.h dscudadefs.h dscudamacros.h
CLIENTBASE     	:= dscudaverb.h libdscuda.h libdscuda.cu dscudaverb.o
CLIENTBASEANDR 	:= dscudaverb.h libdscuda.h libdscuda.cu dscudaverb_andr.o

default :	$(TARGET)

tcponly:	$(TARGET_TCPONLY)
############################################################################
#Binaries..............................

dscudad_ibv :	dscudad.c sockutil.c sockutil.h
	$(CC) $(PERF_C) -o $@ dscudad.c sockutil.c -lpthread

dscudad_tcp:	dscudad.c sockutil.c sockutil.h
	$(CC) $(PERF_C) -DTCP_ONLY=1 -o $@ dscudad.c sockutil.c -lpthread

dummysvr :	dummysvr.c

dummyclient :	dummyclient.c

dscudasvr_ibv :	dscudasvr.cu dscudasvr_ibv.cu dscudasvr_tcp.cu $(DSCUDA_HDR) \
                dscudautil.o ibvutil.o sockutil.o
	$(NVCC) $(CUDA_INC) $(SVROPT) -c -o $@.o dscudasvr.cu
	$(NVCC) $(CUDA_LIB_DIR) $(SVROPT) -o $@ $@.o dscudautil.o ibvutil.o  sockutil.o \
                -lcufft -lcudart -lcuda -lrdmacm -libverbs -lpthread

dscudasvr_tcp:	dscudasvr.cu dscudasvr_tcp.cu $(DSCUDA_HDR) \
                dscudautil.o sockutil.o
	$(NVCC) $(CUDA_INC) -DTCP_ONLY=1 -c -o $@.o dscudasvr.cu
	$(NVCC) $(CUDA_LIB_DIR) -o $@ $@.o dscudautil.o  sockutil.o \
                -lcufft -lcudart -lcuda -lpthread

dscudatest :	dscudatest.cu libdscuda_ibv.so
	$(NVCC) $(CUDA_INC) -o $@ $< -L../lib -ldscuda_ibv -lrdmacm -libverbs -lpthread

############################################################################
#Libraries....................

libdscuda_ibv.so :	$(CLIENTBASE) libdscuda_ibv.cu $(DSCUDA_HDR) \
                        dscudautil.o ibvutil.o sockutil.o
	$(NVCC) $(PICOPT) $(BINCOMPAT) $(CUDA_INC) -c -o libdscuda_ibv.o libdscuda_ibv.cu
	$(CXX) $(PERF_C) -shared -Wl,-soname,libdscuda.so -o $@ libdscuda_ibv.o dscudautil.o \
               ibvutil.o dscudaverb.o sockutil.o -lrdmacm -libverbs -lpthread

libdscuda_tcp.so :	$(CLIENTBASE) libdscuda_tcp.cu $(DSCUDAHDR) \
                        dscudautil.o sockutil.o
	$(NVCC) $(PICOPT) $(BINCOMPAT) $(CUDA_INC) -c -o libdscuda_tcp.o libdscuda_tcp.cu
	$(CXX) $(PERF_C) -shared -Wl,-soname,libdscuda.so -o $@ libdscuda_tcp.o \
               dscudautil.o dscudaverb.o sockutil.o

libdscuda_ibv.a :	$(CLIENTBASE) libdscuda_ibv.cu $(DSCUDA_HDR) \
                        dscudautil.o ibvutil.o sockutil.o
	$(NVCC) $(CUDA_INC) -c -o libdscuda_ibv.o libdscuda_ibv.cu
	ar rvu $@ libdscuda_ibv.o dscudautil.o ibvutil.o dscudaverb.o sockutil.o
	ranlib $@

libdscuda_tcp.a :	$(CLIENTBASE) libdscuda_tcp.cu $(DSCUDA_HDR) \
                        dscudautil.o sockutil.o
	$(NVCC) $(CUDA_INC) -DTCP_ONLY=1 -c -o libdscuda_tcp.o libdscuda_tcp.cu
	ar rvu $@ libdscuda_tcp.o dscudautil.o dscudaverb.o  sockutil.o
	ranlib $@
	
libdscuda_tcp_andr.a :	$(CLIENTBASEANDR) libdscuda_tcp.cpp $(DSCUDA_HDR) \
                        dscudautil_andr.o sockutil_andr.o
	$(CXX) $(CUDA_INC) -DTCP_ONLY=1 -c -o libdscuda_tcp_andr.o libdscuda_tcp.cpp
	ar rvu $@ libdscuda_tcp_andr.o dscudautil_andr.o dscudaverb_andr.o  sockutil_andr.o
	ranlib $@

# A dummy library to override CUDA runtime library functions.
libcudart.so.3:	cudart_dummy.cu
	$(NVCC) $(CUDA_INC) -shared -Xcompiler -fPIC -o $@ $<
############################################################################
#Objects........................
dscudaverb.o:	dscudaverb.cu
	$(NVCC) $(PICOPT) $(CUDA_INC) -DTCP_ONLY=1 -c -o $@ $<

dscudautil.o:	dscudautil.cu
	$(NVCC) $(PICOPT) $(CUDA_INC) -c -o $@ $<

sockutil.o:	sockutil.c
	$(NVCC) $(PICOPT) $(CUDA_INC) -c -o $@ $<

ibvutil.o:	ibvutil.cpp ibvdefs.h
	$(CXX) $(PERF_C) $(CUDA_INC) -c -o $@ $<
#For android.....................
dscudaverb_andr.o:	dscudaverb.cpp
	$(CXX) $(CUDA_INC) -DTCP_ONLY=1 -c -o $@ $<

dscudautil_andr.o:	dscudautil.cpp
	$(CXX) $(CUDA_INC) -c -o $@ $<

sockutil_andr.o:	sockutil.c
	$(CXX) $(CUDA_INC) -c -o $@ $<
############################################################################
#Others.........................

clean:
	/bin/rm $(TARGET) $(TARGET_TCPONLY) *.o *~ *.linkinfo
