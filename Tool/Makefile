BUILD_TYPE := Release
SOURCE_DIR := ..
BUILD_DIR := ../build
DIST_DIR := ../dist

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Tool
CURRENT_BUILD_DIR = $(BUILD_DIR)/Tool
CURRENT_DIST_DIR := $(DIST_DIR)/Tool

# Use PowerShell as the shell for this Makefile
include $(SOURCE_DIR)/make/pwsh.mk
# proxy used by vcpkg, git, etc.
include $(SOURCE_DIR)/make/proxy.mk
# CMake settings
CMAKE_SOURCE_DIR = $(SOURCE_DIR)
CMAKE_BINARY_DIR = $(BUILD_DIR)
CMAKE_INSTALL_PREFIX = $(CURRENT_DIST_DIR)
CMAKE_CONFIG_TYPE = $(BUILD_TYPE)
include $(SOURCE_DIR)/make/cmake.mk
# AutoHotkey compiler
ifndef AHK2EXE_PATH
$(error AHK2EXE_PATH is not set.)
endif

AHK_SOURCE_FILE = $(CURRENT_SOURCE_DIR)/scripts/Tool.ahk
TARGET_EXE = $(CURRENT_BUILD_DIR)/Tool.exe
TARGET_ICON = $(CURRENT_SOURCE_DIR)/Tool.ico
TARGET_DLL = $(CURRENT_BUILD_DIR)/Tool.dll

.PHONY: all build install clean dll exe

all: build install
build: dll exe | $(CURRENT_BUILD_DIR)
install:
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	cmake $(CMAKE_INSTALL_ARGS) --component Tool
	New-Item -Type Directory -Path $(CURRENT_DIST_DIR) -Force | Out-Null
	Copy-Item -Path "$(CURRENT_BUILD_DIR)/$(BUILD_TYPE)/*" -Destination "$(CURRENT_DIST_DIR)" -Recurse -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
exe: | $(TARGET_EXE)
dll: | $(CURRENT_BUILD_DIR)
	cmake $(CMAKE_CONFIGURE_ARGS)
	cmake $(CMAKE_BUILD_ARGS) --target Tool
$(TARGET_EXE): $(AHK_SOURCE_FILE) | $(CURRENT_BUILD_DIR)
# FIXME: Ahk2Exe currently outputs exe to ${CMAKE_BINARY_DIR}/$<CONFIG>/Tool.exe. However, not all cmake generators output artifacts to $<CONFIG> subdirectory.
# NOTE: It is better to run Ahk2Exe in CMakeLists.txt using add_custom_command, which can ensure the output path is correct regardless of the generator used.
	& "$(CURRENT_SOURCE_DIR)/Ahk2Exe.ps1" -CompilerPath "$(AHK2EXE_PATH)" -InputSource "$(AHK_SOURCE_FILE)" -OutputDirectory "$(CURRENT_BUILD_DIR)/$(BUILD_TYPE)" -BinaryName "Tool"
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
