SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command
# 源代码目录
SOURCE_DIR := ..
CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Executor
# 构建目录
BUILD_DIR := ../build
CURRENT_BUILD_DIR = $(BUILD_DIR)/Executor
# 发布目录
DIST_DIR := ../dist/Executor
# 源码文件
SOURCE_FILES := $(wildcard $(CURRENT_SOURCE_DIR)/*.lua)

.PHONY: all build clean install

all: build install
# 移除 Executor.lua，该文件由 Install.ps1 运行后创建
	if (Test-Path "$(BUILD_DIR)/Executor.lua") { Remove-Item -Force "$(BUILD_DIR)/Executor.lua" }
build: | $(CURRENT_BUILD_DIR) $(addprefix $(CURRENT_BUILD_DIR)/,$(notdir $(SOURCE_FILES)))
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.ps1" "$(BUILD_DIR)/Install.ps1" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.cmd" "$(BUILD_DIR)/Install.cmd" -Force
install:
	if (Test-Path "$(DIST_DIR)") { Remove-Item -Recurse -Force "$(DIST_DIR)" -ErrorAction SilentlyContinue }
	Copy-Item -Recurse "$(CURRENT_BUILD_DIR)" "$(DIST_DIR)" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.ps1" "$(DIST_DIR)/Install.ps1" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.cmd" "$(DIST_DIR)/Install.cmd" -Force
clean:
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
$(CURRENT_BUILD_DIR)/%.lua: %.lua | $(CURRENT_BUILD_DIR)
	Copy-Item -Path $< -Destination $@ -Force
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null