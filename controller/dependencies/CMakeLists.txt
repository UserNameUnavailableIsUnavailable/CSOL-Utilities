set(EXTENDER_DIR logitech-ghub-api-extender)

file(GLOB MINHOOK_SOURCES "${EXTENDER_DIR}/src/lib/minhook/**/*.c" "${EXTENDER_DIR}/src/lib/minhook/*.c")
file(GLOB MINHOOK_HEADERS "${EXTENDER_DIR}/src/lib/minhook/**/*.h" "${EXTENDER_DIR}/src/lib/minhook/*.h")
file(GLOB SDK "${EXTENDER_DIR}/src/lib/sdk/**/*.h" "${EXTENDER_DIR}/src/lib/sdk/*.h")
file(GLOB LUA "${EXTENDER_DIR}/src/lib/lua/*.h" "${EXTENDER_DIR}/src/lib/lua/*.c")
file(GLOB HOOKS "${EXTENDER_DIR}/src/lib/hooks/*.h" "${EXTENDER_DIR}/src/lib/hooks/*.cpp")

add_library(
    ExtenderLib
    INTERFACE
    ${EXTENDER_DIR}/src/lib/main.cpp
    ${EXTENDER_DIR}/src/lib/PatternScanner.h

    ${SDK}
    ${LUA}
    ${HOOKS}
    
    ${MINHOOK_SOURCES}
    ${MINHOOK_HEADERS}
)

target_include_directories(
    ExtenderLib
    INTERFACE
    ${EXTENDER_DIR}/src/lib
)