ifeq ("$(OS)", "Windows_NT")
	SHELL := pwsh.exe # Windows 下需要带 .exe 才能正确搜索到
else
	SHELL := pwsh # Unix 下直接用 pwsh
endif
.SHELLFLAGS := -NoProfile -NoLogo -Command