VPATH := $(BUILD)/$(MOD)
SUBMODULES := command game global util
OBJECTS := $(addsuffix .obj, $(SUBMODULES))

all: $(OBJECTS)
	lld-link /lib /out:$(BUILD)/Controller.obj $(addprefix $(BUILD)/$(MOD)/, $(OBJECTS)) /subsystem:windows
%.obj: %.cpp
	clang++ -g -c -o $(BUILD)/$(MOD)/$@ $(SOURCE)/Controller/$< --include-directory=$(SOURCE)/include
