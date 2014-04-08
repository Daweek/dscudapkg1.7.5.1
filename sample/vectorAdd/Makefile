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
TARGET = vectorAdd

all:$(TARGET)_rpc $(TARGET)_nativCUDA

$(TARGET)_rpc : $(TARGET).cu $(CONNECT_LIB)
	$(DSCUDACPP) $(FLAGS) $(INC) $(LIBDIR) -o $@ -i $< $(LIB)

$(TARGET)_nativCUDA:	$(TARGET).cu 
	$(NVCC) $(FLAGS) $(INC) $(CUDALIB_DIR) $< -o $@

clean:
	rm -rf *.o *.ptx *.txt  $(TARGET)_* *.linkinfo ./dscudatmp
########################################

