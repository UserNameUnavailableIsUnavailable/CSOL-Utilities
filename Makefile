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
DIST_DIR := ./dist
CURRENT_DIST_DIR = $(DIST_DIR)/$(DISTRO)

# 手册文件名
MANUAL_NAME := $(DISTRO).pdf

# 压缩包
BUNDLE_NAME := $(DISTRO).zip

TARGETS = Controller Executor Manual Tool Bundle

.PHONY: all clean $(TARGETS)

all: $(TARGETS)

Controller: | $(BUILD_DIR) $(CURRENT_DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Controller" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../$(BUILD_DIR)" BUILD_TYPE="$(BUILD_TYPE)" DIST_DIR="../$(CURRENT_DIST_DIR)"
Executor: | $(BUILD_DIR) $(CURRENT_DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Executor" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../$(BUILD_DIR)" DIST_DIR="../$(CURRENT_DIST_DIR)"
Tool: | $(BUILD_DIR) $(CURRENT_DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Tool" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../$(BUILD_DIR)" BUILD_TYPE="$(BUILD_TYPE)" DIST_DIR="../$(CURRENT_DIST_DIR)"
Manual: | $(BUILD_DIR) $(DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Manual" SOURCE_DIR="../$(SOURCE_DIR)" BUILD_DIR="../build" DIST_DIR="../$(DIST_DIR)" MANUAL_NAME="$(MANUAL_NAME)"
Bundle: | $(BUILD_DIR) $(DIST_DIR) $(CURRENT_DIST_DIR)
	Compress-Archive -Path "$(CURRENT_DIST_DIR)/*" -DestinationPath "$(DIST_DIR)/$(BUNDLE_NAME)" -Force
clean:
	Remove-Item -Force -Recurse -Path $(BUILD_DIR)

$(BUILD_DIR):
	New-Item -Type Directory -Path $(BUILD_DIR) -Force
$(DIST_DIR):
	New-Item -Type Directory -Path $(DIST_DIR) -Force
$(CURRENT_DIST_DIR): | $(DIST_DIR)
	New-Item -Type Directory -Path $(CURRENT_DIST_DIR) -Force
	