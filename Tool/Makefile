SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command

SOURCE_DIR := ..
BUILD_DIR := ../build
BUILD_TYPE := Release
DIST_DIR := ../dist/Tool

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Tool/source
CURRENT_BUILD_DIR = $(BUILD_DIR)/Tool/$(BUILD_TYPE)
AHK_SOURCE_FILE := $(CURRENT_SOURCE_DIR)/exe/Main.ahk
TARGET_EXE := $(CURRENT_BUILD_DIR)/Tool.exe
TARGET_DLL := $(CURRENT_BUILD_DIR)/Tool.dll
# AHK2EXE 是 GUI 程序，使用 Start-Process 调用以等待其完成，否则执行将是异步的，进而导致后续命令找不到生成的文件
AHK2EXE = Start-Process -FilePath "C:/Program Files/AutoHotkey/Compiler/Ahk2Exe.exe" -Wait -ArgumentList

.PHONY: all build install clean dll exe

all: build install
build: | $(CURRENT_BUILD_DIR) dll exe
install:
	if (Test-Path "$(DIST_DIR)") { Remove-Item -Recurse -Force "$(DIST_DIR)" -ErrorAction SilentlyContinue }
	New-Item -Type Directory -Path $(DIST_DIR) -Force | Out-Null
	Copy-Item -Path "$(TARGET_DLL)" -Destination "$(DIST_DIR)" -Force
	Copy-Item -Path "$(TARGET_EXE)" -Destination "$(DIST_DIR)" -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
exe: | $(TARGET_EXE)
dll: | $(CURRENT_BUILD_DIR)
	cmake -S "$(SOURCE_DIR)" -B "$(BUILD_DIR)"
	cmake --build "$(BUILD_DIR)" --config $(BUILD_TYPE) --target Tool
$(TARGET_EXE): | $(CURRENT_BUILD_DIR)
	$(AHK2EXE) "/in $(AHK_SOURCE_FILE)"
	Move-Item "$(CURRENT_SOURCE_DIR)/exe/Main.exe" $@ -Force
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
