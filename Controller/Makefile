SOURCE_DIR := ..
BUILD_TYPE := Release
BUILD_DIR := ../build
DIST_DIR := ../dist
PLATFORM := Windows
TOOLSET := host=x64
PLATFORM := x64

CMAKE_GENERATOR := Visual Studio 17 2022
CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Controller
CURRENT_BUILD_DIR = $(BUILD_DIR)/Controller/$(BUILD_TYPE)
CURRENT_DIST_DIR = $(DIST_DIR)/Controller

include $(SOURCE_DIR)/make/proxy.mk
# 使用 PowerShell 作为 Makefile 的 shell
include $(SOURCE_DIR)/make/pwsh.mk
include $(SOURCE_DIR)/make/cmake.mk

.PHONY: all build clean install

all: build install

build: | $(CURRENT_BUILD_DIR)
	cmake -S "$(SOURCE_DIR)" -B "$(BUILD_DIR)" -G "$(CMAKE_GENERATOR)" -T "$(CMAKE_TOOLSET)" -A "$(CMAKE_PLATFORM)" $(CMAKE_CONFIGURE_ARGS)
	cmake --build "$(BUILD_DIR)" --target Controller $(CMAKE_BUILD_ARGS)
	Copy-Item "$(CURRENT_SOURCE_DIR)/Controller.ps1" "$(BUILD_DIR)/Controller.ps1" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Controller.cmd" "$(BUILD_DIR)/Controller.cmd" -Force
install:
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	Copy-Item -Recurse "$(CURRENT_BUILD_DIR)" "$(CURRENT_DIST_DIR)" -Force
	Copy-Item "$(BUILD_DIR)/Controller.ps1" "$(DIST_DIR)/Controller.ps1" -Force
	Copy-Item "$(BUILD_DIR)/Controller.cmd" "$(DIST_DIR)/Controller.cmd" -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
$(CURRENT_BUILD_DIR):
	New-Item -Type Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null