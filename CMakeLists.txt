cmake_minimum_required(VERSION 3.29)
project(OhBoy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_executable(OhBoy src/main.cpp
        src/cpu.cpp
        src/opcode.cpp
        src/memory.cpp
        src/cartridge.cpp
        src/logger.cpp
        lib/util.cpp
        src/opcode_prefixed.cpp
        deps/imgui/imgui.cpp
        deps/imgui/imgui_demo.cpp
        deps/imgui/imgui_draw.cpp
        deps/imgui/imgui_tables.cpp
        deps/imgui/imgui_widgets.cpp
        deps/imgui/imgui_memory_editor.h
        deps/imgui/misc/cpp/imgui_stdlib.cpp
        deps/imgui/backends/imgui_impl_sdl2.cpp
        deps/imgui/backends/imgui_impl_sdlrenderer2.cpp
        src/debugger.cpp
        include/config.h
        src/PPU.cpp
        include/PPU.h
        src/interrupts.cpp
        include/interrupts.h
        src/timer.cpp
        include/timer.h
        src/dma.cpp
        include/dma.h
        src/joypad.cpp
        include/joypad.h
        include/mbc.h
        include/mbc1.h
        src/mbc1.cpp
        include/mbc5.h
        src/mbc5.cpp
        deps/popl/include/popl.hpp)

# Enable ExternalProject CMake module
include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR})

ExternalProject_Add(
        OhBoyGUI
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/gui"
        DOWNLOAD_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND cargo build COMMAND cargo build --release
        BINARY_DIR "${CMAKE_SOURCE_DIR}/gui"
        INSTALL_COMMAND ""
        USES_TERMINAL_BUILD true
        LOG_BUILD ON)

add_dependencies(OhBoy OhBoyGUI)
configure_file("${CMAKE_SOURCE_DIR}/gui/target/release/OhBoyGUI.exe" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" COPYONLY)

target_link_libraries(OhBoy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/x86_64-w64-mingw32/lib/libSDL2main.a)
target_link_libraries(OhBoy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/x86_64-w64-mingw32/lib/libSDL2.dll.a)

include_directories("include")
include_directories("deps/imgui")
include_directories("deps/imgui/backends")
include_directories("deps/imgui/misc/cpp")
include_directories("deps/SDL2/x86_64-w64-mingw32/include/SDL2")
include_directories("deps/popl/include")