# a list of testing targets
set(testings "")

include(GoogleTest)
find_package(GTest CONFIG REQUIRED)

# Define the helper BEFORE adding subdirectories
function(register_test test)
    list(APPEND testings ${test})
    target_link_libraries(
        ${test}
        PRIVATE
        GTest::gtest_main
    )
    message(STATUS "Registered test: ${test}")
    gtest_discover_tests(${test})
endfunction()

add_subdirectory(controller)