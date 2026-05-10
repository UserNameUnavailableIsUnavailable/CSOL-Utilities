SOURCE_DIR ?= ..
BUILD_TYPE ?= Release
BUILD_DIR ?= ../build
DIST_DIR ?= ../dist

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Emulator
CURRENT_BUILD_DIR = $(BUILD_DIR)/Emulator
CURRENT_DIST_DIR = $(DIST_DIR)/Emulator

# 代理
include $(SOURCE_DIR)/make/proxy.mk
# 使用 PowerShell 作为 Makefile 的 shell
include $(SOURCE_DIR)/make/pwsh.mk
# CMake 设定
CMAKE_SOURCE_DIR = $(SOURCE_DIR)
CMAKE_BINARY_DIR = $(BUILD_DIR)
CMAKE_INSTALL_PREFIX = $(CURRENT_DIST_DIR)
CMAKE_CONFIG_TYPE = $(BUILD_TYPE)
include $(SOURCE_DIR)/make/cmake.mk

.PHONY: all
all: build

build:
	cmake $(CMAKE_CONFIGURE_ARGS)
	cmake $(CMAKE_BUILD_ARGS) --target Emulator

$(CURRENT_BUILD_DIR):
	New-Item -Type Directory -Path "$(CURRENT_BUILD_DIR)" -Force
