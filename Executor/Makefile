SOURCE_DIR := ..
BUILD_DIR := ../build
DIST_DIR := ../dist

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Executor
CURRENT_BUILD_DIR = $(BUILD_DIR)/Executor
CURRENT_DIST_DIR = $(DIST_DIR)/Executor

# 源码文件
SOURCE_FILES := $(wildcard $(CURRENT_SOURCE_DIR)/*.lua)

include $(SOURCE_DIR)/pwsh.mk

.PHONY: all build clean install

all: build install
# 移除 Executor.lua，该文件由 Install.ps1 运行后创建
	if (Test-Path "$(BUILD_DIR)/Executor.lua") { Remove-Item -Force "$(BUILD_DIR)/Executor.lua" }
build: | $(CURRENT_BUILD_DIR) $(addprefix $(CURRENT_BUILD_DIR)/,$(notdir $(SOURCE_FILES)))
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.ps1" "$(BUILD_DIR)/Install.ps1" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.cmd" "$(BUILD_DIR)/Install.cmd" -Force
install:
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	Copy-Item -Recurse "$(CURRENT_BUILD_DIR)" "$(CURRENT_DIST_DIR)" -Force
	Copy-Item "$(BUILD_DIR)/Install.ps1" "$(DIST_DIR)/Install.ps1" -Force
	Copy-Item "$(BUILD_DIR)/Install.cmd" "$(DIST_DIR)/Install.cmd" -Force
clean:
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
$(CURRENT_BUILD_DIR)/%.lua: %.lua | $(CURRENT_BUILD_DIR)
	Copy-Item -Path $< -Destination $@ -Force
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
