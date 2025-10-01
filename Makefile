SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -Command
# 项目
PROJECT_NAME := CSOL-Utilities
VERSION = v1.5.4
PLATFORM = x64
DISTRO = $(PROJECT_NAME)-$(VERSION)-$(PLATFORM)

# 所有的构建文件放在此目录下
BUILD_DIR = build/$(DISTRO)

# Controller
# 模型文件路径需在命令行参数中指定，须为 **绝对路径**
MODELS_PATH :=
# 源代码目录
CONTROLLER_SOURCE_DIR := Controller
 # 构建结束后，将构建的文件复制到此处
CONTROLLER_BUILD_DIR = $(BUILD_DIR)
# Release 或 Debug
CONTROLLER_BUILD_TYPE = Release

# Executor
# 源代码目录
EXECUTOR_SOURCE_DIR := Executor

# 手册文件名
MANUAL_NAME := $(DISTRO).pdf
# 手册源代码目录
MANUAL_SOURCE_DIR := Manual

# 压缩包
BUNDLE_NAME = build/$(DISTRO).zip

VPATH = $(BUILD_DIR)

TARGETS = Controller Executor Manual Tool

.PHONY: all clean $(TARGETS) Bundle

all: $(TARGETS)
	$(MAKE) Bundle

Controller: | $(BUILD_DIR)
	$(MAKE) --directory="$(CONTROLLER_SOURCE_DIR)" BUILD_DIR="../$(BUILD_DIR)" BUILD_TYPE=$(CONTROLLER_BUILD_TYPE) MODELS_PATH="$(MODELS_PATH)"
Executor: | $(BUILD_DIR)
	$(MAKE) BUILD_DIR="../$(BUILD_DIR)" --directory="$(EXECUTOR_SOURCE_DIR)"
Manual: | $(BUILD_DIR)
	$(MAKE) --directory=$(MANUAL_SOURCE_DIR) BUILD_DIR="../build" MANUAL_NAME="$(MANUAL_NAME)"
Bundle: | $(BUILD_DIR)
	Compress-Archive -Path "$(BUILD_DIR)/*" -DestinationPath "$(BUNDLE_NAME)" -Force
Tool: | $(BUILD_DIR)
	$(MAKE) --directory=Tool BUILD_DIR="../$(BUILD_DIR)"
$(BUILD_DIR):
	New-Item -Type Directory -Path $(BUILD_DIR) -Force
clean:
	Remove-Item -Force -Recurse -Path $(BUILD_DIR)
