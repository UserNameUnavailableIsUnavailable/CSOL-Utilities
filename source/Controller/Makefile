VPATH := $(BUILD)/$(MOD)
SUBMODULES := command game global util csol24except
OBJECTS := $(addsuffix .obj, $(SUBMODULES))
.PHONY: all
all: $(OBJECTS)
	lld-link /lib /out:$(BUILD)/Controller.obj $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS))
%.obj: %.cpp
	clang++ -g -c -o $(BUILD)/$(MOD)/$@ $(SOURCE)/Controller/$< --include-directory=$(SOURCE)/include
