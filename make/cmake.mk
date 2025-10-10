# CMake 设定
CMAKE_SUPPORT_MULTI_CONFIG := true
export CMAKE_PLATFORM := x64
export CMAKE_TOOLSET := host=x64
export CMAKE_GENERATOR := Visual Studio 17 2022
export CMAKE_BUILD_TYPE := $(BUILD_TYPE)
ifeq ($(CMAKE_SUPPORT_MULTI_CONFIG), true)
# For multi-config generators like Visual Studio, specify `--config BUILD_TYPE` during build
export CMAKE_BUILD_ARGS := --config "$(BUILD_TYPE)"
else
# For single-config generators like Ninja, specify `-DCMAKE_BUILD_TYPE=BUILD_TYPE` during configure
export CMAKE_CONFIGURE_ARGS := -DCMAKE_BUILD_TYPE="$(BUILD_TYPE)"
endif

