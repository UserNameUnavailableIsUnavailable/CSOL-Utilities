PROJECT := CSOL-Utilities
DISTRO = $(PROJECT)-$(VERSION)
# Get the version from VERSION.txt and trim whitespace
VERSION = $(strip $(file < VERSION.txt))

# SOURCE_DIR refers to the source root of the whole project
SOURCE_DIR := .
# CURRENT_SOURCE_DIR refers to the source root of the current target to build
CURRENT_SOURCE_DIR = $(SOURCE_DIR)
BUILD_TYPE := Release
# BUILD_DIR refers to the build directory of the whole project
BUILD_DIR := ./build
# CURRENT_BUILD_DIR refers to the build directory of the current target
CURRENT_BUILD_DIR = $(BUILD_DIR)
DIST_ROOT := ./dist
# DIST_DIR refers to the project-level distribution directory, from which we make bundles
DIST_DIR = $(DIST_ROOT)/$(DISTRO)
# CURRENT_DIST_DIR refers to the target's distribution directory
CURRENT_DIST_DIR = $(DIST_DIR)
# manual's name
MANUAL_NAME = $(DISTRO).pdf
# the bundle to release
BUNDLE_NAME = $(DISTRO).zip
# AutoHotkey Compiler
AHK2EXE_PATH := C:/Program Files/AutoHotkey/Compiler/Ahk2Exe.exe

TARGETS := Controller Executor Tool Manual Bundle

# We use paths relative to the project root across all Makefiles for consistency.
# We prefix SOURCE_DIR, BUILD_DIR, DIST_DIR, etc. with ../ and pass them to sub-Makefiles, so that sub-Makefiles can also use paths relative to the project root.
SUB_MAKE_ARGS = SOURCE_DIR="../$(SOURCE_DIR)" \
	CURRENT_SOURCE_DIR="../$(SOURCE_DIR)/$@" \
	BUILD_DIR="../$(BUILD_DIR)" \
	CURRENT_BUILD_DIR="../$(BUILD_DIR)/$@" \
	DIST_ROOT="../$(DIST_ROOT)" \
	DIST_DIR="../$(DIST_DIR)" \
	CURRENT_DIST_DIR="../$(CURRENT_DIST_DIR)/$@" \
	BUILD_TYPE="$(BUILD_TYPE)" \
	VERSION="$(VERSION)"

# Use PowerShell v7+ as the shell for this Makefile
include $(SOURCE_DIR)/make/pwsh.mk
# proxy settings
include $(SOURCE_DIR)/make/proxy.mk

# Sometimes we may want to run all commands in one shell session.
# For example, we may want keep all testing logs in one session.
# We can do this by enabling ONESHELL.
# ifdef ONESHELL
# .ONESHELL:
# endif
.PHONY: $(TARGETS)

# TIP: We may add phony targets on demand
# For clarity, targets representing project modules start with uppercase, others are lowercased.
# default target
.PHONY: all
all: $(TARGETS)

Controller: | $(BUILD_DIR) $(DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Controller" $(SUB_MAKE_ARGS)

Executor: | $(BUILD_DIR) $(DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Executor" $(SUB_MAKE_ARGS)

Tool: | $(BUILD_DIR) $(DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Tool" $(SUB_MAKE_ARGS) AHK2EXE_PATH="$(AHK2EXE_PATH)"

Manual: | $(BUILD_DIR) $(DIST_DIR)
	$(MAKE) --directory="$(SOURCE_DIR)/Manual" MANUAL_NAME="$(MANUAL_NAME)" $(SUB_MAKE_ARGS)

Bundle: | $(BUILD_DIR) $(DIST_DIR)
	Compress-Archive -Path "$(DIST_DIR)/*" -DestinationPath "$(DIST_ROOT)/$(BUNDLE_NAME)" -Force

.PHONY: testing
TESTABLE_TARGETS := Controller
# If set, test cases will be built but not executed.
BUILD_ONLY :=
__targets := $(filter $(TESTABLE_TARGETS),$(TARGETS))
testing: $(addprefix testing-,$(__targets))

.PHONY: testing-%
testing-%:
	@Write-Host "Running tests for $*..."
	$(MAKE) --directory="$(SOURCE_DIR)/$*" $(SUB_MAKE_ARGS) testing

# verbose
.PHONY: verbose
verbose:
# PowerShell does not use `\` as line seperator, but Makefile need it.
# We need to add an `#` to comment `\`.
	@ # \
	Write-Host "Project: $(PROJECT)" -ForegroundColor GREEN # \
	Write-Host "Targets: $(TARGETS)" -ForegroundColor GREEN # \
	Write-Host "Shell: $(SHELL)" -ForegroundColor GREEN # \
	Write-Host "Version: $(VERSION)" -ForegroundColor GREEN # \
	Write-Host "HTTP_PROXY: $(HTTP_PROXY)" -ForegroundColor GREEN # \
	Write-Host "HTTPS_PROXY: $(HTTPS_PROXY)" -ForegroundColor GREEN # \
	Write-Host "Source directory: $(SOURCE_DIR)" -ForegroundColor GREEN # \
	Write-Host "Build directory: $(BUILD_DIR)" -ForegroundColor GREEN # \
	Write-Host "Distrution directory: $(DIST_DIR)" -ForegroundColor GREEN # \
	Write-Host "Bundle: $(BUNDLE_NAME)" -ForegroundColor GREEN

.PHONY: clean
clean:
	Remove-Item -Force -Recurse -Path $(BUILD_DIR)

$(BUILD_DIR):
	New-Item -Type Directory -Path "$(BUILD_DIR)" -Force
$(DIST_DIR):
	New-Item -Type Directory -Path "$(DIST_DIR)" -Force
