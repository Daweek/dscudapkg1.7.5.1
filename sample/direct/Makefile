#DS-CUDA SDK Template makefile #
#By Edgar J.                   #
################################

#Makefiles Includes..................
include ../../script/aux.mk

#General Paths........................
#CUDA..............................
CUDA_PATH       ?= /usr/local/cuda
CUDA_SDKPATH    ?= $(CUDA_PATH)/samples
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
CUDA_LIB_DIR		:= 
CUDA_LIB				:= -lcudart -lcutil_$(CUTIL_SIZE) 
#DSCUDA......................
DSCUDA_LIB_DIR	:= -L$(DSCUDA_PATH)/lib
CONNECT_TCP     := $(DSCUDA_PATH)/lib/libdscuda_tcp.a
CONNECT_IBV     := $(DSCUDA_PATH)/lib/libdscuda_ibv.a

#Others......................
GL_LIB					:= -lglut -lGL -lGLU
IBV							:= -lrdmacm -libverbs -lpthread
#Includes............................
#CUDA
CUDA_INC				:= -I$(CUDA_PATH)/include -I$(CUDA_SDKPATH)/common/inc 
#DSCUDA......................
DSCUDA_INC			:= -I. -I$(DSCUDA_PATH)/include/common/inc
#Others......................
GL_INC					:= -I$(CUDA_SDKPATH)/common/inc

#Specific Flags......................
#CUDA........................
CUDA_FLAGS			:= --cudart=shared -use_fast_math

#DSCUDA......................
PERF_FLAG				:= 

#Others......................
OPENMP					:= -Xcompiler -fopenmp

#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
#--------------------------------------
#.........GLOBAL VARIABLES.............
LIB							:=   
LIBDIR					:= $(CUDA_LIB_DIR) $(DSCUDA_LIB_DIR) 
INC							:= $(CUDA_INC) $(DSCUDA_INC)
FLAGS						:= $(CUDA_FLAGS)  

###########...Main Body...#############
TARGET = direct

all: $(TARGET)_tcp

run_tcp: $(TARGET)_tcp pl2k pl8k pl64k
	./$(TARGET)_tcp pl2k pl2k.snapshot
	./$(TARGET)_tcp pl8k pl8k.snapshot
	./$(TARGET)_tcp pl8k pl64k.snapshot
run_ibv: $(TARGET)_ibv pl2k pl8k pl64k
	./$(TARGET)_ibv pl2k pl2k.snapshot
	./$(TARGET)_ibv pl8k pl8k.snapshot
	./$(TARGET)_ibv pl8k pl64k.snapshot

$(TARGET)_tcp: $(TARGET)_tcp.o util.o $(CONNECT_TCP) 
	$(NVCC) $(CUDA_FLAGS) -DTCP_ONLY=1 $(INC) $(LIBDIR) -o $@  $< util.o $(LIB) -ldscuda_tcp 

$(TARGET)_ibv: $(TARGET)_ibv.o util.o $(CONNECT_IBV) 
	$(NVCC) $(CUDA_FLAGS) $(INC) $(LIBDIR) -o $@  $< util.o $(LIB) -ldscuda_ibv $(IBV)

$(TARGET)_tcp.o:$(TARGET).cu 
	$(DSCUDA_CPP) $(FLAGS) -DTCP_ONLY=1 $(INC) $(LIBDIR) -o $@ -c -i $< 

$(TARGET)_ibv.o:$(TARGET).cu 
	$(DSCUDA_CPP) $(FLAGS) $(INC) $(LIBDIR) -o $@ -c -i $< 

util.o:   util.c
	$(GCC) -c -o $@ util.c

mkdist:   mkdist.c util.o
	$(GCC) -o $@ mkdist.c util.o -lm

pl2k: mkdist
	./mkdist 2048 1 pl2k
pl8k: mkdist
	./mkdist 8192 1 pl8k
pl64k:  mkdist
	./mkdist 65536 1 pl64k

clean:
	rm -rf *.o *.ptx *.snapshot *.txt  $(TARGET)_* pl2k pl8k pl64k mkdist *.linkinfo ./dscudatmp

########################################

