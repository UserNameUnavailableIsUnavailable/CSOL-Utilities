# Note:
# If you use vcpkg bundled in Visual Studio from Visual Studio's Developer Shell, then you do not have to manually set `VCPKG_ROOT`.

cmake_minimum_required(VERSION 3.20)

message(STATUS "Controller CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Controller CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

set(DEPENDENCIES_DIR "${CMAKE_CURRENT_BINARY_DIR}/dependencies")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(CLI11 CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(Clipper2 CONFIG REQUIRED)
find_package(onnxruntime CONFIG REQUIRED)
find_package(aho_corasick CONFIG REQUIRED)

include_directories(
    Controller
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DEPENDENCIES_DIR}/include
)
get_target_property(OPENCV_INCLUDES opencv_core INTERFACE_INCLUDE_DIRECTORIES)

add_executable(
    Controller
    source/Main.cpp
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

target_link_libraries(Controller
    PRIVATE
    onnxruntime::onnxruntime
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    Clipper2::Clipper2
    CLI11::CLI11
)

target_precompile_headers(Controller
    PRIVATE
    include/pch.hpp
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

message(STATUS "Dependencies: ${dependencies}")

if (dependencies)
    add_custom_command(TARGET Controller POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dependencies}
            "$<TARGET_FILE_DIR:Controller>"
    )

# embed manifest into Controller.exe
add_custom_command(TARGET Controller POST_BUILD
    COMMAND mt -manifest "${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml" "-outputresource:\"$<TARGET_FILE:Controller>;#1\""
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
    COMMAND pwsh -NoProfile -NoLogo -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/models.ps1" -InstallDir "$<TARGET_FILE_DIR:Controller>/models"
)
endif()