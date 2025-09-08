BUILD_DIR = build
SUBDIR = $(BUILD_DIR)/Executor
SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command
SOURCE_FILES = $(wildcard *.lua)
INSTALLER = $(BUILD_DIR)/Install.ps1
TARGET_FILES = $(addprefix $(SUBDIR)/,$(notdir $(SOURCE_FILES))) $(INSTALLER)

all: $(TARGET_FILES) | $(SUBDIR)
# 移除 Executor.lua，该文件由 Install.ps1 运行后创建
	if (Test-Path "$(BUILD_DIR)/Executor.lua") { Remove-Item -Force "$(BUILD_DIR)/Executor.lua" }
$(SUBDIR)/%.lua: %.lua | $(SUBDIR)
	Copy-Item -Path $< -Destination $@ -Force

$(INSTALLER): Install.ps1 | $(SUBDIR)
	Copy-Item -Path $< -Destination $@ -Force
$(SUBDIR):
	New-Item -Type Directory -Path $(SUBDIR) -Force