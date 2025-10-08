ifeq ($(OS), Windows_NT)
# Windows 下需要带 .exe 才能正确搜索到
PWSH := pwsh.exe
else
# Unix 下直接用 pwsh
PWSH := pwsh
endif

# 尝试使用 $(PWSH) 作为 Makefile 的 shell，若 $(PWSH) 不存在则不会变更
SHELL := $(PWSH)
.SHELLFLAGS := -NoProfile -NoLogo -Command

# 基于 $(SHELL) 赋值的特点检查 $(PWSH) 是否存在，不存在则在构建开始前就报错
# $(SHELL) 可能包含空格，先用 $(lastword) 提取空格分隔开的路径的最后一部分，然后提取出文件名
ifneq ($(notdir $(lastword $(SHELL))), $(PWSH))
$(error "Cannot find PowerShell executable: $(PWSH). Please install $(PWSH) and ensure it is in your PATH.")
endif