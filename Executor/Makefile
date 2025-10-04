BUILD_DIR = build
SUBDIR = $(BUILD_DIR)/Executor
SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command
SOURCE_FILES = $(wildcard *.lua)
INSTALLER = Install.ps1 Install.cmd
TARGET_FILES = $(addprefix $(SUBDIR)/,$(notdir $(SOURCE_FILES))) $(addprefix $(BUILD_DIR)/,$(INSTALLER))

all: $(TARGET_FILES) | $(SUBDIR)
# 移除 Executor.lua，该文件由 Install.ps1 运行后创建
	if (Test-Path "$(BUILD_DIR)/Executor.lua") { Remove-Item -Force "$(BUILD_DIR)/Executor.lua" }

$(SUBDIR)/%.lua: %.lua | $(SUBDIR)
	Copy-Item -Path $< -Destination $@ -Force

$(BUILD_DIR)/%.ps1: %.ps1 | $(BUILD_DIR)
	Copy-Item -Path $< -Destination $@ -Force

$(BUILD_DIR)/%.cmd: %.cmd | $(BUILD_DIR)
	Copy-Item -Path $< -Destination $@ -Force

$(BUILD_DIR):
	New-Item -Type Directory -Path $(BUILD_DIR) -Force

$(SUBDIR): | $(BUILD_DIR)
	New-Item -Type Directory -Path $(SUBDIR) -Force