SOURCE_DIR ?= ..
BUILD_TYPE ?= Release
BUILD_DIR ?= ../build
DIST_DIR ?= ../dist

CMAKE_SOURCE_DIR = $(SOURCE_DIR)
CMAKE_BINARY_DIR = $(BUILD_DIR)
CMAKE_CONFIG_TYPE = $(BUILD_TYPE)
include $(SOURCE_DIR)/make/cmake.mk

include $(SOURCE_DIR)/make/pwsh.mk

.PHONY: all
all: build

.PHONY: build
build:
	cmake $(CMAKE_CONFIGURE_ARGS)
	cmake $(CMAKE_BUILD_ARGS) --target Protos

.PHONY: clean
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue

