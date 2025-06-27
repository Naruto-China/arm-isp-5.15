OUT_DIR ?= .
$(info "OUT_DIR : $(OUT_DIR)")

LENS_SUBDIR := $(shell dirname $(lastword $(MAKEFILE_LIST)))
LENS_SUBDIR_DTS := $(LENS_SUBDIR)/dw9714
SRCS = $(shell find $(LENS_SUBDIR) -name '*.c')
OBJS = $(patsubst %.c,%.o,$(SRCS))

#LOCAL_PATH := $(shell dirname $(lastword $(MAKEFILE_LIST)))

ISP_ROOT_DIR := $(LENS_SUBDIR)/..
LOCAL_INCLUDES := -I$(ISP_ROOT_DIR)/include/
LOCAL_INCLUDES += -I$(LENS_SUBDIR_DTS)

CFLAGS += -I ./include/
CFLAGS += -I ./kernel-headers/
CFLAGS += $(LOCAL_INCLUDES)

LIBLENS = liblens.so


.PHONY: clean

# rules
all: $(LIBLENS)

%.o: %.c
#%.o: $(TUNING_SUBDIR) %.cpp
	$(CC) -c -fPIC  $(CFLAGS) $^ -o $(OUT_DIR)/$@

$(LIBLENS): $(OBJS)
	$(CC) -shared  -Wl,-soname,$(LIBLENS) -fPIC $(CFLAGS) $(patsubst %,$(OUT_DIR)/%,$^) -o $(OUT_DIR)/$(LIBLENS)

clean:
	rm -f $(patsubst %,$(OUT_DIR)/%,$(OBJS)) $(OUT_DIR)/$(LIBLENS)

$(shell mkdir -p $(OUT_DIR))
$(shell mkdir -p $(patsubst %,$(OUT_DIR)/%,$(dir $(SRCS))))