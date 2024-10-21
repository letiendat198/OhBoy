#include "cartridge.h"

#include <memory.h>

FILE* Cartridge::f;
int Cartridge::size = 0;

bool Cartridge::init(const char* file){
    f = fopen(file, "rb");
    fseek(f, 0L, SEEK_END);
    size = ftell(f);
    std::cout<<"ROM size: "<<size<<std::endl;
    if(size<=0) return false;
    return true;
}

void Cartridge::read_to_mem() {
    u_short read_len = 0x7FFF+1;
    fseek(f, 0L, SEEK_SET);

    fread(Memory::get_raw(), sizeof(u_char),read_len, f);
}

void Cartridge::close() {
    fclose(f);
}

