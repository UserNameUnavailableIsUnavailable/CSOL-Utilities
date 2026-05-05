# Note:
# If you use vcpkg bundled in Visual Studio from Visual Studio's Developer Shell, then you do not have to manually set `VCPKG_ROOT`.

cmake_minimum_required(VERSION 3.20)

message(STATUS "Controller CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Controller CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(CLI11 CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(Clipper2 CONFIG REQUIRED)
find_package(onnxruntime CONFIG REQUIRED)
find_package(aho_corasick CONFIG REQUIRED)
get_target_property(OPENCV_INCLUDES opencv_core INTERFACE_INCLUDE_DIRECTORIES)

add_library(
    ControllerLib
    source/Configuration.cpp
    source/Configuration/Game.cpp
    source/Configuration/Locale.cpp
    source/Driver.cpp
    source/Command.cpp
    source/CommandDispatcher.cpp
    source/Console.cpp
    source/CSOBannerSuppressor.cpp
    source/Global.cpp
    source/Hotkey.cpp
    source/LowLevelKeyboardHook.cpp
    source/IdleEngine.cpp
    source/ModelUtilities.cpp
    source/OCR.cpp
    source/OCR_Detector.cpp
    source/OCR_Recognizer.cpp
    source/OCRIdleEngine.cpp
    source/OCRUtilities.cpp
    source/pch.cpp
    source/Utilities.cpp
    source/Classifier.cpp
    source/ClassifierIdleEngine.cpp
)

target_include_directories(
    ControllerLib
    PUBLIC # make ControllerLib available to ControllerTesting
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    ControllerLib
    PUBLIC
    onnxruntime::onnxruntime
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    Clipper2::Clipper2
    CLI11::CLI11
)

target_compile_definitions(
    ControllerLib
    PUBLIC
    CONTROLLER_VERSION="${VERSION}"
)

Message(STATUS "Version defined in ControllerLib: ${VERSION}")

target_precompile_headers(
    ControllerLib
    PRIVATE
    include/pch.hpp
)

add_executable(
    Controller
    Controller.rc # rc file contains manifest
    source/Main.cpp
)

target_link_libraries(
    Controller
    PRIVATE
    ControllerLib
)

target_precompile_headers(Controller REUSE_FROM ControllerLib)

# since the rc file already includes a manifest, we need to disable automatic manifest generation to avoid conflicts
target_link_options(
    Controller PRIVATE /MANIFEST:NO
)

list(APPEND dependencies "C:/Windows/System32/vcruntime140.dll")
list(APPEND dependencies "C:/Windows/System32/vcruntime140_1.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140_1.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140_atomic_wait.dll")
file(GLOB dll_files "${DEPENDENCIES_DIR}/bin/*.dll")
file(GLOB pdb_files "${DEPENDENCIES_DIR}/bin/*.pdb")
list(APPEND dependencies ${dll_files})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND dependencies ${pdb_files})
endif()

if (BUILD_TESTING)
    add_subdirectory(testing)
endif()

message(STATUS "Dependencies: ${dependencies}")

if (dependencies)
    add_custom_command(TARGET Controller POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dependencies}
            "$<TARGET_FILE_DIR:Controller>"
    )

# assets: locales
# copy ${CMAKE_CURRENT_SOURCE_DIR}/locales to $<TARGET_FILE_DIR:Controller>/locales
add_custom_command(TARGET Controller POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/locales"
        "$<TARGET_FILE_DIR:Controller>/locales"
)

# assets: models
# run models.ps1 to fetch models (models located in ${CMAKE_CURRENT_SOURCE_DIR}/models)
# then copy these models to $<TARGET_FILE_DIR:Controller>/models
add_custom_command(TARGET Controller POST_BUILD
    COMMAND pwsh -NoProfile -NoLogo -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/models.ps1" -DownloadDir "${DOWNLOADS_DIR}/models" -InstallDir "$<TARGET_FILE_DIR:Controller>/models"
)

endif()

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
    DESTINATION .
    COMPONENT Controller
)
