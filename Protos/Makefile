SOURCE_DIR ?= ..
BUILD_DIR ?= ./build

CURRENT_SOURCE_DIR = $(SOURCE_DIR)/Protos
CURRENT_BUILD_DIR = $(BUILD_DIR)/gen/Protos

MODULES := Controller
MODULE_DIRS := $(addprefix $(CURRENT_BUILD_DIR)/,$(MODULES))


PROTO_SOURCE_FILES := $(foreach module,$(MODULES),$(wildcard $(CURRENT_SOURCE_DIR)/$(module)/*.proto))
__proto_build_files := $(patsubst $(CURRENT_SOURCE_DIR)/%,$(CURRENT_BUILD_DIR)/%,$(PROTO_SOURCE_FILES))
PROTO_CPP_FILES := $(__proto_build_files:.proto=.pb.cc) \
				   $(__proto_build_files:.proto=.pb.h)

PROTO_TARGETS := $(PROTO_CPP_FILES)

include $(SOURCE_DIR)/make/pwsh.mk

.PHONY: all
all: build

.PHONY: build
build: | $(CURRENT_BUILD_DIR) $(MODULE_DIRS) $(PROTO_TARGETS)

$(CURRENT_BUILD_DIR)/%.pb.cc $(CURRENT_BUILD_DIR)/%.pb.h: $(CURRENT_SOURCE_DIR)/%.proto
	protoc --proto_path="$(CURRENT_SOURCE_DIR)" --cpp_out="$(CURRENT_BUILD_DIR)" $<

.PHONY: clean
clean:
	Remove-Item -Recurse -Force "$(CURRENT_BUILD_DIR)" -ErrorAction SilentlyContinue

$(CURRENT_BUILD_DIR):
	New-Item -Type Directory -Path "$(CURRENT_BUILD_DIR)" -Force
$(MODULE_DIRS):
	New-Item -Type Directory -Path "$@" -Force
