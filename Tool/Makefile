SHELL := pwsh.exe
.SHELLFLAGS := -NoLogo -NoProfile -Command

SOURCE_DIR := source
BUILD_DIR := build
DLL_SOURCE_DIR := $(SOURCE_DIR)/dll
EXE_SOURCE_DIR := $(SOURCE_DIR)/exe
CXX_SOURCE_FILES = $(wildcard $(DLL_SOURCE_DIR)/*.cpp)
CXX_OBJECT_FILES = $(addprefix $(BUILD_DIR)/,$(notdir $(patsubst %.cpp,%.obj,$(CXX_SOURCE_FILES))))
AHK_SOURCE_FILE = $(EXE_SOURCE_DIR)/Main.ahk
TOOL_DLL = $(BUILD_DIR)/Tool.dll
TOOL_EXE = $(BUILD_DIR)/Tool.exe
# AHK2EXE 是 GUI 程序，使用 Start-Process 调用以等待其完成，否则执行将是异步的，进而导致后续命令找不到生成的文件
AHK2EXE = Start-Process -FilePath "C:/Program Files/AutoHotkey/Compiler/Ahk2Exe.exe" -Wait -ArgumentList

.PHONY: all clean dll exe

all: $(TOOL_DLL) $(TOOL_EXE) | $(BUILD_DIR)
$(TOOL_DLL): $(CXX_OBJECT_FILES) | $(BUILD_DIR)
	lld-link /out:$@ /DLL /subsystem:console $(CXX_OBJECT_FILES) user32.lib kernel32.lib
$(TOOL_EXE): $(AHK_SOURCE_FILE) | $(BUILD_DIR)
# AHK2EXE 的 /out 参数在指定相对路径时，相对路径被认为是相对于源文件所在目录，而不是相对于当前工作目录，因此这里不用
# 变通方案：先让可执行文件直接输出到源文件目录下，再移动到目标目录
	$(AHK2EXE) "/in $(AHK_SOURCE_FILE)"
	Move-Item "$(EXE_SOURCE_DIR)/Main.exe" $@ -Force
$(BUILD_DIR)/%.obj: $(SOURCE_DIR)/dll/%.cpp | $(BUILD_DIR)
	clang-cl /c $< /GX /o $@
$(BUILD_DIR):
	New-Item -Type Directory -Path $(BUILD_DIR)
dll: $(TOOL_DLL) | $(BUILD_DIR)
exe: $(TOOL_EXE) | $(BUILD_DIR)
clean:
	Remove-Item -Recurse $(BUILD_DIR)