VPATH := $(SOURCE)/include $(SOURCE)/$(MOD) $(BUILD)/$(MOD)
HEADERS := CConsole CController CDateTime CEvent CEventList CException CInGameState CMessenger CSOL_Utilities CLI11
IMPLEMENTATIONS := main Win32Util WatchInGameState WatchGameProcess HandleHotKeyEvent DispatchFixedCommand CMessenger CDateTime CController CConsole
OBJECTS := $(addsuffix .obj, $(IMPLEMENTATIONS))
.PHONY: all
all: $(addsuffix .hpp, $(HEADERS)) $(addsufix .cpp, $(IMPLEMENTATIONS)) $(OBJECTS)
	clang++ -o $(BUILD)/$(MOD)/$(MOD).exe $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS)) -lkernel32 -luser32 -ladvapi32 -lshell32 -municode
%.obj: %.cpp
	clang++ -g3 -c -std=c++17 -o $(BUILD)/$(MOD)/$@ $(SOURCE)/$(MOD)/$< --include-directory=$(SOURCE)/include