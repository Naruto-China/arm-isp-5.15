OUT_DIR ?= .
$(info "OUT_DIR : $(OUT_DIR)")

LOCAL_PATH := $(shell dirname $(lastword $(MAKEFILE_LIST)))
SRCS = $(shell find $(LENS_SUBDIR) -name '*.cpp')
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

ISP_ROOT_DIR := $(LOCAL_PATH)/..
ISP_ROOT_OUT_DIR := $(OUT_DIR)/..
LOCAL_INCLUDES := -I$(ISP_ROOT_DIR)/include/
LOCAL_INCLUDES += -I$(ISP_ROOT_DIR)/sensor_calibration/imx290/
LOCAL_INCLUDES += -I$(ISP_ROOT_DIR)/sensor_calibration/ov08a10/
LOCAL_INCLUDES += -I$(ISP_ROOT_DIR)/sensor_calibration/imx415/
LOCAL_INCLUDES += -I$(ISP_ROOT_DIR)/lens/dw9714/

TARGET := v4l2_test_raw

#CFLAGS := -Wall -Werror -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
CFLAGS := -Wall -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
CFLAGS += $(LOCAL_INCLUDES)
CFLAGS += -I./external -pthread -fPIE
CFLAGS += -L$(ISP_ROOT_DIR)/firmware/${ARM_TARGET}/ -lispaml -ldl -L$(ISP_ROOT_OUT_DIR)/media-v4l2 -lmediaAPI -L$(ISP_ROOT_OUT_DIR)/sensor_calibration -ltuning -L$(ISP_ROOT_OUT_DIR)/lens -llens -lm
CFLAGS += -fPIC -D_FORTIFY_SOURCE=2 -O3

#CFLAGS += -L./external -lH264VideoStreamer -lx264 -lcrypto -lssl

.PHONY:clean

all: $(TARGET)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $(OUT_DIR)/$@

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(patsubst %,$(OUT_DIR)/%,$^) -o $(OUT_DIR)/$@

clean:
	rm -rf $(patsubst %,$(OUT_DIR)/%,${OBJS}) $(OUT_DIR)/$(TARGET)

$(shell mkdir -p $(OUT_DIR))
$(shell mkdir -p $(patsubst %,$(OUT_DIR)/%,$(dir $(SRCS))))