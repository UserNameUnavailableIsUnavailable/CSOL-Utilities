VPATH := $(SOURCE)/include $(DEPENDENCIES) $(SOURCE)/$(MOD) $(BUILD)/$(MOD)
HEADERS := aho_corasick/aho_corasick CLI11 Console Controller CSOL_Utilities DateTime Event EventList Exception ExecutorCommand InGmeState Messenger
C_HEADERS := OCR/OcrLiteCApi
IMPLEMENTATIONS := Main AutoPlayStateMachine Console Controller DateTime DispatchFixedCommand EventHandler ExecutorCommand Messenger WatchGameProcess Win32Util
LIBS := RapidOcrOnnx.lib
OBJECTS := $(addsuffix .obj, $(IMPLEMENTATIONS))
.PHONY: all
all: $(addsuffix .hpp, $(HEADERS)) $(addsuffix .h, $(C_HEADERS)) $(addsufix .cpp, $(IMPLEMENTATIONS)) $(OBJECTS) $(LIBS)
	clang++ -g3 -o $(BUILD)/$(MOD)/$(MOD).exe $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS)) $(addprefix $(DEPENDENCIES)/, $(LIBS)) -lkernel32 -luser32 -ladvapi32 -lshell32 -lgdi32 -municode
%.obj: %.cpp
	clang++ -g3 -c -std=c++17 -o $(BUILD)/$(MOD)/$@ $(SOURCE)/$(MOD)/$< --include-directory=$(SOURCE)/include