# 注意：手册构建需要 pygmentize 支持代码高亮

SOURCE_DIR := ..
BUILD_DIR := ../build
DIST_DIR := ../dist

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Manual
CURRENT_BUILD_DIR = $(BUILD_DIR)/Manual
MANUAL_NAME := CSOL-Utilities

_SOURCE := main.tex $(wildcard chapter_*.tex)
SOURCE = $(addprefix $(CURRENT_SOURCE_DIR)/,$(_SOURCE))
# 生成的手册，main.tex -> main.pdf
TARGET := $(CURRENT_BUILD_DIR)/main.pdf

include $(SOURCE_DIR)/make/pwsh.mk # 使用 PowerShell 作为 Makefile 的 shell

.PHONY: all build clean install

all: build install

build: $(TARGET)

install: $(TARGET) | $(DIST_DIR)
	Copy-Item -Path "$(TARGET)" -Destination "$(DIST_DIR)/$(MANUAL_NAME)" -Force
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue
$(TARGET): $(SOURCE) | $(CURRENT_BUILD_DIR)
# build main.pdf
	xelatex --shell-escape -8bit --output-dir=$(CURRENT_BUILD_DIR) main.tex
# build main.pdf again to ensure references are correct
	xelatex --shell-escape -8bit --output-dir=$(CURRENT_BUILD_DIR) main.tex
$(CURRENT_BUILD_DIR):
	New-Item -Type Directory -Path $(CURRENT_BUILD_DIR) -Force
$(DIST_DIR):
	New-Item -Type Directory -Path $(DIST_DIR) -Force
