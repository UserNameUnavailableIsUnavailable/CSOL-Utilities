ifndef CMAKE_MK
CMAKE_MK := 1

CMAKE_SOURCE_DIR ?=
CMAKE_BINARY_DIR ?=
CMAKE_INSTALL_PREFIX ?=
CMAKE_CONFIG_TYPE ?=

# 检查上述字段是否已定义
ifndef CMAKE_SOURCE_DIR
$(error CMAKE_SOURCE_DIR is not defined!)
endif
ifndef CMAKE_BINARY_DIR
$(error CMAKE_BINARY_DIR is not defined!)
endif
ifndef CMAKE_INSTALL_PREFIX
$(error CMAKE_INSTALL_PREFIX is not defined!)
endif
ifndef CMAKE_CONFIG_TYPE
$(error CMAKE_CONFIG_TYPE is not defined!)
endif
ifndef VERSION
$(error VERSION is not defined!)
endif

# CMake settings
export CMAKE_SUPPORT_MULTI_CONFIG := true
export CMAKE_PLATFORM := x64
export CMAKE_TOOLSET := host=x64
# generator **MUST** support multi-config, otherwise the build type will be ignored and all targets will be built in Debug mode by default, which is not what we want.
# Since this project runs on Windows, Visual Studio 17 2022 or higher is recommended.
export CMAKE_GENERATOR := Visual Studio 18 2026
export CMAKE_CONFIGURE_ARGS = -S "$(CMAKE_SOURCE_DIR)" -B "$(CMAKE_BINARY_DIR)" -G "$(CMAKE_GENERATOR)" -A "$(CMAKE_PLATFORM)" -T "$(CMAKE_TOOLSET)" -DVERSION="$(VERSION)"
export CMAKE_BUILD_ARGS = --build $(CMAKE_BINARY_DIR) --config "$(CMAKE_CONFIG_TYPE)"
export CMAKE_INSTALL_ARGS = --install $(CMAKE_BINARY_DIR) --prefix "$(CMAKE_INSTALL_PREFIX)" --config "$(CMAKE_CONFIG_TYPE)"

endif # !CMAKE_MK
