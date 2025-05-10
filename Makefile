SHELL = pwsh.exe
export .SHELLFLAGS = -NoProfile -Command
export ROOT := $(shell (Get-Location).ToString() -replace("\\", "/"))
export DEPENDENCIES := $(ROOT)/dependencies
export TEST= $(ROOT)/test
export DOCS = $(ROOT)/documents
export BUILD = $(ROOT)/build

PROJECT_NAME = CSOL-Utilities
MAIN_VERSION = 1
SUB_VERSION = 5
REVISION_VERSION = 2
VERSION = v$(MAIN_VERSION).$(SUB_VERSION).$(REVISION_VERSION)

ARCH = x86_64

MODULES = Controller Documents Executor Ps1 Web ConfigPanel
TEST_UNIT := module

.PHONY: $(MODULES) all

# link everything
all: MODULES

ConfigPanel:
	tsc -p "$(ROOT)/ConfigPanel"
	Copy-Item -Recurse -Force -Destination $(BUILD)/ConfigPanel -Path "$(ROOT)/ConfigPanel/assets"
	Copy-Item -Recurse -Force -Destination $(BUILD)/ConfigPanel -Path "$(ROOT)/ConfigPanel/styles","$(ROOT)/ConfigPanel/weapon_templates"
	Copy-Item -Force -Destination $(BUILD)/ConfigPanel -Path "$(ROOT)/ConfigPanel/index.html","$(ROOT)/ConfigPanel/WeaponList.html","$(ROOT)/ConfigPanel/Setting.html"
	Copy-Item -Force -Destination $(BUILD)/ConfigPanel -Path "$(ROOT)/ConfigPanel/Setting.json","$(ROOT)/ConfigPanel/WeaponTemplateList.json"
Ps1:
	Copy-Item -Destination $(BUILD) -Path $(ROOT)/Install.ps1 -Force
	Copy-Item -Destination $(BUILD) -Path $(ROOT)/Controller.ps1 -Force
Executor:
	if (Test-Path $(BUILD)/$@) { Remove-Item $(BUILD)/$@ -Recurse }
	Copy-Item -Destination $(BUILD) -Path $@ -Recurse -Force
# compile and link test
Test:
	clang++ -g -o $(BUILD)/$(TEST_UNIT).exe $(TEST)/$(TEST_UNIT).cpp $(BUILD)/Controller.obj -lkernel32 -luser32 -lAdvapi32 --include-directory=$(ROOT)/include
# compile Controller
Controller:
	(New-Item -Type Directory -Path $(BUILD)/$@ -Force).Attributes += "Hidden"
	Write-Host $(MAKE)
	$(MAKE) --directory=$@ SHELL="$(SHELL)" MOD=$@
	Move-Item -Force -Destination $(BUILD) -Path $(BUILD)/$@/$@.exe
Documents:
	New-Item -Type Directory -Path $(BUILD)/Documents -Force
	(New-Item -Type Directory -Force -Path $(BUILD)/$@).Attributes += "Hidden"
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/Documents $(DOCS)/main.tex
	xelatex --shell-escape -8bit --output-dir=$(BUILD)/Documents $(DOCS)/main.tex
Pack:
	Out-File -FilePath $(BUILD)/VERSION.txt -InputObject "$(VERSION)"
	Copy-Item -Force -Destination $(BUILD)/pack -Path $(BUILD)/VERSION.txt
	if (Test-Path $(BUILD)/pack) { Remove-Item -Force -Recurse $(BUILD)/pack }
	New-Item -Type Directory -Path $(BUILD)/pack -Force
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Install.ps1"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Controller.exe","$(BUILD)/Controller.ps1"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/Executor" -Recurse
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(BUILD)/GamingTool.dll","$(BUILD)/GamingTool.exe"
	Copy-Item -Force -Destination $(BUILD)/pack -Path "$(DEPENDENCIES)/*" -Recurse
	Compress-Archive -DestinationPath $(BUILD)/$(PROJECT_NAME)-$(VERSION)-$(ARCH).zip -Path $(BUILD)/pack/* -Force
Clean:
	Remove-Item -Force -Recurse -Path $(BUILD)
