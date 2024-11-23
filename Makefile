SHELL = pwsh.exe
export .SHELLFLAGS = -NoProfile -Command
export ROOT := $(shell (Get-Location).ToString() -replace("\\", "/"))
export SOURCE = $(ROOT)/source
export DEPENDENCIES := $(ROOT)/dependencies
export TEST= $(ROOT)/test
export DOCS = $(ROOT)/docs
export BUILD = $(ROOT)/build

PROJECT_NAME = CSOL_Utilities
VERSION = v1.4.5
ARCH = x86_64

MODULES = Controller Docs Executor Ps1 Web
VPATH = source
TEST_UNIT := check_file

.PHONY: $(MODULES) all

# link everything
all: MODULES
Ps1:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Install.ps1 -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Controller.ps1 -Force
Executor:
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
Docs:
	New-Item -Type Directory -Path $(BUILD)/Docs -Force
	(New-Item -Type Directory -Force -Path $(BUILD)/$@).Attributes += "Hidden"
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/Docs $(DOCS)/main.tex
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/Docs $(DOCS)/main.tex
	Rename-Item -NewName "$(BUILD)/Docs/`u{4F7F}`u{7528}`u{624B}`u{518C}.pdf" -Path $(BUILD)/Docs/main.pdf
Web:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/ConfigWebPages -Recurse -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Setting.html -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Weapon.html -Force
Pack:
	New-Item -Type Directory -Path $(BUILD)/pack -Force
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Install.ps1"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/ConfigWebPages","$(BUILD)/Setting.html","$(BUILD)/Weapon.html" -Recurse
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Executor","$(BUILD)/Main.lua" -Recurse
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Controller.exe","$(BUILD)/Controller.ps1"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/GamingTool.dll","$(BUILD)/GamingTool.exe"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(DEPENDENCIES)/*" -Recurse
	Compress-Archive -DestinationPath $(BUILD)/$(PROJECT_NAME)-$(VERSION)-$(ARCH).zip -Path $(BUILD)/pack/* -Force
Clean:
	Remove-Item -Force -Recurse -Path $(BUILD)