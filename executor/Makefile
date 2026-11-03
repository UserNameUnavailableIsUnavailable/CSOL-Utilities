SOURCE_DIR ?= ..
BUILD_DIR ?= ../build
DIST_DIR ?= ../dist

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Executor
CURRENT_BUILD_DIR = $(BUILD_DIR)/Executor
CURRENT_DIST_DIR = $(DIST_DIR)/Executor

MODULES := core custom

define NL


endef

include $(SOURCE_DIR)/make/pwsh.mk

.PHONY: all build clean install

all: install
# 移除 Executor.lua，该文件由 Install.ps1 运行后创建
	if (Test-Path "$(BUILD_DIR)/Executor.lua") { Remove-Item -Force "$(BUILD_DIR)/Executor.lua" }
build: | $(CURRENT_BUILD_DIR)
	$(foreach module,$(MODULES),\
	Copy-Item -Recurse "$(CURRENT_SOURCE_DIR)/$(module)" "$(CURRENT_BUILD_DIR)" -Force$(NL)\
	)
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.ps1" "$(CURRENT_BUILD_DIR)" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Install.cmd" "$(CURRENT_BUILD_DIR)" -Force
install: build
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	Copy-Item -Recurse "$(CURRENT_BUILD_DIR)" "$(CURRENT_DIST_DIR)" -Force
clean:
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null

