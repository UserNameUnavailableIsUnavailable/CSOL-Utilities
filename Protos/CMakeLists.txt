cmake_minimum_required(VERSION 3.20)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(MODULES Controller)

# About `protobuf_generate`
# See also: <https://cmake.org/cmake/help/latest/module/FindProtobuf.html#generating-source-files>

foreach(MODULE ${MODULES})
    file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.proto")
    set(MODULE_PROTO_OUT_DIR "${GENERATED_DIR}/Protos/${MODULE}")
    file(MAKE_DIRECTORY "${MODULE_PROTO_OUT_DIR}")
    set(MODULE_IMPORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}")

    # `PROTO_OUT_DIR`: The output directory for generated source files. Defaults to: `CMAKE_CURRENT_BINARY_DIR`.
    # `IMPORT_DIRS` <dirs>...: A list of one or more common parent directories for the schema files. For example, if the schema file is `proto/helloworld/helloworld.proto` and the import directory is `proto/`, then the generated files will be `<out-dir>/helloworld/helloworld.pb.h` and `<out-dir>/helloworld/helloworld.pb.cc`.

    protobuf_generate(
        LANGUAGE cpp
        OUT_VAR MODULE_GEN
        PROTOS ${PROTO_FILES}
        PROTOC_OUT_DIR "${MODULE_PROTO_OUT_DIR}"
        IMPORT_DIRS "${MODULE_IMPORT_DIR}"
    )

    protobuf_generate(
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        OUT_VAR MODULE_GRPC_GEN
        PROTOS ${PROTO_FILES}
        PROTOC_OUT_DIR "${MODULE_PROTO_OUT_DIR}"
        IMPORT_DIRS "${MODULE_IMPORT_DIR}"
        PLUGIN "protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    )

    list(APPEND PROTO_GEN_FILES ${MODULE_GEN})
    list(APPEND PROTO_GEN_FILES ${MODULE_GRPC_GEN})
endforeach()

add_custom_target(Protos ALL DEPENDS ${PROTO_GEN_FILES})

