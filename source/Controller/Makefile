VPATH := $(SOURCE)/include $(SOURCE)/$(MOD) $(BUILD)/$(MOD)
# IMPLEMENTATIONS := Main Command CSOL24H_EXCEPT CSOL24H DateTime HotKey Util GameState GameProcess Console CombineParts PurchaseItem LocateCursor StopCSOBanner
HEADERS := CConsole CController CDateTime CEvent CEventList CException CInGameState CMessenger CSOL_Utilities CLI11
IMPLEMENTATIONS := main Win32Util WatchInGameState WatchGameProcess HandleHotKeyEvent DispatchFixedCommand CMessenger CDateTime CController CConsole
OBJECTS := $(addsuffix .obj, $(IMPLEMENTATIONS))
.PHONY: all
all: $(addsuffix .hpp, $(HEADERS)) cmdline.h $(addsufix .cpp, $(IMPLEMENTATIONS)) $(OBJECTS)
	clang++ -o $(BUILD)/$(MOD)/$(MOD).exe $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS)) -lkernel32 -luser32 -llibcmt -ladvapi32
%.obj: %.cpp
	clang++ -g3 -c -std=c++17 -o $(BUILD)/$(MOD)/$@ $(SOURCE)/$(MOD)/$< --include-directory=$(SOURCE)/include