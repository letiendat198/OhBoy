#include "memory.h"
#include <stdlib.h>

void Memory::init(){
    address = (unsigned char*)malloc((0xFFFF+1)*sizeof(char));
}