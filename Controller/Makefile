BUILD_DIR := build
SUBDIR = $(BUILD_DIR)/Controller
SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command
# 模型文件路径，需在命令行参数中指定
MODELS_PATH :=
# 汇总构建目录，可由命令行参数指定（将在汇总目录下创建 Controller 子文件夹）
# 源代码目录
CURRENT_SOURCE_DIR := .
# 所有的构建文件放在此目录下
CURRENT_BUILD_DIR := build
# Release 或 Debug
BUILD_TYPE := Release

all: | $(SUBDIR)
# 使用 cmake 构建 Controller
	$$ErrorActionPreference = 'Stop' # 遇到错误时停止执行
	cmake -S . -B "$(CURRENT_BUILD_DIR)" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DMODELS_PATH="$(MODELS_PATH)"
	cmake --build "$(CURRENT_BUILD_DIR)" --config $(BUILD_TYPE)
# 如果目录存在，则删除目录下的所有文件
	if (Test-Path "$(SUBDIR)") { Remove-Item -Force -Recurse "$(SUBDIR)/*" }
# 重新创建目录
	New-Item -Type Directory -Path "$(SUBDIR)" -Force
# 复制的编译结果到构建目录
	Copy-Item -Destination "$(SUBDIR)" -Path "$(CURRENT_BUILD_DIR)/$(BUILD_TYPE)/*" -Recurse -Force
# 复制 Controller.ps1 到构建目录
	Copy-Item -Path "$(CURRENT_SOURCE_DIR)/Controller.ps1" -Destination "$(BUILD_DIR)/Controller.ps1" -Force
$(SUBDIR):
	New-Item -Type Directory -Path $(SUBDIR) -Force