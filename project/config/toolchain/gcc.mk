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
FILENAME := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
$(info Including:$(FILENAME))

TOOLCHAIN         := arm-none-eabi
TOOLCHAIN_RELEASE := 10.3-2021.07
TOOLCHAIN_DIR     := $(TOOLS_ROOT_DIR)/gcc-$(TOOLCHAIN)-$(TOOLCHAIN_RELEASE)
TOOLCHAIN_IFLAGS  := $(TOOLCHAIN_DIR)/$(TOOLCHAIN)/include
TOOLCHAIN_BIN_DIR := $(TOOLCHAIN_DIR)/bin
TOOLCHAIN_LIB_DIR := $(TOOLCHAIN_DIR)/$(TOOLCHAIN)/lib/thumb/$(CORE)/$(CORE_FLOAT_TYPE)
TOOLCHAIN_LIBC    := c_nano
TOOLCHAIN_AR      := $(TOOLCHAIN_BIN_DIR)/$(TOOLCHAIN)-ar
TOOLCHAIN_AS      := $(TOOLCHAIN_BIN_DIR)/$(TOOLCHAIN)-as
TOOLCHAIN_CC      := $(TOOLCHAIN_BIN_DIR)/$(TOOLCHAIN)-gcc
TOOLCHAIN_LD      := $(TOOLCHAIN_BIN_DIR)/$(TOOLCHAIN)-ld

TOOLCHAIN_CFLAGS := -mcpu=$(CPU)
TOOLCHAIN_CFLAGS += -march=arm$(CORE)
TOOLCHAIN_CFLAGS += -mthumb
TOOLCHAIN_CFLAGS += -mfloat-abi=$(CORE_FLOAT_TYPE)
TOOLCHAIN_CFLAGS += -mfpu=$(CORE_FLOAT)-d16

TOOLCHAIN_CFLAGS += -O3 -Os
TOOLCHAIN_CFLAGS += -ffunction-sections -fdata-sections
TOOLCHAIN_CFLAGS += -g
TOOLCHAIN_CFLAGS += -Wall

TOOLCHAIN_LFLAGS := -Wl,-Map,"$(BUILD_DIR)/$(PRODUCT).map"
TOOLCHAIN_LFLAGS += -nostartfiles
TOOLCHAIN_LFLAGS += -Wl,--gc-sections

# Function to add a list of library search paths to the linker flags
define toolchain_lib_paths
    $(foreach path,$(1),-L"$(path)")
endef

# Function to add a linker command file to linker flags
define toolchain_cmd_file
    -Wl,-T"$(1)"
endef

# Function to add a list of libraries to the linker flags
define toolchain_libs
	-Wl,--start-group \
    $(foreach lib,$(1),-l$(lib)) \
	-Wl,--end-group
endef

# Function to build the dependency preprocessor flags
define toolchain_preprocess
    -MT $(1) -MMD -MP -MF $(2)
endef

TOOLCHAIN_DEFINE       := -D
TOOLCHAIN_INCLUDE      := -I
TOOLCHAIN_OUTPUT_FLAG  := -o
TOOLCHAIN_COMPILE_FLAG := -c
TOOLCHAIN_LINK_CMD_EXT := lds
TOOLCHAIN_AR_OPT       := cr
TOOLCHAIN_LINK_FLAG    :=
