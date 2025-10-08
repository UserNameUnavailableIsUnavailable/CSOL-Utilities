SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command

# 源代码目录
SOURCE_DIR := ..
# 构建类型
BUILD_TYPE := Release
# 构建目录
BUILD_DIR := ../build
# 发布目录
DIST_DIR := ../dist
ARCH := Win64
CMAKE_GENERATOR := Visual Studio 17 2022

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Controller
CURRENT_BUILD_DIR = $(BUILD_DIR)/Controller/$(BUILD_TYPE)
CURRENT_DIST_DIR = $(DIST_DIR)/Controller
# 模型文件路径
MODELS_DIR = $(CURRENT_SOURCE_DIR)/models
BUILD_MODELS_DIR = $(CURRENT_BUILD_DIR)/models
# 本地化文件路径
LOCALES_DIR = $(CURRENT_SOURCE_DIR)/locales
BUILD_LOCALES_DIR = $(CURRENT_BUILD_DIR)/locales

.PHONY: all build clean install models exe

all: build install

build: | $(CURRENT_BUILD_DIR) exe $(BUILD_MODELS_DIR) $(BUILD_LOCALES_DIR)
	Copy-Item "$(CURRENT_SOURCE_DIR)/Controller.ps1" "$(BUILD_DIR)/Controller.ps1" -Force
	Copy-Item "$(CURRENT_SOURCE_DIR)/Controller.cmd" "$(BUILD_DIR)/Controller.cmd" -Force
exe: | $(CURRENT_BUILD_DIR)
	cmake -S "$(SOURCE_DIR)" -B "$(BUILD_DIR)" -G "$(CMAKE_GENERATOR)" -DCMAKE_BUILD_TYPE="$(BUILD_TYPE)"
	cmake --build "$(BUILD_DIR)" --config "$(BUILD_TYPE)" --target Controller
install:
	if (Test-Path "$(CURRENT_DIST_DIR)") { Remove-Item -Recurse -Force "$(CURRENT_DIST_DIR)" }
	Copy-Item -Recurse "$(CURRENT_BUILD_DIR)" "$(CURRENT_DIST_DIR)" -Force
	Copy-Item "$(BUILD_DIR)/Controller.ps1" "$(DIST_DIR)/Controller.ps1" -Force
	Copy-Item "$(BUILD_DIR)/Controller.cmd" "$(DIST_DIR)/Controller.cmd" -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
models:
# 下载模型文件
	&"$(CURRENT_SOURCE_DIR)/models.ps1" -Path "$(MODELS_DIR)"
$(CURRENT_BUILD_DIR):
	New-Item -ItemType Directory -Path "$(CURRENT_BUILD_DIR)" -Force | Out-Null
$(BUILD_MODELS_DIR): models
# MODELS_DIR 文件夹发生变化，先删除旧的，再复制新的
	if (Test-Path "$(BUILD_MODELS_DIR)") { Remove-Item -Recurse -Force "$(BUILD_MODELS_DIR)" -ErrorAction SilentlyContinue }
	Copy-Item -Recurse "$(MODELS_DIR)" "$(BUILD_MODELS_DIR)" -Force -Exclude ".git*"
$(BUILD_LOCALES_DIR): $(LOCALES_DIR)
# LOCALES_DIR 文件夹发生变化，先删除旧的，再复制新的
	if (Test-Path "$(BUILD_LOCALES_DIR)") { Remove-Item -Recurse -Force "$(BUILD_LOCALES_DIR)" -ErrorAction SilentlyContinue }
	Copy-Item -Recurse "$(LOCALES_DIR)" "$(BUILD_LOCALES_DIR)" -Force
