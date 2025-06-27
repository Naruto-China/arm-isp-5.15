OUT_DIR ?= .
$(info "OUT_DIR : $(OUT_DIR)")

MEDIA_API_LOCAL_PATH := $(shell dirname $(lastword $(MAKEFILE_LIST)))
SRCS = $(shell find $(MEDIA_API_LOCAL_PATH) -name '*.c')
OBJS = $(patsubst %.c,%.o,$(SRCS))


ISP_ROOT_DIR := $(MEDIA_API_LOCAL_PATH)/..
LOCAL_INCLUDES := -I$(ISP_ROOT_DIR)/include/

CFLAGS += $(LOCAL_INCLUDES)

LIBMEDIA_API = libmediaAPI.so

.PHONY: clean

# rules
all: $(LIBMEDIA_API)

%.o: %.c
	$(CC) -c -fPIC  $(CFLAGS) $^ -o $(OUT_DIR)/$@

$(LIBMEDIA_API): $(OBJS)
	$(CC) -shared  -Wl,-soname,$(LIBMEDIA_API) -fPIC $(patsubst %,$(OUT_DIR)/%,$^) -o $(OUT_DIR)/$(LIBMEDIA_API)

clean:
	rm -f $(patsubst %,$(OUT_DIR)/%,$(OBJS)) $(OUT_DIR)/$(LIBMEDIA_API)

$(shell mkdir -p $(OUT_DIR))
$(shell mkdir -p $(patsubst %,$(OUT_DIR)/%,$(dir $(SRCS))))