LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libretro
LOCAL_CFLAGS :=

ifeq ($(TARGET_ARCH),arm)
LOCAL_CPPFLAGS += -DANDROID_ARM
endif

ifeq ($(TARGET_ARCH),x86)
LOCAL_CPPFLAGS +=  -DANDROID_X86
endif

ifeq ($(TARGET_ARCH),mips)
LOCAL_CPPFLAGS += -DANDROID_MIPS -D__mips__ -D__MIPSEL__
endif

LOCAL_SRC_FILES = ../audio.cpp ../cpu.cpp ../vmu.cpp ../main.cpp ../video.cpp ../t0.cpp ../t1.cpp ../basetimer.cpp ../bitwisemath.cpp ../flashfile.cpp ../flash.cpp ../rom.cpp ../ram.cpp ../interrupts.cpp
LOCAL_CPPFLAGS += -O3 -std=c++98 -ffast-math -funroll-loops


include $(BUILD_SHARED_LIBRARY)
