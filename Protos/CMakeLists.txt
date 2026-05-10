cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "vcpkg toolchain file")

project(Protos CXX)

find_package(protobuf CONFIG REQUIRED)

set(MODULES Controller)

foreach(MODULE ${MODULES})
    file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.proto")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/gen/Protos/${MODULE}")

    protobuf_generate(
        LANGUAGE cpp
        OUT_VAR MODULE_GEN
        PROTOS ${PROTO_FILES}
        PROTOC_OUT_DIR "${CMAKE_BINARY_DIR}/gen/Protos"
        # import proto source files from the current source directory
        IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        VERBOSE
    )

    list(APPEND PROTO_GEN_FILES ${MODULE_GEN})
endforeach()

add_custom_target(GenerateProtos ALL DEPENDS ${PROTO_GEN_FILES})
