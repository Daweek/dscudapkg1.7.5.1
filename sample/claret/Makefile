#DS-CUDA SDK Template makefile #
#By Edgar J.                   #
################################

#Makefiles Includes..................
include ../../script/aux.mk

#General Paths........................
#CUDA..............................
CUDA_PATH       ?= /usr/local/cuda
CUDA_SDKPATH    ?= $(CUDA_PATH)/NVIDIA_GPU_Computing_SDK
NVCC						?= $(CUDA_PATH)/bin/nvcc
#DSCUDA............................
DSCUDA_PATH			?= /usr/local/DSCUDA/dscudapkg$(DSCUDA_VER)
DSCUDA_SDKPATH	?= $(DSCUDA_PATH)/sample
DSCUDA_CPP			?= $(DSCUDA_PATH)/bin/dscudacpp
#Others............................
CPP							?= g++

#Libraries...........................
ifeq ($(OS_SIZE),64)
	OS_SIZE_LIB 	:= lib64
	CUTIL_SIZE		:= x86_64
endif
ifeq ($(OS_SIZE),32)
	OS_SIZE_LIB 	:= lib
	CUTIL_SIZE		:= i386 
endif

#CUDA........................
CUDA_LIB_DIR		:= -L$(CUDA_PATH)/$(OS_SIZE_LIB) -L$(CUDA_SDKPATH)/C/lib -L$(CUDA_SDKPATH)/CUDALibraries/common/lib \
								-L$(CUDA_SDKPATH)/shared/lib
CUDA_LIB				:= -lcudart -lcutil_$(CUTIL_SIZE) 
#DSCUDA......................
DSCUDA_LIB_TYPE	?= tcp
DSCUDA_LIB_DIR	:= -L$(DSCUDA_PATH)/lib
DSCUDA_LIB			:= -ldscuda_$(DSCUDA_LIB_TYPE)
#Others......................
GL_LIB					:= -lglut -lGL -lGLU

#Includes............................
#CUDA
CUDA_INC				:= -I$(CUDA_PATH)/include -I$(CUDA_SDKPATH)/C/common/inc -I$(CUDA_SDKPATH)/shared/inc
#DSCUDA......................
DSCUDA_INC			:= -I.
#Others......................
GL_INC					:= -I$(CUDA_SDKPATH)/shared/inc 

#Specific Flags......................
#CUDA........................
CUDA_FLAGS			:= -DVTGRAPE -O -use_fast_math

#DSCUDA......................
CONNECT_LIB			:= $(DSCUDA_PATH)/lib/libdscuda_$(DSCUDA_LIB_TYPE).a
PERF_FLAG				:= -DVTGRAPE -O0 -ffast-math -funroll-loops  

#Others......................
OPENMP					:= -Xcompiler -fopenmp

#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
#--------------------------------------
#.........GLOBAL VARIABLES.............
LIB							:= $(DSCUDA_LIB) $(GL_LIB)  
LIBDIR					:= $(CUDA_LIB_DIR) $(DSCUDA_LIB_DIR) 
INC							:= $(CUDA_INC) $(DSCUDA_INC) $(GL_INC)
FLAGS						:= $(PERF_FLAG)  

###########...Main Body...#############
TARGET = cras_gpu

all: $(TARGET)_tcp

$(TARGET)_tcp: cras36.c sockhelp.o mr3_tcp.o $(CONNECT_LIB)
	$(NVCC) $(CUDA_FLAGS) $(INC) $(LIBDIR) $(LIB) $< -o $@ mr3_tcp.o

$(TARGET)_ibv: cras36.c sockhelp.o mr3_ibv.o $(CONNECT_LIB)
	$(NVCC) $(CUDA_FLAGS) $(INC) $(LIBDIR) $(LIB) $< -o $@ mr3_ibv.o
#---------------------Objects
mr3_tcp.o:  mr3.cu
	$(DSCUDA_CPP) $(PERF_FLAGS) -DTCP_ONLY=1 $(INC) -o $@ -c -i $< 
 
mr3_ibv.o:  mr3.cu
	$(DSCUDA_CPP) $(PERF_FLAGS) -DTCP_ONLY=0 $(INC) -o $@ -c -i $< 

cras_host : cras36.c sockhelp.o mr3_host.o
	$(CC) $(PERF_FLAG) $(INC) $< -o $@ sockhelp.o $(LIB) 

mr3_host.o : mr3_host.c
	$(CC) $(PERF_FLAG) -c $<
 
sockhelp.o : sockhelp.c
	$(CC) $(PERF_FLAG) -o $@ -c $<

clean:
	rm -rf *.o *.ptx *.txt  $(TARGET)_* *.linkinfo ./dscudatmp

########################################

