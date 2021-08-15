#/**
# * Copyright 2017 Brian Costabile
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

TOOLCHAIN         := ti-cgt-arm
TOOLCHAIN_RELEASE := 20.2.5.LTS
TOOLCHAIN_DIR     := $(TOOLS_ROOT_DIR)/$(TOOLCHAIN)_$(TOOLCHAIN_RELEASE)
TOOLCHAIN_IFLAGS  := $(TOOLCHAIN_DIR)/include
TOOLCHAIN_BIN_DIR := $(TOOLCHAIN_DIR)/bin
TOOLCHAIN_LIB_DIR := $(TOOLCHAIN_DIR)/lib
TOOLCHAIN_LIBC    := libc.a
TOOLCHAIN_AR      := $(TOOLCHAIN_BIN_DIR)/armar
TOOLCHAIN_AS      := $(TOOLCHAIN_BIN_DIR)/armas
TOOLCHAIN_CC      := $(TOOLCHAIN_BIN_DIR)/armcl
TOOLCHAIN_LD      := $(TOOLCHAIN_BIN_DIR)/armld

TOOLCHAIN_CFLAGS := -mv7M4
TOOLCHAIN_CFLAGS += --code_state=16
TOOLCHAIN_CFLAGS += --float_support=FPv4SPD16
TOOLCHAIN_CFLAGS += -me
TOOLCHAIN_CFLAGS += -O2
TOOLCHAIN_CFLAGS += --opt_for_speed=0
TOOLCHAIN_CFLAGS += --c99
TOOLCHAIN_CFLAGS += --printf_support=nofloat
TOOLCHAIN_CFLAGS += --diag_warning=225
TOOLCHAIN_CFLAGS += --diag_wrap=off
TOOLCHAIN_CFLAGS += --display_error_number
TOOLCHAIN_CFLAGS += --abi=eabi

TOOLCHAIN_LFLAGS := -m"$(BUILD_DIR)/$(PRODUCT).map"
TOOLCHAIN_LFLAGS += -i"$(TOOLCHAIN_LIB_DIR)"
TOOLCHAIN_LFLAGS += -i"$(TOOLCHAIN_IFLAGS)"
TOOLCHAIN_LFLAGS += --reread_libs
TOOLCHAIN_LFLAGS += --diag_wrap=off
TOOLCHAIN_LFLAGS += --display_error_number
TOOLCHAIN_LFLAGS += --warn_sections
TOOLCHAIN_LFLAGS += --xml_link_info="$(BUILD_DIR)/$(PRODUCT)_linkInfo.xml"
TOOLCHAIN_LFLAGS += --rom_model

# Function to add a list of library search paths to the linker flags
define toolchain_lib_paths
    $(foreach path,$(1),-i"$(path)")
endef

# Function to add a linker command file to linker flags
define toolchain_cmd_file
    "$(1)"
endef

# Function to add a list of libraries to the linker flags
define toolchain_libs
    $(foreach lib,$(1),-l$(lib))
endef

# Function to build the dependency preprocessor flags
define toolchain_preprocess
    --preproc_with_compile --preproc_dependency="$(2)"
endef

TOOLCHAIN_DEFINE       := --define=
TOOLCHAIN_INCLUDE      := --include_path=
TOOLCHAIN_OUTPUT_FLAG  := -fe
TOOLCHAIN_COMPILE_FLAG :=
TOOLCHAIN_LINK_CMD_EXT := cmd
TOOLCHAIN_AR_OPT       := a
TOOLCHAIN_LINK_FLAG    := $(TOOLCHAIN_CFLAGS) -z
