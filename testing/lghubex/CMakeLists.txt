add_executable(
    ControllerTest
    environment.cpp
    injector.cpp
)

target_link_libraries(
    ControllerTest
    PRIVATE
    ControllerLib
)
register_test(ControllerTest)