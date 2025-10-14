BUILD_TYPE := Release
SOURCE_DIR := ..
BUILD_DIR := ../build
DIST_DIR := ../dist

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Tool/source
CURRENT_BUILD_DIR = $(BUILD_DIR)/Tool/$(BUILD_TYPE)
CURRENT_DIST_DIR := $(DIST_DIR)/Tool

AHK_SOURCE_FILE := $(CURRENT_SOURCE_DIR)/exe/Main.ahk
TARGET_EXE := $(CURRENT_BUILD_DIR)/Tool.exe
TARGET_DLL := $(CURRENT_BUILD_DIR)/Tool.dll

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

# AHK2EXE 是 GUI 程序，使用 Start-Process 调用以等待其完成，否则执行将是异步的，进而导致后续命令找不到生成的文件
AHK2EXE := Start-Process -FilePath "C:/Program Files/AutoHotkey/Compiler/Ahk2Exe.exe" -Wait -ArgumentList

.PHONY: all build install clean dll exe

all: build install
build: dll exe | $(CURRENT_BUILD_DIR)
install:
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	New-Item -Type Directory -Path $(CURRENT_DIST_DIR) -Force | Out-Null
	Copy-Item -Path "$(TARGET_DLL)" -Destination "$(CURRENT_DIST_DIR)" -Force
	Copy-Item -Path "$(TARGET_EXE)" -Destination "$(CURRENT_DIST_DIR)" -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
exe: | $(TARGET_EXE)
dll: | $(CURRENT_BUILD_DIR)
	cmake $(CMAKE_CONFIGURE_ARGS)
	cmake $(CMAKE_BUILD_ARGS) --target Tool
$(TARGET_EXE): $(AHK_SOURCE_FILE) | $(CURRENT_BUILD_DIR)
	$(AHK2EXE) "/in $(AHK_SOURCE_FILE)"
	Move-Item "$(CURRENT_SOURCE_DIR)/exe/Main.exe" $@ -Force
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
