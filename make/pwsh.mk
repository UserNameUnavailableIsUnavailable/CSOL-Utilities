ifndef PWSH_MK
PWSH_MK := 1

ifeq ($(OS),Windows_NT)
# We need .exe suffix on Windows to ensure we find the correct executable
PWSH := pwsh.exe
else
# On Unix, use pwsh directly
PWSH := pwsh
endif

# Try to use $(PWSH) as the shell for the Makefile, if $(PWSH) does not exist, it will not change
SHELL := $(PWSH)
.SHELLFLAGS := -NoProfile -NoLogo -NonInteractive -Command

# Based on the assignment of $(SHELL), check if $(PWSH) exists, if not, report an error before the build starts
# $(SHELL) may contain spaces, first use $(lastword) to extract the last part of the space-separated path, then extract the file name
ifneq ($(notdir $(lastword $(SHELL))), $(PWSH))
$(error "Cannot find PowerShell executable: $(PWSH). Please install $(PWSH) and ensure it is in your PATH.")
endif

endif # !PWSH_MK
