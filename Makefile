SHELL = pwsh.exe
.SHELLFLAGS = -Noprofile -Command
PROJECT_NAME = CSOL24H
PROJECT_PATH := $(shell (Get-Location).ToString() -replace("\\", "/"))
SOURCE_PATH = source
BUILD_PATH = build
MODULES = Console.lua Craft.lua Delay.lua Keyboard.lua Mouse.lua Player.lua Round.lua Runtime.lua Setting.lua Store.lua Utility.lua Weapon.lua main.lua
VPATH = source
.PHONY : $(MODULES) all clean
CSOL24H.LUA :
	New-Item -Type Directory -Path "$(PROJECT_PATH)/$(BUILD_PATH)" -Force
	Out-File -InputObject "local PATH = `"$(PROJECT_PATH)/`"" -FilePath "$(PROJECT_PATH)/$(BUILD_PATH)/$@"
	Out-File -Append -InputObject "dofile(PATH .. `"$(SOURCE_PATH)/main.lua`")" -FilePath "$(PROJECT_PATH)/$(BUILD_PATH)/$@"
all : $(MODULES)
$(MODULES):
	(Get-Content -Path "$(PROJECT_PATH)/$(SOURCE_PATH)/$@") -replace("local\sPATH\s=\s`".*`"", "local PATH = `"$(PROJECT_PATH)/`"") | Out-File -FilePath "$(PROJECT_PATH)/$(SOURCE_PATH)/$@"
clean : 
	Remove-Item -Path "$(PROJECT_PATH)/$(BUILD_PATH)" -Recurse -Force