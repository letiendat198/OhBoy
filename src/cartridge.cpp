#include "cartridge.h"
#include <string.h>
#include <cstdlib>

void Cartridge::init(unsigned char* buf, int size){
    address = (unsigned char*)malloc(size);
    memcpy(address,buf, size);
}