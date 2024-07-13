SHELL = pwsh.exe
export .SHELLFLAGS = -Noprofile -Command
export ROOT := $(shell (Get-Location).ToString() -replace("\\", "/"))
export SOURCE = $(ROOT)/source
export TEST= $(ROOT)/test
export BUILD = $(ROOT)/build

PROJECT_NAME = CSOL24H

MODULES = Controller Test
VPATH = source
TEST_UNIT := check_file



.PHONY: $(MODULES) all

# link everything
all:

# compile and link test
Test:
	clang++ -g -o $(BUILD)/$(TEST_UNIT).exe $(TEST)/$(TEST_UNIT).cpp $(BUILD)/Controller.obj -lkernel32 -luser32 -lAdvapi32 --include-directory=$(SOURCE)/include
# compile Controller
Controller:
	New-Item -Type Directory -Path $(BUILD)/$@ -Force
	$(MAKE) --directory=$(SOURCE)/$@ SHELL="$(SHELL)" MOD=$@
	clang++ -g $(BUILD)/Controller.obj -o $(BUILD)/Controller.exe
clean:
	Remove-Item -Force -Recurse -Path $(BUILD)