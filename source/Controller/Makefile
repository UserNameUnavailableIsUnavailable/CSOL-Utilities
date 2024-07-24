VPATH := $(BUILD)/$(MOD)
SUBMODULES := main Command CSOL24H_EXCEPT CSOL24H DateTime HotKey Util GameState GameProcess Console CombineParts PurchaseItem LocateCursor
OBJECTS := $(addsuffix .obj, $(SUBMODULES))
.PHONY: all
all: $(OBJECTS)
	lld-link /lib /out:$(BUILD)/Controller.obj $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS))
%.obj: %.cpp
	clang++ -g -c -o $(BUILD)/$(MOD)/$@ $(SOURCE)/Controller/$< --include-directory=$(SOURCE)/include