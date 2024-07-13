VPATH := $(BUILD)/$(MOD)
SUBMODULES := main command CSOL24H_EXCEPT CSOL24H DateTime Game State util HotKey
OBJECTS := $(addsuffix .obj, $(SUBMODULES))
.PHONY: all
all: $(OBJECTS)
	lld-link /lib /out:$(BUILD)/Controller.obj $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS))
%.obj: %.cpp
	clang++ -g -c -o $(BUILD)/$(MOD)/$@ $(SOURCE)/Controller/$< --include-directory=$(SOURCE)/include
