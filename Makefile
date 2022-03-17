#/**
# * Copyright 2021 Brian Costabile
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy
# * of this software and associated documentation files (the "Software"), to deal
# * in the Software without restriction, including without limitation the rights
# * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# * copies of the Software, and to permit persons to whom the Software is
# * furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in
# * all copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# * THE SOFTWARE.
# */

# Detect the build machine OS and architecture
# Contribution from:
# https://stackoverflow.com/questions/714100/os-detecting-makefile
ifeq ($(OS),Windows_NT)
    DFLAGS += WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        DFLAGS += AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            DFLAGS += AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            DFLAGS += IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        DFLAGS += LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        DFLAGS += OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        DFLAGS += AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        DFLAGS += IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        DFLAGS += ARM
    endif
endif

# Product and Build options typically passed in on command line
ifndef PRODUCT
PRODUCT := TivaBspCore
endif

# gcc or ccs
ifndef COMPILER
COMPILER := gcc
endif
COMPILER_UPPER := $(shell echo $(COMPILER) | tr a-z A-Z)

# 1=Verbose 0=Silent
ifndef VERBOSE
VERBOSE := 0
endif

# 1 = build and use built newlib
# 0 = use newlib from toolchain
# note: newlib is only used for gcc build
ifeq ($(COMPILER),gcc)
ifndef BUILD_NEWLIB
BUILD_NEWLIB := 0
endif
else
BUILD_NEWLIB := 0  #override command line
endif

ifndef TOOLS_ROOT_DIR
TOOLS_ROOT_DIR := /Applications/ti/ccs1040/ccs/tools/compiler
endif

# Get the root dir where the makefile is
ROOT_DIR        := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR       := $(ROOT_DIR)/build/$(PRODUCT)/$(COMPILER)
SRC_DIR         := $(ROOT_DIR)/source
PRJ_DIR         := $(ROOT_DIR)/project
THIRD_PARTY_DIR := $(ROOT_DIR)/third_party

# Pull from product config makefile
include ./project/config/product/$(PRODUCT).mk
include ./project/config/toolchain/$(COMPILER).mk

CC  := $(TOOLCHAIN_CC)
AR  := $(TOOLCHAIN_AR)
AS  := $(TOOLCHAIN_AS)
LD  := $(TOOLCHAIN_LD)
D   := $(TOOLCHAIN_DEFINE)
I   := $(TOOLCHAIN_INCLUDE)

ifeq ($(VERBOSE),1)
Q :=
else
Q := @
endif

C_OPT := $(TOOLCHAIN_COMPILE_FLAG)
O_OPT := $(TOOLCHAIN_OUTPUT_FLAG)
L_OPT := $(TOOLCHAIN_LINK_FLAG)

# Function to convert list of source files (.asm, .s, .c) into a list of objects .o
define objs_from_srcs
    $(foreach src,$(1),$(BUILD_DIR)/$(subst $(ROOT_DIR)/,,$(basename $(src)).o))
endef

# Function to convert list of object files .o into a list of dependency files .d
define deps_from_objs
    $(foreach obj,$(1),$(basename $(obj)).d)
endef

####
# Third Party Libraries
#

################
# FreeRTOS
FREERTOS_NAME     := FreeRTOS
FREERTOS_VER      := v202107.00
FREERTOS_DIR      := $(THIRD_PARTY_DIR)/$(FREERTOS_NAME)/$(FREERTOS_NAME)$(FREERTOS_VER)/$(FREERTOS_NAME)
FREERTOS_SRC_DIR  := $(FREERTOS_DIR)/Source
FREERTOS_SRC_DIRS := $(FREERTOS_SRC_DIR)
FREERTOS_SRC_DIRS += $(FREERTOS_SRC_DIR)/portable/$(COMPILER_UPPER)/ARM_CM4F
FREERTOS_LIB      := $(BUILD_DIR)/$(FREERTOS_NAME).a
FREERTOS_IFLAGS   := $(FREERTOS_SRC_DIRS) $(FREERTOS_SRC_DIR)/include
FREERTOS_DFLAGS   :=

FREERTOS_CSRC     := $(foreach dir,$(FREERTOS_SRC_DIRS),$(wildcard $(dir)/*.c))
FREERTOS_ASRC     := $(foreach dir,$(FREERTOS_SRC_DIRS),$(wildcard $(dir)/*.s)) \
                     $(foreach dir,$(FREERTOS_SRC_DIRS),$(wildcard $(dir)/*.asm))

FREERTOS_SRCS     := $(FREERTOS_CSRC) $(FREERTOS_ASRC)
FREERTOS_OBJS     := $(call objs_from_srcs,$(FREERTOS_SRCS))
FREERTOS_DEPS     := $(call deps_from_objs,$(FREERTOS_OBJS))

FREERTOS_CFLAGS   := $(TOOLCHAIN_CFLAGS) \
                    $(foreach flag,$(TOOLCHAIN_IFLAGS),$(I)"$(flag)") \
                    $(foreach flag,$(FREERTOS_IFLAGS),$(I)"$(flag)") \
                     -I"$(SRC_DIR)/config"

################
# TI TivaWare
TIVAWARE_NAME   := TivaWare_C_Series
TIVAWARE_VER    := 2.2.0.295
TIVAWARE_DIR    := $(THIRD_PARTY_DIR)/TI/$(TIVAWARE_NAME)-$(TIVAWARE_VER)

PROCESSOR_UPPER := $(shell echo $(PROCESSOR) | tr a-z A-Z)
VARIANT_UPPER   := $(shell echo $(VARIANT) | tr a-z A-Z)
REVISION_UPPER  := $(shell echo $(REVISION) | tr a-z A-Z)

TIVAWARE_DFLAGS := PART_$(PROCESSOR_UPPER)$(VARIANT_UPPER)
TIVAWARE_DFLAGS += TARGET_IS_$(PROCESSOR_UPPER)_$(REVISION_UPPER)
TIVAWARE_DFLAGS += $(COMPILER)
TIVAWARE_IFLAGS := $(TIVAWARE_DIR) $(TIVAWARE_DIR)/inc


################
# TI Driverlib
DRIVERLIB_NAME     := driverlib
DRIVERLIB_DIR      := $(TIVAWARE_DIR)/$(DRIVERLIB_NAME)
DRIVERLIB_SRC_DIRS := $(DRIVERLIB_DIR)
DRIVERLIB_LIB      := $(BUILD_DIR)/$(DRIVERLIB_NAME).a
DRIVERLIB_IFLAGS   := $(DRIVERLIB_SRC_DIRS)
DRIVERLIB_DFLAGS   :=
DRIVERLIB_CSRC     := $(foreach dir,$(DRIVERLIB_SRC_DIRS),$(wildcard $(dir)/*.c))
DRIVERLIB_ASRC     := $(foreach dir,$(DRIVERLIB_SRC_DIRS),$(wildcard $(dir)/*.s)) \
                      $(foreach dir,$(DRIVERLIB_SRC_DIRS),$(wildcard $(dir)/*.asm))

# Remove drivers that don't compile
DRIVERLIB_CSRC     := $(filter-out $(DRIVERLIB_DIR)/onewire.c,$(DRIVERLIB_CSRC))
ifeq ($(COMPILER),gcc)
DRIVERLIB_ASRC     := $(filter-out $(DRIVERLIB_DIR)/epi_workaround_ccs.s,$(DRIVERLIB_ASRC))
endif

DRIVERLIB_SRCS     := $(DRIVERLIB_CSRC) $(DRIVERLIB_ASRC)
DRIVERLIB_OBJS     := $(call objs_from_srcs,$(DRIVERLIB_SRCS))
DRIVERLIB_DEPS     := $(call deps_from_objs,$(DRIVERLIB_OBJS))

DRIVERLIB_CFLAGS   := $(TOOLCHAIN_CFLAGS) \
                      $(foreach flag,$(TIVAWARE_DFLAGS),$(D)$(flag)) \
                      $(foreach flag,$(TOOLCHAIN_IFLAGS),$(I)"$(flag)") \
                      $(foreach flag,$(TIVAWARE_IFLAGS),$(I)"$(flag)") \
                      $(foreach flag,$(DRIVERLIB_IFLAGS),$(I)"$(flag)")

################
# TI Usblib
USBLIB_NAME     := usblib
USBLIB_DIR      := $(TIVAWARE_DIR)/$(USBLIB_NAME)
USBLIB_SRC_DIRS := $(USBLIB_DIR) $(USBLIB_DIR)/device $(USBLIB_DIR)/host
USBLIB_LIB      := $(BUILD_DIR)/$(USBLIB_NAME).a
USBLIB_IFLAGS   := $(USBLIB_SRC_DIRS)
USBLIB_DFLAGS   :=
USBLIB_CSRC     := $(foreach dir,$(USBLIB_SRC_DIRS),$(wildcard $(dir)/*.c))
USBLIB_ASRC     := $(foreach dir,$(USBLIB_SRC_DIRS),$(wildcard $(dir)/*.s)) \
                   $(foreach dir,$(USBLIB_SRC_DIRS),$(wildcard $(dir)/*.asm))

USBLIB_SRCS     := $(USBLIB_CSRC) $(USBLIB_ASRC)
USBLIB_OBJS     := $(call objs_from_srcs,$(USBLIB_SRCS))
USBLIB_DEPS     := $(call deps_from_objs,$(USBLIB_OBJS))

USBLIB_CFLAGS   := $(TOOLCHAIN_CFLAGS) \
                   $(foreach flag,$(TIVAWARE_DFLAGS),$(D)$(flag)) \
                   $(foreach flag,$(TOOLCHAIN_IFLAGS),$(I)"$(flag)") \
                   $(foreach flag,$(TIVAWARE_IFLAGS),$(I)"$(flag)") \
                   $(foreach flag,$(USBLIB_IFLAGS),$(I)"$(flag)")

################
# Newlib
NEWLIB_NAME    := newlib
NEWLIB_VER     := 3.0.0.20180831
NEWLIB_DIR     := $(THIRD_PARTY_DIR)/$(NEWLIB_NAME)/$(NEWLIB_NAME)-$(NEWLIB_VER)
NEWLIB_LIB_DIR := $(NEWLIB_DIR)/$(TOOLCHAIN)/thumb/$(CORE)/$(CORE_FLOAT_TYPE)/$(NEWLIB_NAME)
NEWLIB_LIBC    := c #libc is configured to be the nano version
NEWLIB_DFLAGS  := _GNU_SOURCE
NEWLIB_CONFIG  := --target=arm-none-eabi \
                  --enable-newlib-reent-small \
                  --disable-newlib-fvwrite-in-streamio \
                  --disable-newlib-fseek-optimization \
                  --disable-newlib-wide-orient \
                  --enable-newlib-nano-malloc \
                  --disable-newlib-unbuf-stream-opt \
                  --enable-lite-exit \
                  --enable-newlib-global-atexit \
                  --enable-newlib-nano-formatted-io \
                  --disable-nls \
                  --disable-newlib-supplied-syscalls


#####
# TivaBsp source
SRC_DIRS := $(sort $(dir $(wildcard $(SRC_DIR)/*/)))
SRC_DIRS += $(sort $(dir $(wildcard $(SRC_DIR)/*/product/$(PRODUCT)/)))
SRC_DIRS += $(sort $(dir $(wildcard $(SRC_DIR)/*/platform/$(PLATFORM)/)))
SRC_DIRS += $(sort $(dir $(wildcard $(SRC_DIR)/*/processor/$(PROCESSOR)/)))
SRC_DIRS += $(sort $(dir $(wildcard $(SRC_DIR)/*/toolchain/$(COMPILER)/)))
SRC_DIRS := $(foreach dir,$(SRC_DIRS),$(dir:/=)) # Remove trailing slash

IFLAGS := $(SRC_DIRS)
CSRC   := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
ASRC   := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s)) \
          $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.asm))

SRCS := $(CSRC) $(ASRC)
OBJS := $(foreach src,$(SRCS),$(BUILD_DIR)/$(subst $(ROOT_DIR)/,,$(basename $(src)).o))
DEPS := $(foreach obj,$(OBJS),$(basename $(obj)).d)


# combine all of the Define flags
DFLAGS += $(PRODUCT_DFLAGS) \
          $(TOOLCHAIN_DFLAGS) \
          $(TIVAWARE_DFLAGS) \
          $(DRIVERLIB_DFLAGS) \
          $(USBLIB_DFLAGS) \
          $(FREERTOS_DFLAGS) \
          $(NEWLIB_DFLAGS)

# combine all of the Include flags
IFLAGS += $(TOOLCHAIN_IFLAGS) \
          $(TIVAWARE_IFLAGS) \
          $(DRIVERLIB_IFLAGS) \
          $(USBLIB_IFLAGS) \
          $(FREERTOS_IFLAGS) \
          $(NEWLIB_IFLAGS)

# convert flags to the proper format and combine
CFLAGS := $(TOOLCHAIN_CFLAGS) \
          $(foreach flag,$(DFLAGS),$(D)$(flag)) \
          $(foreach flag,$(IFLAGS),$(I)"$(flag)")


# Setup Linker flags
LFLAGS := $(TOOLCHAIN_LFLAGS)
LFLAGS += $(call toolchain_cmd_file,project/$(PRODUCT)_$(PROCESSOR)$(VARIANT)_$(COMPILER).$(TOOLCHAIN_LINK_CMD_EXT))
LFLAGS += $(call toolchain_lib_paths,$(PRJ_DIR))

# Select the right library path based on whether we're using a precompiled
# newlib or not
ifeq ($(BUILD_NEWLIB),1)
LFLAGS += $(call toolchain_lib_paths,$(NEWLIB_LIB_DIR))
LFLAGS += $(call toolchain_libs,$(NEWLIB_LIBC))
else
LFLAGS += $(call toolchain_lib_paths,$(TOOLCHAIN_LIB_DIR))
LFLAGS += $(call toolchain_libs,$(TOOLCHAIN_LIBC))
endif


.PHONY: all
all: $(BUILD_DIR)/$(PRODUCT).out $(USBLIB_LIB) $(DRIVERLIB_LIB) $(FREERTOS_LIB)

$(BUILD_DIR):
	$(Q)mkdir -p $@

$(BUILD_DIR)/$(PRODUCT).out: $(OBJS) $(USBLIB_LIB) $(DRIVERLIB_LIB) $(FREERTOS_LIB)
	$(Q)echo "Linking $@"
	$(Q)mkdir -p $(BUILD_DIR)
	$(Q)$(CC) $(L_OPT) -o $@ $^ $(LFLAGS)

# Macro for Archiving libraries
define archive
	$(Q)echo "Archiving $<"
	$(Q)$(AR) $(TOOLCHAIN_AR_OPT) $@ $^
endef

-include $(FREERTOS_DEPS)
$(FREERTOS_OBJS): CFLAGS := $(FREERTOS_CFLAGS)
$(FREERTOS_LIB): $(FREERTOS_OBJS)
	$(call archive)

-include $(USBLIB_DEPS)
$(USBLIB_OBJS): CFLAGS := $(USBLIB_CFLAGS)
$(USBLIB_LIB): $(USBLIB_OBJS)
	$(call archive)

-include $(DRIVERLIB_DEPS)
$(DRIVERLIB_OBJS): CFLAGS := $(DRIVERLIB_CFLAGS)
$(DRIVERLIB_LIB): $(DRIVERLIB_OBJS)
	$(call archive)

-include $(DEPS)   # include all dep files in the makefile

# compile macro used for assembler and compiler
define compile
	$(Q)echo "Compiling $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(C_OPT) $(CFLAGS) $(call toolchain_preprocess,$@,$(BUILD_DIR)/$*.d) $(O_OPT) $@ $(ROOT_DIR)/$<
endef


$(BUILD_DIR)/%.o: %.asm
	$(call compile)

$(BUILD_DIR)/%.o: %.s
	$(call compile)

$(BUILD_DIR)/%.o: %.c
	$(call compile)

.PHONY: clean
clean:
	$(Q)rm -rf $(BUILD_DIR)

distclean:
	$(Q)rm -rf $(ROOT_DIR)/build

newlib:
	$(shell cd $(NEWLIB_DIR);./configure $(NEWLIB_CONFIG))

