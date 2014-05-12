#DS-CUDA SDK Template makefile #
#By Edgar J.                   #
################################

#Makefiles Includes..................
include ../../script/aux.mk

#General Paths........................
#CUDA..............................
CUDA_PATH       ?= /usr/local/cuda
CUDA_SDKPATH    ?= $(CUDA_PATH)/samples
NVCC			?= $(CUDA_PATH)/bin/nvcc
#DSCUDA............................
DSCUDA_PATH		?= /usr/local/DSCUDA/dscudapkg$(DSCUDA_VER)
DSCUDA_SDKPATH	?= $(DSCUDA_PATH)/sample
DSCUDA_CPP		?= $(DSCUDA_PATH)/bin/dscudacpp
#Others............................
CPP				:= g++

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
CUDA_LIB			:= -lcudart -lcutil_$(CUTIL_SIZE) 
#DSCUDA......................
DSCUDA_LIB_DIR	:= -L$(DSCUDA_PATH)/lib
CONNECT_TCP     := $(DSCUDA_PATH)/lib/libdscuda_tcp.a
CONNECT_IBV     := $(DSCUDA_PATH)/lib/libdscuda_ibv.a
GL_LIB_DIR		:= -L/usr/local/lib 

#Others......................
GL_LIB						:= -lGL -lGLU -lglut
IBV							:= -lrdmacm -libverbs -lpthread
#Includes............................
#CUDA
CUDA_INC				:= -I$(CUDA_PATH)/include -I$(CUDA_SDKPATH)/common/inc 
#DSCUDA......................
DSCUDA_INC			:= -I. -I$(DSCUDA_PATH)/include/common/inc
#Others......................
GL_INC					:= -I/usr/local/include/GL
#-I$(CUDA_SDKPATH)/common/inc 

#Specific Flags......................
#CUDA........................
CUDA_FLAGS			:= --cudart=shared -DVTGRAPE -O -use_fast_math

#DSCUDA......................
PERF_FLAG				:= -DVTGRAPE -O0 -ffast-math -funroll-loops  

#Others......................
OPENMP					:= -Xcompiler -fopenmp

#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
#--------------------------------------
#.........GLOBAL VARIABLES.............
LIB							:= $(GL_LIB)  
LIBDIR						:= $(CUDA_LIB_DIR) $(DSCUDA_LIB_DIR) $(GL_LIB_DIR)
INC							:= $(CUDA_INC) $(DSCUDA_INC) $(GL_INC)
FLAGS						:=   

###########...Main Body...#############
TARGET = cras_gpu

all: $(TARGET)_tcp

$(TARGET)_tcp: cras36.c sockhelp.o mr3_tcp.o $(CONNECT_TCP)
	$(NVCC) $(CUDA_FLAGS) $(INC) $(LIBDIR) $(LIB) $< -o $@ mr3_tcp.o -ldscuda_tcp

$(TARGET)_tcp_android: cras36.c sockhelp.o mr3_tcp_android.o $(DSCUDA_PATH)/lib/libdscuda_tcp_andr.a
	gcc -DVTGRAPE -DTCP_ONLY=1 -O0 -g -P -ffast-math -funroll-loops $(INC) $(LIBDIR) \
	 $< -o $@ $(LIB) mr3_tcp_android.o -ldscuda_tcp_andr \
	-lm

$(TARGET)_ibv: cras36.c sockhelp.o mr3_ibv.o $(CONNECT_IBV)
	$(NVCC) $(CUDA_FLAGS) $(INC) $(LIBDIR) $(LIB) $< -o $@ mr3_ibv.o -ldscuda_ibv $(IBV)
#---------------------Objects
mr3_tcp.o:  mr3.cu
	$(DSCUDA_CPP) $(PERF_FLAGS) -DTCP_ONLY=1 $(INC) -o $@ -c -i $< 

mr3_tcp_android.o:  mr3.cu
	$(DSCUDA_CPP) --cpp $(PERF_FLAGS) -DTCP_ONLY=1 $(INC) -o $@ -c -i $< 
 
mr3_ibv.o:  mr3.cu
	$(DSCUDA_CPP) $(PERF_FLAGS) -DTCP_ONLY=0 $(INC) -o $@ -c -i $< 

cras_host : cras36.c sockhelp.o mr3_host.o
	$(CPP) $(PERF_FLAG) $(INC) $< -o $@ sockhelp.o $(LIB) 

mr3_host.o : mr3_host.c
	$(CPP) $(PERF_FLAG) -c $<
 
sockhelp.o : sockhelp.c

	$(CC) $(PERF_FLAG) -o $@ -c $<
clean:
	rm -rf *.o *.ptx *.txt  $(TARGET)_* *.linkinfo ./dscudatmp

########################################

