SOURCE_DIR := ..
BUILD_TYPE := Release
BUILD_DIR := ../build
DIST_DIR := ../dist

CMAKE_GENERATOR := Visual Studio 17 2022
CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Controller
CURRENT_BUILD_DIR = $(BUILD_DIR)/Controller/$(BUILD_TYPE)
CURRENT_DIST_DIR = $(DIST_DIR)/Controller

# 代理
include $(SOURCE_DIR)/make/proxy.mk
# 使用 PowerShell 作为 Makefile 的 shell
include $(SOURCE_DIR)/make/pwsh.mk
# CMake 设定
CMAKE_SOURCE_DIR = $(SOURCE_DIR)
CMAKE_BINARY_DIR = $(BUILD_DIR)
CMAKE_INSTALL_PREFIX = $(DIST_DIR)
CMAKE_CONFIG_TYPE = $(BUILD_TYPE)
include $(SOURCE_DIR)/make/cmake.mk

.PHONY: all build clean install

all: build install

build: | $(CURRENT_BUILD_DIR)
	cmake $(CMAKE_CONFIGURE_ARGS)
	cmake $(CMAKE_BUILD_ARGS) --target Controller
	Copy-Item "$(CURRENT_SOURCE_DIR)/Controller.ps1" "$(BUILD_DIR)/Controller.ps1" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Controller.cmd" "$(BUILD_DIR)/Controller.cmd" -Force
install:
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	Copy-Item -Path "$(CURRENT_BUILD_DIR)" -Destination "$(CURRENT_DIST_DIR)" -Recurse -Force
	Copy-Item "$(BUILD_DIR)/Controller.ps1" "$(DIST_DIR)/Controller.ps1" -Force
	Copy-Item "$(BUILD_DIR)/Controller.cmd" "$(DIST_DIR)/Controller.cmd" -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
$(CURRENT_BUILD_DIR):
	New-Item -Type Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
