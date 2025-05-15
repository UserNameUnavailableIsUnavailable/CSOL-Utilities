SHELL = pwsh.exe
export .SHELLFLAGS = -NoProfile -Command

export ROOT := $(shell (Get-Location).ToString() -replace("\\", "/"))

export DEPENDENCIES := $(ROOT)/dependencies
export TEST = $(ROOT)/test
export DOCS = $(ROOT)/documents
export Configuration = Release
export BUILD = $(ROOT)/build

PROJECT_NAME = CSOL-Utilities
MAIN_VERSION = 1
SUB_VERSION = 5
REVISION_VERSION = 2
VERSION = v$(MAIN_VERSION).$(SUB_VERSION).$(REVISION_VERSION)
PLATFORM = Win64
DISTRO = $(PROJECT_NAME)-$(VERSION)-$(PLATFORM)


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
	Copy-Item -Destination $(BUILD)/$(Configuration) -Path $(ROOT)/Install.ps1 -Force
	Copy-Item -Destination $(BUILD)/$(Configuration) -Path $(ROOT)/Controller.ps1 -Force
Executor:
	if (Test-Path $(BUILD)/$(Configuration)/$@) { Remove-Item $(BUILD)/$(Configuration)/$@ -Force -Recurse }
	Copy-Item -Destination $(BUILD)/$(Configuration) -Path $@ -Recurse -Force
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
	if (Test-Path "$(BUILD)/$(DISTRO)") { Remove-Item -Force -Recurse "$(BUILD)/$(DISTRO)" }
	New-Item -Type Directory -Path "$(BUILD)/$(DISTRO)" -Force
	Out-File -FilePath "$(BUILD)/$(DISTRO)/VERSION.txt" -InputObject "$(VERSION)"
	Copy-Item -Force -Destination "$(BUILD)/$(DISTRO)" -Path "$(BUILD)/$(Configuration)/Install.ps1"
	Copy-Item -Force -Destination "$(BUILD)/$(DISTRO)" -Path "$(BUILD)/$(Configuration)/Controller.ps1"
	New-Item -Type Directory -Path "$(BUILD)/$(DISTRO)/Controller" -Force
	foreach ($$item in (Get-ChildItem -Path "$(BUILD)/$(Configuration)/Controller" -Exclude "*.pdb")) <#\
	#> { Copy-Item -Force -Destination "$(BUILD)/$(DISTRO)/Controller" -Path $$item.FullName -Recurse }
	Copy-Item -Force -Destination $(BUILD)/$(DISTRO) -Path "$(BUILD)/$(Configuration)/Executor" -Recurse
	Copy-Item -Force -Destination $(BUILD)/$(DISTRO) -Path "$(BUILD)/GamingTool/*"
	Compress-Archive -DestinationPath $(BUILD)/$(DISTRO).zip -Path $(BUILD)/$(DISTRO)/* -Force
Clean:
	Remove-Item -Force -Recurse -Path $(BUILD)
