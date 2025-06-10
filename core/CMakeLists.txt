add_library(OhBoy
        src/cpu.cpp
        src/opcode.cpp
        src/memory.cpp
        src/cartridge.cpp
        src/logger.cpp
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

# set_property(TARGET OhBoy PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/spdlog)
target_link_libraries(OhBoy spdlog)

target_include_directories(OhBoy PUBLIC "include")
target_include_directories(OhBoy PRIVATE "deps/spdlog/include")