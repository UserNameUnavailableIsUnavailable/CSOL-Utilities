ifndef CMAKE_MK
CMAKE_MK := 1

CMAKE_SOURCE_DIR ?=
CMAKE_BINARY_DIR ?=
CMAKE_INSTALL_PREFIX ?=
CMAKE_CONFIG_TYPE ?=

CMAKE_PLATFORM := x64
CMAKE_TOOLSET := host=x64
# generator **MUST** support multi-config, otherwise the build type will be ignored and all targets will be built in Debug mode by default, which is not what we want.
# Since this project runs on Windows only, Visual Studio 17 2022 or higher is recommended.
CMAKE_GENERATOR := Visual Studio 18 2026

# These settings are mandatory
ifndef CMAKE_SOURCE_DIR
$(error CMAKE_SOURCE_DIR is not defined!)
endif
ifndef CMAKE_BINARY_DIR
$(error CMAKE_BINARY_DIR is not defined!)
endif
ifndef VERSION
$(error VERSION is not defined!)
endif
ifndef CMAKE_CONFIG_TYPE
$(error CMAKE_CONFIG_TYPE is not defined!)
endif

CMAKE_CONFIGURE_ARGS = -S "$(CMAKE_SOURCE_DIR)" -B "$(CMAKE_BINARY_DIR)" -G "$(CMAKE_GENERATOR)" -A "$(CMAKE_PLATFORM)" -T "$(CMAKE_TOOLSET)" -DVERSION="$(VERSION)"

CMAKE_BUILD_ARGS = --build $(CMAKE_BINARY_DIR) --config "$(CMAKE_CONFIG_TYPE)"

CMAKE_INSTALL_ARGS = --install $(CMAKE_BINARY_DIR) --config "$(CMAKE_CONFIG_TYPE)"

# customize install dir
ifdef CMAKE_INSTALL_PREFIX
CMAKE_INSTALL_ARGS += --prefix "$(CMAKE_INSTALL_PREFIX)" 
endif

endif # !CMAKE_MK
