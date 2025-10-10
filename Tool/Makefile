BUILD_TYPE := Release
SOURCE_DIR := ..
BUILD_DIR := ../build
DIST_DIR := ../dist
PLATFORM := Windows
ARCH := x64

CMAKE_GENERATOR := Visual Studio 17 2022

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Tool/source
CURRENT_BUILD_DIR = $(BUILD_DIR)/Tool/$(BUILD_TYPE)
CURRENT_DIST_DIR := $(DIST_DIR)/Tool
AHK_SOURCE_FILE := $(CURRENT_SOURCE_DIR)/exe/Main.ahk
TARGET_EXE := $(CURRENT_BUILD_DIR)/Tool.exe
TARGET_DLL := $(CURRENT_BUILD_DIR)/Tool.dll
include $(SOURCE_DIR)/pwsh.mk # 使用 PowerShell 作为 Makefile 的 shell
# AHK2EXE 是 GUI 程序，使用 Start-Process 调用以等待其完成，否则执行将是异步的，进而导致后续命令找不到生成的文件
AHK2EXE = Start-Process -FilePath "C:/Program Files/AutoHotkey/Compiler/Ahk2Exe.exe" -Wait -ArgumentList

.PHONY: all build install clean dll exe

all: build install
build: | $(CURRENT_BUILD_DIR) dll exe
install:
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	New-Item -Type Directory -Path $(CURRENT_DIST_DIR) -Force | Out-Null
	Copy-Item -Path "$(TARGET_DLL)" -Destination "$(CURRENT_DIST_DIR)" -Force
	Copy-Item -Path "$(TARGET_EXE)" -Destination "$(CURRENT_DIST_DIR)" -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
exe: | $(TARGET_EXE)
dll: | $(CURRENT_BUILD_DIR)
	cmake -S "$(SOURCE_DIR)" -B "$(BUILD_DIR)" -G "$(CMAKE_GENERATOR)" -A "$(ARCH)" -DCMAKE_BUILD_TYPE="$(BUILD_TYPE)"
	cmake --build "$(BUILD_DIR)" --config "$(BUILD_TYPE)" --target Tool
$(TARGET_EXE): | $(CURRENT_BUILD_DIR)
	$(AHK2EXE) "/in $(AHK_SOURCE_FILE)"
	Move-Item "$(CURRENT_SOURCE_DIR)/exe/Main.exe" $@ -Force
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
