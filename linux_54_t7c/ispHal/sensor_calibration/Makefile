OUT_DIR ?= .
$(info "OUT_DIR : $(OUT_DIR)")

SRC_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
INC_DIR := $(SRC_DIR)/../include
SRCS := $(wildcard $(SRC_DIR)/*.c \
                   $(SRC_DIR)/imx290/*.c \
                   $(SRC_DIR)/imx415/*.c \
                   $(SRC_DIR)/ov08a10/*.c \
                   $(SRC_DIR)/imx585/*.c \
                   $(SRC_DIR)/imx678/*.c)

OBJDIR = $(OUT_DIR)/objs
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

TARGETLIB_SO = $(OUT_DIR)/libsns.so

CFLAGS = -Wall -fPIC -Werror -g -O2
CFLAGS += -I $(INC_DIR) \
  -I $(SRC_DIR)/imx290 \
  -I $(SRC_DIR)/imx415 \
  -I $(SRC_DIR)/ov08a10 \
  -I $(SRC_DIR)/imx585 \
  -I $(SRC_DIR)/imx678

LDFLAGS += -lm -shared

.PHONY: all clean

# rules
all: $(TARGETLIB_SO)

$(TARGETLIB_SO): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRC_DIR)/%.c
	@echo "[CC] $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGETLIB_SO)

$(shell mkdir -p $(OUT_DIR))
