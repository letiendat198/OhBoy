add_executable(OhBoyDebugger
        main.cpp
        debugger.cpp
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

include(FetchContent)
FetchContent_Declare(
        Corrosion
        GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
        GIT_TAG v0.5 # Optionally specify a commit hash, version tag or branch here
)
FetchContent_MakeAvailable(Corrosion)
# Import targets defined in a package or workspace manifest `Cargo.toml` file
corrosion_import_crate(MANIFEST_PATH gui/Cargo.toml)

add_dependencies(OhBoyDebugger OhBoyGUI)

# OhBoy core may have already built this dependency
if (NOT TARGET spdlog::spdlog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/spdlog)
endif ()
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

# Debugger only need core headers to compile. BUT REMEMBER TO LINK CORE WITH DEBUGGER AFTER
target_include_directories(OhBoyDebugger PRIVATE "../core/include")
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