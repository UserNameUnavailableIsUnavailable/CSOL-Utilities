# 这些字段必须在 include 之前定义
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

# CMake 设定
export CMAKE_SUPPORT_MULTI_CONFIG := true
export CMAKE_PLATFORM := x64
export CMAKE_TOOLSET := host=x64
# 生成器 **必须** 支持多配置
export CMAKE_GENERATOR := Visual Studio 17 2022
export CMAKE_CONFIGURE_ARGS = -S "$(CMAKE_SOURCE_DIR)" -B "$(CMAKE_BINARY_DIR)" -G "$(CMAKE_GENERATOR)" -A "$(CMAKE_PLATFORM)" -T "$(CMAKE_TOOLSET)"
export CMAKE_BUILD_ARGS = --build $(CMAKE_BINARY_DIR) --config "$(CMAKE_CONFIG_TYPE)"
export CMAKE_INSTALL_ARGS = --install $(CMAKE_BINARY_DIR) --prefix "$(CMAKE_INSTALL_PREFIX)"