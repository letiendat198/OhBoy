cmake_minimum_required(VERSION 3.29)
project(OhBoy)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif ()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/debugger)

target_compile_definitions(OhBoy PRIVATE OHBOY_ENABLE_LOGGING)

target_link_libraries(OhBoyDebugger OhBoy)