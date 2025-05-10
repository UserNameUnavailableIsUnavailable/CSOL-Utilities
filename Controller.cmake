cmake_minimum_required(VERSION 3.0)

project(CSOL-Utilities)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    build/Controller.exe
    WIN32
    Controller/implementation/Main.cpp
    Controller/implementation/CSOL-Utilities.cpp
    Controller/implementation/OCR.cpp
    Controller/implementation/clipper.cpp
    Controller/implementation/Command.cpp
    Controller/implementation/CommandDispatcher.cpp
    Controller/implementation/Console.cpp
    Controller/implementationCrnnNet.cpp
    Controller/implementation/DbNet.cpp
    Controller/implementation/GameIdlerEngine.cpp
    Controller/implementation/GameProcessDetector.cpp
    Controller/implementation/OcrLite.cpp
    Controller/implementation/Recognition.cpp
    Controller/implementation/Signal.cpp
)


