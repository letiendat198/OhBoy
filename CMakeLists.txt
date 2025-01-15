cmake_minimum_required(VERSION 3.29)
project(OhBoy)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

add_library(OhBoy STATIC
        src/cpu.cpp
        src/opcode.cpp
        src/memory.cpp
        src/cartridge.cpp
        src/util.cpp
        src/opcode_prefixed.cpp
        src/ppu.cpp
        src/interrupts.cpp
        src/timer.cpp
        src/dma.cpp
        src/joypad.cpp
        src/scheduler.cpp
        include/scheduler.h
        src/mbc.cpp
)

set_property(TARGET OhBoy PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

include_directories("include")
