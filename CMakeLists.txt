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

add_library(OhBoy STATIC
        src/cpu.cpp
        src/opcode.cpp
        src/memory.cpp
        src/cartridge.cpp
        src/opcode_prefixed.cpp
        src/ppu.cpp
        src/interrupt.cpp
        src/timer.cpp
        src/dma.cpp
        src/joypad.cpp
        src/scheduler.cpp
        include/scheduler.h
        src/mbc.cpp
        src/apu.cpp
)

set_property(TARGET OhBoy PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

include_directories("include")
