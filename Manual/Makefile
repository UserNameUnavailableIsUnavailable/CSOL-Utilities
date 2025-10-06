# 注意：手册构建需要 pygmentize 支持代码高亮

SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command

SOURCE_DIR := ..
CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Manual
BUILD_DIR := ../build
CURRENT_BUILD_DIR = $(BUILD_DIR)/Manual
DIST_DIR := ../dist
MANUAL_NAME := CSOL-Utilities

_SOURCE := main.tex $(wildcard chapter_*.tex)
SOURCE = $(addprefix $(CURRENT_SOURCE_DIR)/,$(_SOURCE))
# 生成的手册，main.tex -> main.pdf
TARGET := $(CURRENT_BUILD_DIR)/main.pdf

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