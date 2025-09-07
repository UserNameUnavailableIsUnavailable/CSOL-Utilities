SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -Command

# 项目
PROJECT_NAME := CSOL-Utilities
VERSION = v1.5.3
PLATFORM = x64
DISTRO = $(PROJECT_NAME)-$(VERSION)-$(PLATFORM)

# 所有的构建文件放在此目录下
BUILD_DIR = build/$(DISTRO)

# Controller
# 模型文件路径需在命令行参数中指定
MODELS_PATH :=#
# 源代码目录
CONTROLLER_SOURCE_DIR := Controller
 # 构建结束后，将构建的文件复制到此处
CONTROLLER_BUILD_DIR = $(BUILD_DIR)
# Release 或 Debug
CONTROLLER_BUILD_TYPE = Release

# Executor
# 源代码目录
EXECUTOR_SOURCE_DIR := Executor
# 构建结束后，将构建的文件复制到此处
EXECUTOR_BUILD_DIR = $(BUILD_DIR)

# 手册文件名
MANUAL_NAME := $(DISTRO).pdf
# 手册源代码目录
MANUAL_SOURCE_DIR := Manual
# 构建结束后，将手册复制到构建根目录下
MANUAL_BUILD_DIR = build

# 压缩包
BUNDLE_NAME = build/$(DISTRO).zip

VPATH = $(BUILD_DIR)

TARGETS = Controller Executor Manual Bundle

.PHONY: all clean $(TARGETS)

all: $(TARGETS)

Controller: | $(BUILD_DIR)
	$(MAKE) --directory="$(CONTROLLER_SOURCE_DIR)" BUILD_DIR="../$(CONTROLLER_BUILD_DIR)" MODELS_PATH="$$((Get-Item $(MODELS_PATH)).FullName)" BUILD_TYPE=$(CONTROLLER_BUILD_TYPE)
Executor: | $(BUILD_DIR)
	$(MAKE) BUILD_DIR="../$(EXECUTOR_BUILD_DIR)" --directory="$(EXECUTOR_SOURCE_DIR)"
Manual: | $(BUILD_DIR)
	$(MAKE) --directory=$(MANUAL_SOURCE_DIR) BUILD_DIR="../$(MANUAL_BUILD_DIR)" MANUAL_NAME="$(MANUAL_NAME)"
Bundle: | $(BUILD_DIR)
	Compress-Archive -Path "$(BUILD_DIR)/*" -DestinationPath "$(BUNDLE_NAME)" -Force
$(BUILD_DIR):
	New-Item -Type Directory -Path $(BUILD_DIR) -Force
clean:
	Remove-Item -Force -Recurse -Path $(BUILD_DIR)
