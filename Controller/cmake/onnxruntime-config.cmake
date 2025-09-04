# onnxruntimeConfig.cmake
include("${CMAKE_CURRENT_LIST_DIR}/onnxruntime.cmake")
add_library(onnxruntime::onnxruntime STATIC IMPORTED)
set_target_properties(onnxruntime::onnxruntime PROPERTIES
    IMPORTED_LOCATION "${DEPENDENCIES_DIR}/lib/onnxruntime.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${DEPENDENCIES_DIR}/include"
    IMPORTED_IMPLIB "${DEPENDENCIES_DIR}/lib/onnxruntime.lib"
    IMPORTED_DLL "${DEPENDENCIES_DIR}/bin/onnxruntime.dll"
)