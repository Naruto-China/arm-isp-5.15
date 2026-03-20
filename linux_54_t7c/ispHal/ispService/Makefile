OUT_DIR ?= .
$(info "OUT_DIR : $(OUT_DIR)")

LOCAL_PATH := $(shell dirname $(lastword $(MAKEFILE_LIST)))
SRCS = $(shell find $(LENS_SUBDIR) -name '*.cpp')
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

ISP_ROOT_DIR := $(LOCAL_PATH)/..
ISP_ROOT_OUT_DIR := $(OUT_DIR)/..
LOCAL_INCLUDES := -I$(ISP_ROOT_DIR)/include/

TARGET := ispService

#CFLAGS := -Wall -Werror -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
CFLAGS := -Wall -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
CFLAGS += -pthread -fPIE
CFLAGS += $(LOCAL_INCLUDES)
LDFLAGS = -ldl -L$(OUT_DIR)/../media-v4l2 -lmediaAPI \
  -L$(OUT_DIR)/../sensor_calibration -lsns \
  -L$(OUT_DIR)/../lens -llens -lm

.PHONY:clean

all: $(TARGET)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $(OUT_DIR)/$@

$(TARGET): $(OBJS)
	$(CXX) $(patsubst %,$(OUT_DIR)/%,$^) -o $(OUT_DIR)/$@ $(LDFLAGS)

clean:
	rm -rf $(patsubst %,$(OUT_DIR)/%,${OBJS}) $(OUT_DIR)/$(TARGET)

$(shell mkdir -p $(OUT_DIR))
$(shell mkdir -p $(patsubst %,$(OUT_DIR)/%,$(dir $(SRCS))))