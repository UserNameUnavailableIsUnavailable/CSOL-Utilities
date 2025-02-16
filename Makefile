SHELL = pwsh.exe
export .SHELLFLAGS = -NoProfile -Command
export ROOT := $(shell (Get-Location).ToString() -replace("\\", "/"))
export SOURCE = $(ROOT)/source
export DEPENDENCIES := $(ROOT)/dependencies
export TEST= $(ROOT)/test
export DOCS = $(ROOT)/documents
export BUILD = $(ROOT)/build

PROJECT_NAME = CSOL_Utilities
VERSION = v1.5.1-preview

ARCH = x86_64

MODULES = Controller Documents Executor Ps1 Web
VPATH = source
TEST_UNIT := check_file

.PHONY: $(MODULES) all

# link everything
all: MODULES
Ps1:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Install.ps1 -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Controller.ps1 -Force
Executor:
	if (Test-Path $(BUILD)/Executor) { Remove-Item $(BUILD)/Executor -Recurse }
	if (Test-Path $(BUILD)/Main.lua) { Remove-Item $(BUILD)/Main.lua }
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Executor -Recurse -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Main.lua
# compile and link test
Test:
	clang++ -g -o $(BUILD)/$(TEST_UNIT).exe $(TEST)/$(TEST_UNIT).cpp $(BUILD)/Controller.obj -lkernel32 -luser32 -lAdvapi32 --include-directory=$(SOURCE)/include
# compile Controller
Controller:
	(New-Item -Type Directory -Path $(BUILD)/$@ -Force).Attributes += "Hidden"
	Write-Host $(MAKE)
	$(MAKE) --directory=$(SOURCE)/$@ SHELL="$(SHELL)" MOD=$@
	Move-Item -Force -Destination $(BUILD) -Path $(BUILD)/$@/$@.exe
Documents:
	New-Item -Type Directory -Path $(BUILD)/Documents -Force
	(New-Item -Type Directory -Force -Path $(BUILD)/$@).Attributes += "Hidden"
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/Documents $(DOCS)/main.tex
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/Documents $(DOCS)/main.tex
Panel:
	Copy-Item -Destination $(BUILD) -Path ConfigPanel -Recurse -Force
Pack:
	if (Test-Path $(BUILD)/pack) { Remove-Item -Force -Recurse $(BUILD)/pack }
	New-Item -Type Directory -Path $(BUILD)/pack -Force
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Install.ps1"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Executor","$(BUILD)/Main.lua" -Recurse
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Controller.exe","$(BUILD)/Controller.ps1"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/GamingTool.dll","$(BUILD)/GamingTool.exe"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(DEPENDENCIES)/*" -Recurse
	Compress-Archive -DestinationPath $(BUILD)/$(PROJECT_NAME)-$(VERSION)-$(ARCH).zip -Path $(BUILD)/pack/* -Force
Clean:
	Remove-Item -Force -Recurse -Path $(BUILD)
