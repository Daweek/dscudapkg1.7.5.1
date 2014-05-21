################Static Library libdscuda_tcp.a
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE	:= dscuda_tcp1.7.5.1

LOCAL_CFLAGS := -O0 -g -ffast-math -funroll-loops -I. -I/usr/local/cuda/include \
-I/usr/local/cuda-4.1/NVIDIA_GPU_Computing_SDK/C/common/inc \
-I/usr/local/cuda/samples/common/inc -DTCP_ONLY=1
LOCAL_SRC_FILES :=	dscudaverb.cpp dscudautil.cpp sockutil.c libdscuda_tcp.cpp \
LOCAL_LDLIBS := -ldl -llog
include $(BUILD_STATIC_LIBRARY)
################Static Library DS-CUDA Routine

