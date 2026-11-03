cmake_minimum_required(VERSION 3.20)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(
    MODULES
    Controller
)

add_library(
    Protocol STATIC
    "${generated_files}"
)

foreach(module ${MODULES})
    file(GLOB proto_files "${CMAKE_CURRENT_SOURCE_DIR}/${module}/*.proto")

    set(import_dir "${CMAKE_CURRENT_SOURCE_DIR}/${module}")

    # export to the same directory
    set(export_dir "${CMAKE_CURRENT_SOURCE_DIR}/${module}")

    # `PROTO_OUT_DIR`: The output directory for generated source files. Defaults to: `CMAKE_CURRENT_BINARY_DIR`.
    # `IMPORT_DIRS` <dirs>...: A list of one or more common parent directories for the schema files. For example, if the schema file is `proto/helloworld/helloworld.proto` and the import directory is `proto/`, then the generated files will be `<out-dir>/helloworld/helloworld.pb.h` and `<out-dir>/helloworld/helloworld.pb.cc`.

    # About `protobuf_generate`
    # See also: <https://cmake.org/cmake/help/latest/module/FindProtobuf.html#generating-source-files>

    protobuf_generate(
        TARGET Protocol
        LANGUAGE cpp
        OUT_VAR gen
        PROTOS ${proto_files}
        IMPORT_DIRS "${import_dir}"
        PROTOC_OUT_DIR "${export_dir}"
    )
    list(APPEND generated_files ${gen})

    protobuf_generate(
        TARGET Protocol
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        OUT_VAR gen
        PROTOS ${proto_files}
        PLUGIN "protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
        IMPORT_DIRS "${import_dir}"
        PROTOC_OUT_DIR "${export_dir}"
    )
    list(APPEND generated_files ${gen})

endforeach()

target_include_directories(
    Protocol PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(
    Protocol PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
    gRPC::grpc
)

