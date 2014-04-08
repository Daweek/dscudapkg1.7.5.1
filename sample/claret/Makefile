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
#Libraries...........................
#CUDA........................
CUDALIB_DIR			:= -L$(CUDAPATH)/lib64 -L$(CUDASDKPATH)/C/lib -L$(CUDASDKPATH)/CUDALibraries/common/lib \
									 -L$(CUDASDKPATH)/shared/lib
#CUDALIB					:= -lcudart -lcutil_i386 
CUDALIB					:= -lcudart -lcutil_x86_64 
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
CUDA_FLAGS			:= -DVTGRAPE -use_fast_math -O

#DSCUDA......................
CONNECT_FLAG		:= -DRPC_ONLY=1
CONNECT_LIB			:= $(DSCUDAPATH)/lib/libdscuda_rpc.a
PERF_FLAG				:= -DVTGRAPE -O -ffast-math -funroll-loops

#Others......................
OPENMP					:= -Xcompiler -fopenmp

#Makefiles Includes..................
include $(DSCUDASDKPATH)/aux.mk

#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
#--------------------------------------
#.........GLOBAL VARIABLES.............
LIB							:= $(CUDALIB) $(DSCUDALIB) $(GL_LIB) -lm -lstdc++
LIBDIR					:= $(CUDALIB_DIR) $(DSCUDALIB_DIR) 
INC							:= $(CUDAINC) $(DSCUDAINC) $(GL_INC)
FLAGS						:= $(CUDA_FLAGS)   

###########...Main Body...#############
TARGET = cras_gpu

all: $(TARGET)_rpc $(TARGET)_nativCUDA

$(TARGET)_rpc : cras36.c sockhelp.o mr3_rpc.o $(CONNECT_LIB)
	$(NVCC) $(CUDA_FLAGS) $(INC) $(LIBDIR) $(LIB) $< -o $@ mr3_rpc.o 

$(TARGET)_nativCUDA : cras36.c sockhelp.o mr3_nativ.o $(CONNECT_LIB)
	$(NVCC) $(CUDA_FLAGS) $(INC) $(LIBDIR) $(LIB) $< -o $@ mr3_nativ.o 
#---------------------------------------
mr3_rpc.o:  mr3.cu
	$(DSCUDACPP) $(CUDA_FLAGS) $(CONNECT_FLAG) $(INC) -o $@ -c -i $< 
 
mr3_nativ.o:  mr3.cu
	$(NVCC) $(CUDA_FLAGS) $(CONNECT_FLAG) $(INC) -o $@ -c $< 

cras_host : cras36.c sockhelp.o mr3_host.o
	$(CC) $(PERF_FLAG) $(INC) $< -o $@ sockhelp.o $(LIB) 

mr3_host.o : mr3_host.c
	$(CC) $(PERF_FLAG) -c $<
 
sockhelp.o : sockhelp.c
	$(CC) $(PERF_FLAG) -o $@ -c $<


clean:
	rm -rf *.o *.ptx *.txt  $(TARGET)_* *.linkinfo ./dscudatmp

########################################
