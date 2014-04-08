#DS-CUDA SDK Template makefile #
#By Edgar J.                   #
################################

#General Paths........................
#CUDA..............................
CUDAPATH        ?= /usr/local/cuda
CUDASDKPATH     ?= $(CUDAPATH)/NVIDIA_GPU_Computing_SDK
NVCC						:= $(CUDAPATH)/bin/nvcc
#DSCUDA............................
DSCUDAPATH			?= /usr/local/DSCUDA/dscudapkg1.4.2
DSCUDASDKPATH		?= $(DSCUDAPATH)/sample
DSCUDACPP				:= $(DSCUDAPATH)/bin/dscudacpp
#Others............................
CPP							:= g++

#Libraries...........................
#CUDA........................
CUDALIB_DIR			:= -L$(CUDAPATH)/lib -L$(CUDASDKPATH)/C/lib -L$(CUDASDKPATH)/CUDALibraries/common/lib \
									 -L$(CUDASDKPATH)/shared/lib
CUDALIB					:= -lcudart -lcutil_i386 
#DSCUDA......................
DSCUDALIB_DIR		:= -L$(DSCUDAPATH)/lib
DSCUDALIB				:= -ldscuda_rpc
#Others......................
GL_LIB					:= -lglut -lGL -lGLU

#Includes............................
#CUDA
CUDAINC					:= -I$(CUDAPATH)/include -I$(CUDASDKPATH)/C/common/inc -I$(CUDASDKPATH)/shared/inc
#DSCUDA......................
DSCUDAINC				:= -I.
#Others......................
GL_INC					:= -I$(CUDASDKPATH)/shared/inc 

#Specific Flags......................
#CUDA........................
CUDA_FLAGS			:= -use_fast_math

#DSCUDA......................
CONNECT_FLAG		:= -DRPC_ONLY=1
CONNECT_LIB			:= $(DSCUDAPATH)/lib/libdscuda_rpc.a
PERF_FLAG				:= 

#Others......................
OPENMP					:= -Xcompiler -fopenmp

#Makefiles Includes..................
include $(DSCUDASDKPATH)/aux.mk

#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
#--------------------------------------
#.........GLOBAL VARIABLES.............
LIB							:= $(DSCUDALIB)  
LIBDIR					:= $(CUDALIB_DIR) $(DSCUDALIB_DIR) 
INC							:= $(CUDAINC) $(DSCUDAINC)
FLAGS						:= $(CONNECT_FLAG) $(PERF)  

###########...Main Body...#############
TARGET = direct

all: $(TARGET)_rpc $(TARGET)_MultiGPU_rpc $(TARGET)_nativCUDA

run: $(TARGET)_rpc pl2k pl8k pl64k
	./$(TARGET)_rpc pl2k pl2k.snapshot
	./$(TARGET)_rpc pl8k pl8k.snapshot
	./$(TARGET)_rpc pl8k pl64k.snapshot

run_native: $(TARGET)_nativCUDA pl2k pl8k pl64k
	./$(TARGET)_nativCUDA pl2k pl2knativ.snapshot
	./$(TARGET)_nativCUDA pl8k pl8knativ.snapshot
	./$(TARGET)_nativCUDA pl8k pl64knativ.snapshot
		

$(TARGET)_rpc : $(TARGET).cu $(CONNECT_LIB) util.o
	$(DSCUDACPP) $(FLAGS) $(INC) $(LIBDIR) -o $@ -i $< util.o $(LIB)

$(TARGET)_rpcOMP: $(TARGET).cu $(CONNECT_LIB) util.o
	$(DSCUDACPP) $(FLAGS) $(INC) $(LIBDIR) -o $@ -i $< util.o $(LIB) $(OPENMP)
	
$(TARGET)_MultiGPU_rpc:	$(TARGET)MultiGPU.cu $(CONNECT_LIB) util.o
	$(DSCUDACPP) $(FLAGS) $(INC) $(LIBDIR) -o $@ -i $< util.o $(LIB)
	
$(TARGET)_MultiGPU_rpcOMP: $(TARGET)MultiGPU.cu $(CONNECT_LIB) util.o
	$(DSCUDACPP) $(FLAGS) $(INC) $(LIBDIR) -o $@ -i $< $(LIB) util.o $(OPENMP)

$(TARGET)_nativCUDA:	$(TARGET).cu util.o
	$(NVCC) $(FLAGS) $(INC) $(CUDALIB_DIR) $< -o $@ util.o

util.o:   util.c
	$(CPP) -c -o $@ util.c

mkdist:   mkdist.c util.o
	$(CPP) -o $@ mkdist.c util.o -lm

pl2k: mkdist
	./mkdist 2048 1 pl2k
pl8k: mkdist
	./mkdist 8192 1 pl8k
pl64k:  mkdist
	./mkdist 65536 1 pl64k


clean:
	rm -rf *.o *.ptx *.txt *.snapshot $(TARGET)_* pl2k pl8k pl64k mkdist *.linkinfo ./dscudatmp

########################################

