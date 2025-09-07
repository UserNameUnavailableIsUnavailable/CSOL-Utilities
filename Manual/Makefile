SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command
# 项目构建目录，构建的最终放在此处，可由命令行参数指定
BUILD_DIR := build
# 最终保存到项目构建目录的手册文件名，由外部命令行参数指定
MANUAL_NAME := Manual.pdf
# 当前手册源代码目录
CURRENT_SOURCE_DIR := .
# 当前手册构建目录
CURRENT_BUILD_DIR := build
# 源代码目录
_SOURCE := main.tex $(wildcard chapter_*.tex)
# 生成的手册，main.tex -> main.pdf
_TARGET := main.pdf
SOURCE = $(addprefix $(CURRENT_SOURCE_DIR)/,$(_SOURCE))
TARGET = $(addprefix $(CURRENT_BUILD_DIR)/,$(_TARGET))

.PHONY: all

# 注意：手册构建需要 pygmentize 支持代码高亮

all: $(TARGET) | $(BUILD_DIR)
# 将 main.pdf 复制到项目构建目录，并重命名为指定的手册文件名
	Copy-Item -Path "$(TARGET)" -Destination "$(BUILD_DIR)/$(MANUAL_NAME)" -Force
$(TARGET): $(SOURCE)
# build main.pdf
	xelatex --shell-escape -8bit --output-dir=build main.tex
# build main.pdf again to ensure references are correct
	xelatex --shell-escape -8bit --output-dir=build main.tex
$(BUILD_DIR):
	New-Item -Type Directory -Path $(BUILD_DIR) -Force