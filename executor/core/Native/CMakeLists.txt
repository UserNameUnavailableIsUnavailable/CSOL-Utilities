add_library(
    Native_impl
    STATIC
    Types.hpp
    Multiplexer.hpp
    Multiplexer.cpp
    Channel.hpp
    Channel.cpp
    Timer.hpp
    Timer.cpp
    TimerChannel.hpp
    TimerChannel.cpp
)

add_library(
    Native
    SHARED
    init.cpp
)

target_link_libraries(
    Native
    Native_impl
)
