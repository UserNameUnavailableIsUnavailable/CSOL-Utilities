VPATH := $(BUILD)/$(MOD)
# SUBMODULES := Main Command CSOL24H_EXCEPT CSOL24H DateTime HotKey Util GameState GameProcess Console CombineParts PurchaseItem LocateCursor StopCSOBanner
SUBMODULES := WinMain Win32Util WatchInGameState WatchGameProcess HandleHotKeyEvent DispatchFixedCommand CMessenger CDateTime CController CConsole
OBJECTS := $(addsuffix .obj, $(SUBMODULES))
.PHONY: all
all: $(OBJECTS)
	lld-link /lib /out:$(BUILD)/Controller.obj $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS))
%.obj: %.cpp
	clang++ -g3 -c -o $(BUILD)/$(MOD)/$@ $(SOURCE)/Controller/$< --include-directory=$(SOURCE)/include