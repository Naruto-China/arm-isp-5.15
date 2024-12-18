OUT_DIR ?= .
$(info "OUT_DIR : $(OUT_DIR)")

TUNING_SUBDIR := $(shell dirname $(lastword $(MAKEFILE_LIST)))
TUNING_SUBDIR_DTS := $(TUNING_SUBDIR)/imx290
TUNING_SUBDIR_DTS_1 := $(TUNING_SUBDIR)/ov08a10
SRCS = $(shell find $(TUNING_SUBDIR) -name '*.cpp')
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

#LOCAL_PATH := $(shell dirname $(lastword $(MAKEFILE_LIST)))

ISP_ROOT_DIR := $(TUNING_SUBDIR)/..
LOCAL_INCLUDES := -I$(ISP_ROOT_DIR)/include/
LOCAL_INCLUDES += -I$(TUNING_SUBDIR_DTS)
LOCAL_INCLUDES += -I$(TUNING_SUBDIR_DTS_1)

CFLAGS += -I ./include/
CFLAGS += -I ./kernel-headers/
CFLAGS += $(LOCAL_INCLUDES)

LIBTUNING = libtuning.so


.PHONY: clean

# rules
all: $(LIBTUNING)
	@echo "CXX is: $(CXX)"

%.o: %.cpp
#%.o: $(TUNING_SUBDIR) %.cpp
	$(CC) -c -fPIC  $(CFLAGS) $^ -o $(OUT_DIR)/$@

$(LIBTUNING): $(OBJS)
	$(CXX) -shared  -Wl,-soname,$(LIBTUNING) -fPIC $(CFLAGS) $(patsubst %,$(OUT_DIR)/%,$^) -o $(OUT_DIR)/$(LIBTUNING)

clean:
	rm -f $(patsubst %,$(OUT_DIR)/%,$(OBJS)) $(OUT_DIR)/$(LIBTUNING)

$(shell mkdir -p $(OUT_DIR))
$(shell mkdir -p $(patsubst %,$(OUT_DIR)/%,$(dir $(SRCS))))