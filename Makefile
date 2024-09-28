SHELL = pwsh.exe
export .SHELLFLAGS = -Noprofile -Command
export ROOT := $(shell (Get-Location).ToString() -replace("\\", "/"))
export SOURCE = $(ROOT)/source
export TEST= $(ROOT)/test
export DOCS = $(ROOT)/docs
export BUILD = $(ROOT)/build

PROJECT_NAME = CSOL24H

MODULES = Controller Docs Executor Ps1 ConfigWebPages
VPATH = source
TEST_UNIT := check_file

.PHONY: $(MODULES) all

# link everything
all: MODULES
Ps1:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/install.ps1 -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Controller.ps1 -Force
Executor:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Executor -Recurse -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Main.lua
# compile and link test
Test:
	clang++ -g -o $(BUILD)/$(TEST_UNIT).exe $(TEST)/$(TEST_UNIT).cpp $(BUILD)/Controller.obj -lkernel32 -luser32 -lAdvapi32 --include-directory=$(SOURCE)/include
# compile Controller
Controller:
	New-Item -Type Directory -Path $(BUILD)/$@ -Force
	$(MAKE) --directory=$(SOURCE)/$@ SHELL="$(SHELL)" MOD=$@
	Move-Item -Force -Destination $(BUILD) -Path $(BUILD)/$@/$@.exe
Docs:
	New-Item -Type Directory -Force -Path $(BUILD)/$@
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/docs $(DOCS)/main.tex
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/docs $(DOCS)/main.tex
Web:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/ConfigWebPages -Recurse -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Setting.html -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Weapon.html -Force
Pack:
Clean:
	Remove-Item -Force -Recurse -Path $(BUILD)