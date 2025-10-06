SHELL := pwsh.exe
.SHELLFLAGS := -NoProfile -NoLogo -Command

# 项目
PROJECT := CSOL-Utilities
VERSION := v1.5.4
PLATFORM := x64
DISTRO = $(PROJECT)-$(VERSION)-$(PLATFORM)

# 源代码目录
SOURCE_DIR := .
# 构建目录
BUILD_DIR := ./build
# 构建类型
BUILD_TYPE := Release
# 发布目录
DIST_DIR := ./dist/$(DISTRO)

# Executor
# 源代码目录
EXECUTOR_SOURCE_DIR := Executor

# 手册文件名
MANUAL_NAME := $(DISTRO).pdf
# 手册源代码目录
MANUAL_SOURCE_DIR := Manual

# 压缩包
BUNDLE := ./dist/$(DISTRO).zip

VPATH = $(BUILD_DIR)

TARGETS = Controller Executor Manual Tool

.PHONY: all clean $(TARGETS) Bundle

all: $(TARGETS)
	$(MAKE) Bundle

Controller: | $(BUILD_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Controller" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../$(BUILD_DIR)" BUILD_TYPE="$(BUILD_TYPE)" DIST_DIR="../$(DIST_DIR)/Controller"
Executor: | $(BUILD_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Executor" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../$(BUILD_DIR)" DIST_DIR="../$(DIST_DIR)/Executor"
Manual: | $(BUILD_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Manual" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../build" DIST_DIR="../$(DIST_DIR)" MANUAL_NAME="$(MANUAL_NAME)"
Bundle: | $(BUILD_DIR)
	Compress-Archive -Path "$(DIST_DIR)/*" -DestinationPath "$(BUNDLE)" -Force
Tool: | $(BUILD_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Tool" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../$(BUILD_DIR)" BUILD_TYPE="$(BUILD_TYPE)" DIST_DIR="../$(DIST_DIR)/Tool"
$(BUILD_DIR):
	New-Item -Type Directory -Path $(BUILD_DIR) -Force
clean:
	Remove-Item -Force -Recurse -Path $(BUILD_DIR)
$(DIST_DIR):
	New-Item -Type Directory -Path $(DIST_DIR) -Force