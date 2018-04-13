LOCAL_PATH := $(call my-dir)

CORE_DIR := $(LOCAL_PATH)/..

SOURCES_CPP := $(wildcard $(CORE_DIR)/*.cpp)

COREFLAGS := -ffast-math -funroll-loops

include $(CLEAR_VARS)
LOCAL_MODULE    := retro
LOCAL_SRC_FILES := $(SOURCES_CPP)
LOCAL_CPPFLAGS  := -std=c++98 $(COREFLAGS)
LOCAL_LDFLAGS   := -Wl,-version-script=$(CORE_DIR)/link.T
include $(BUILD_SHARED_LIBRARY)
