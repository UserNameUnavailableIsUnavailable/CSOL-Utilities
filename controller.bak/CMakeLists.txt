cmake_minimum_required(VERSION 3.20)

project(Controller)

message(STATUS "Source dir: ${PROJECT_SOURCE_DIR}")

# Shared lib
add_subdirectory(source)
add_subdirectory(testing)

add_executable(
    Controller
    Main.cpp
    Controller.rc
)

# since the rc file already includes a manifest, we need to disable automatic manifest generation to avoid conflicts
target_link_options(
    Controller PRIVATE /MANIFEST:NO
)

target_link_libraries(
    Controller
    PRIVATE
    ControllerLib
)

if (BUILD_TESTING)
    add_test(NAME ControllerCLI.Help COMMAND Controller --help)
    set_tests_properties(ControllerCLI.Help PROPERTIES
        PASS_REGULAR_EXPRESSION "--classifier-model-json-path"
        FAIL_REGULAR_EXPRESSION "ocr"
    )
    add_test(NAME ControllerCLI.RejectOCREngine COMMAND Controller --idle-engine-type OCR)
    add_test(NAME ControllerCLI.RejectOCRDetector COMMAND Controller --ocr-detector-json-path unused)
    add_test(NAME ControllerCLI.RejectOCRRecognizer COMMAND Controller --ocr-recognizer-json-path unused)
    add_test(NAME ControllerCLI.RejectOCRKeywords COMMAND Controller --ocr-keywords-path unused)
    set_tests_properties(
        ControllerCLI.RejectOCREngine
        ControllerCLI.RejectOCRDetector
        ControllerCLI.RejectOCRRecognizer
        ControllerCLI.RejectOCRKeywords
        PROPERTIES WILL_FAIL TRUE
    )
endif()

# deal with deps

list(APPEND dependencies "C:/Windows/System32/vcruntime140.dll")
list(APPEND dependencies "C:/Windows/System32/vcruntime140_1.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140_1.dll")
list(APPEND dependencies "C:/Windows/System32/msvcp140_atomic_wait.dll")
message(STATUS "Dependencies: ${dependencies}")

if (dependencies)
    add_custom_command(TARGET Controller POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dependencies}
            "$<TARGET_FILE_DIR:Controller>"
    )
endif()

# assets: locales
# copy ${CMAKE_CURRENT_SOURCE_DIR}/locales to $<TARGET_FILE_DIR:Controller>/locales
add_custom_command(TARGET Controller POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/locales"
        "$<TARGET_FILE_DIR:Controller>/locales"
)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
    DESTINATION .
    #ignore *.lib, *.exp files
    COMPONENT Controller
    PATTERN "*.lib" EXCLUDE
    PATTERN "*.exp" EXCLUDE
)
