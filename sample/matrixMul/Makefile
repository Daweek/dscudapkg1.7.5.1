#DS-CUDA SDK Template makefile #
#By Edgar J.                   #
################################

#Makefiles Includes..................
#include $(DSCUDA_PATH)/script/aux.mk
include /usr/local/DSCUDA/dscudapkg1.7.5.1/script/aux.mk

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
DSCUDA_LIB_TYPE	?= dscuda_tcp
DSCUDA_LIB_DIR	:= -L$(DSCUDA_PATH)/lib
DSCUDA_LIB			:= -l$(DSCUDA_LIB_TYPE)
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
CUDA_FLAGS			:= -use_fast_math

#DSCUDA......................
CONNECT_LIB			:= $(DSCUDA_PATH)/lib/lib$(DSCUDA_LIB_TYPE).a
PERF_FLAG				:= 

#Others......................
OPENMP					:= -Xcompiler -fopenmp

#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
#--------------------------------------
#.........GLOBAL VARIABLES.............
LIB							:= $(DSCUDA_LIB)  
LIBDIR					:= $(CUDA_LIB_DIR) $(DSCUDA_LIB_DIR) 
INC							:= $(CUDA_INC) $(DSCUDA_INC)
FLAGS						:= $(PERF_FLAG)  

###########...Main Body...#############
TARGET = matrixMul

all: $(TARGET)_tcp

$(TARGET)_tcp: $(TARGET).cu $(CONNECT_LIB)
	$(DSCUDA_CPP) $(FLAGS) -DTCP_ONLY=1 $(INC) $(LIBDIR) -o $@ -i $< $(LIB)

$(TARGET)_ibv: $(TARGET).cu $(CONNECT_LIB)
	$(DSCUDA_CPP) $(FLAGS) -DTCP_ONLY=0 $(INC) $(LIBDIR) -o $@ -i $< $(LIB) 

clean:
	rm -rf *.o *.ptx *.txt  $(TARGET)_* *.linkinfo ./dscudatmp

########################################

