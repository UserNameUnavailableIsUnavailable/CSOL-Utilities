VPATH := $(SOURCE)/include $(DEPENDENCIES) $(SOURCE)/$(MOD) $(BUILD)/$(MOD)
HEADERS := CConsole CController CDateTime CEvent CEventList CException CInGameState CMessenger CSOL_Utilities ExecutorCommand CLI11 aho_corasick/aho_corasick
C_HEADERS := OCR/OcrLiteCApi
IMPLEMENTATIONS := Main Win32Util AutoPlayStateMachine WatchGameProcess HandleHotKeyEvent DispatchFixedCommand CMessenger CDateTime CController CConsole ExecutorCommand
LIBS := RapidOcrOnnx.lib
OBJECTS := $(addsuffix .obj, $(IMPLEMENTATIONS))
.PHONY: all
all: $(addsuffix .hpp, $(HEADERS)) $(addsuffix .h, $(C_HEADERS)) $(addsufix .cpp, $(IMPLEMENTATIONS)) $(OBJECTS) $(LIBS)
	clang++ -g3 -o $(BUILD)/$(MOD)/$(MOD).exe $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS)) $(addprefix $(DEPENDENCIES)/, $(LIBS)) -lkernel32 -luser32 -ladvapi32 -lshell32 -lgdi32 -municode
%.obj: %.cpp
	clang++ -g3 -c -std=c++17 -o $(BUILD)/$(MOD)/$@ $(SOURCE)/$(MOD)/$< --include-directory=$(SOURCE)/include