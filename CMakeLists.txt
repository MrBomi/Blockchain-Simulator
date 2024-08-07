cmake_minimum_required(VERSION 3.10)
project(LinuxEx2)

set(CMAKE_C_STANDARD 11)

# Find and link zlib
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
link_libraries(${ZLIB_LIBRARIES})

add_executable(LinuxEx2 main.c)

target_link_libraries(LinuxEx2 ${ZLIB_LIBRARIES})






