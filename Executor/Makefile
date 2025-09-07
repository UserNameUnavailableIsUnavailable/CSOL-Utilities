BUILD_DIR = build
SUBDIR = $(BUILD_DIR)/Executor
SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command
SOURCE_FILES = $(wildcard *.lua)
INSTALLER = $(BUILD_DIR)/Install.ps1
TARGET_FILES = $(addprefix $(SUBDIR)/,$(notdir $(SOURCE_FILES))) $(INSTALLER)

all: $(TARGET_FILES) | $(SUBDIR)

$(SUBDIR)/%.lua: %.lua | $(SUBDIR)
	Copy-Item -Path $< -Destination $@ -Force

$(INSTALLER): Install.ps1 | $(SUBDIR)
	Copy-Item -Path $< -Destination $@ -Force

$(SUBDIR):
	New-Item -Type Directory -Path $(SUBDIR) -Force