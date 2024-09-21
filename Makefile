SHELL = pwsh.exe
export .SHELLFLAGS = -Noprofile -Command
export ROOT := $(shell (Get-Location).ToString() -replace("\\", "/"))
export SOURCE = $(ROOT)/source
export TEST= $(ROOT)/test
export DOCS = $(ROOT)/docs
export BUILD = $(ROOT)/build

PROJECT_NAME = CSOL24H

MODULES = Controller Test docs lua ps1 web
VPATH = source
TEST_UNIT := check_file

.PHONY: $(MODULES) all

# link everything
all: Controller lua docs ps1
ps1:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/install.ps1 -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Controller.ps1 -Force
lua:
	New-Item -Type Directory -Path $(BUILD)/lua -Force
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/lua -Recurse -Force
# compile and link test
Test:
	clang++ -g -o $(BUILD)/$(TEST_UNIT).exe $(TEST)/$(TEST_UNIT).cpp $(BUILD)/Controller.obj -lkernel32 -luser32 -lAdvapi32 --include-directory=$(SOURCE)/include
# compile Controller
Controller:
	New-Item -Type Directory -Path $(BUILD)/$@ -Force
	$(MAKE) --directory=$(SOURCE)/$@ SHELL="$(SHELL)" MOD=$@
	clang++ -g3 $(BUILD)/Controller.obj -o $(BUILD)/Controller.exe
docs:
	New-Item -Type Directory -Force -Path $(BUILD)/$@
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/docs $(DOCS)/main.tex
web:
	Copy-Item -Destination $(BUILD) -Path $(SOURCE)/Web -Recurse -Force
clean:
	Remove-Item -Force -Recurse -Path $(BUILD)