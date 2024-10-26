#include "cartridge.h"

#include <debugger.h>
#include <format>
#include <memory.h>

bool Cartridge::init(const char* file){
    f = fopen(file, "rb");
    fseek(f, 0L, SEEK_END);
    size = ftell(f);
    Debugger::log(std::format("ROM size: {}", size).c_str());
    if(size<=0) return false;

    f_boot = fopen("../roms/boot.bin", "rb");
    fseek(f_boot, 0L, SEEK_END);
    boot_size = ftell(f_boot);
    Debugger::log(std::format("BOOT size: {}", boot_size).c_str());
    if(boot_size<=0) return false;
    return true;
}

void Cartridge::read_to_mem() {
    u_short read_len = 0x7FFF+1;
    fseek(f, 0L, SEEK_SET);

    fread(Memory::get_raw(), sizeof(u_char),read_len, f);
    Debugger::log(std::format("MBC chip: {:#X}", Memory::read(0x147)));

    fseek(f_boot, 0L, SEEK_SET);
    fread(Memory::get_boot_raw(), sizeof(u_char), boot_size + 1, f_boot);
}

void Cartridge::close() {
    fclose(f);
}

