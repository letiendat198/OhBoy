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

add_library(OhBoy
        core/src/cpu.cpp
        core/src/opcode.cpp
        core/src/memory.cpp
        core/src/cartridge.cpp
        core/src/logger.cpp
        core/src/opcode_prefixed.cpp

        core/src/ppu.cpp
        core/src/interrupt.cpp
        core/src/timer.cpp
        core/src/dma.cpp
        core/src/joypad.cpp
        core/src/scheduler.cpp
        core/include/scheduler.h
        core/src/mbc.cpp
        core/src/apu.cpp
)

add_executable(OhBoyDebugger
        debugger/main.cpp
        debugger/debugger.cpp
        deps/imgui/imgui.cpp
        deps/imgui/imgui_demo.cpp
        deps/imgui/imgui_draw.cpp
        deps/imgui/imgui_tables.cpp
        deps/imgui/imgui_widgets.cpp
        deps/imgui_club/imgui_memory_editor/imgui_memory_editor.h
        deps/imgui/misc/cpp/imgui_stdlib.cpp
        deps/imgui/backends/imgui_impl_sdl2.cpp
        deps/imgui/backends/imgui_impl_sdlrenderer2.cpp
        deps/popl/include/popl.hpp
)

set_property(TARGET OhBoy PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

include(FetchContent)
FetchContent_Declare(
        Corrosion
        GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
        GIT_TAG v0.5 # Optionally specify a commit hash, version tag or branch here
)
FetchContent_MakeAvailable(Corrosion)
# Import targets defined in a package or workspace manifest `Cargo.toml` file
corrosion_import_crate(MANIFEST_PATH debugger/gui/Cargo.toml)
add_dependencies(OhBoyDebugger OhBoyGUI)
add_dependencies(OhBoyDebugger OhBoy)
target_link_libraries(OhBoyDebugger OhBoy)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/spdlog)
target_link_libraries(OhBoy spdlog)
target_link_libraries(OhBoyDebugger spdlog)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/fmt)
target_link_libraries(OhBoyDebugger fmt::fmt)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") 
    target_link_libraries(OhBoyDebugger ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2-VC/lib/x64/SDL2main.lib)
    target_link_libraries(OhBoyDebugger ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2-VC/lib/x64/SDL2.lib)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
    target_link_libraries(OhBoyDebugger ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/x86_64-w64-mingw32/lib/libSDL2main.a)
    target_link_libraries(OhBoyDebugger ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/x86_64-w64-mingw32/lib/libSDL2.dll.a)
else ()
    find_package(SDL2 REQUIRED)
    target_link_libraries(OhBoyDebugger ${SDL2_LIBRARIES})
endif()

target_include_directories(OhBoy PRIVATE "core/include")
target_include_directories(OhBoy PRIVATE "deps/spdlog/include")
target_include_directories(OhBoyDebugger PRIVATE "core/include")
target_include_directories(OhBoyDebugger PRIVATE "deps/imgui")
target_include_directories(OhBoyDebugger PRIVATE "deps/imgui/backends")
target_include_directories(OhBoyDebugger PRIVATE "deps/imgui/misc/cpp")
target_include_directories(OhBoyDebugger PRIVATE "deps/imgui_club/imgui_memory_editor")
target_include_directories(OhBoyDebugger PRIVATE "deps/popl/include")
target_include_directories(OhBoyDebugger PRIVATE "deps/spdlog/include")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") 
    target_include_directories(OhBoyDebugger PRIVATE "deps/SDL2-VC/include")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
    target_include_directories(OhBoyDebugger PRIVATE "deps/SDL2/x86_64-w64-mingw32/include/SDL2")
else ()
    target_include_directories(OhBoyDebugger PRIVATE ${SDL2_INCLUDE_DIRS})
endif()