# 项目设定
PROJECT := CSOL-Utilities
VERSION := v1.5.6
DISTRO = $(PROJECT)-$(VERSION)
# 源码
SOURCE_DIR := .
CURRENT_SOURCE_DIR = $(SOURCE_DIR)
# 构建
BUILD_TYPE := Release
BUILD_DIR := ./build
CURRENT_BUILD_DIR = $(BUILD_DIR)/$(DISTRO)
# 发布
DIST_DIR := ./dist
CURRENT_DIST_DIR = $(DIST_DIR)/$(DISTRO)
# 手册文件名
MANUAL_NAME := $(DISTRO).pdf
# 压缩包
BUNDLE_NAME := $(DISTRO).zip
TARGETS := Controller Executor Manual Tool Bundle

include $(SOURCE_DIR)/make/proxy.mk
# 使用 PowerShell 作为 Makefile 的 shell
include $(SOURCE_DIR)/make/pwsh.mk

.PHONY: all clean environment $(TARGETS)

all: environment $(TARGETS)

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
environment:
	Write-Host "HTTP_PROXY: $(HTTP_PROXY)" -ForegroundColor GREEN
	Write-Host "HTTPS_PROXY: $(HTTPS_PROXY)" -ForegroundColor GREEN
$(BUILD_DIR):
	New-Item -Type Directory -Path "$(BUILD_DIR)" -Force
$(DIST_DIR):
	New-Item -Type Directory -Path "$(DIST_DIR)" -Force
$(CURRENT_DIST_DIR): | $(DIST_DIR)
	New-Item -Type Directory -Path "$(CURRENT_DIST_DIR)" -Force
	