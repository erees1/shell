cmake_minimum_required(VERSION 3.20.5)
set(CMAKE_CXX_STANDARD 20)
project(shell)

# Set src (our code root) as include path (to allow imports relative to src) and add it's contents
include_directories(src)
file(GLOB SOURCES "src/*.cpp")
add_executable(shell ${SOURCES})
