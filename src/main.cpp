#include <iostream>
#include <algorithm>
#include "cpu.h"

using namespace std;

Cpu cpu;

int main(){
    cpu.init();
    cout<<"Starting GB emulator"<<endl;

    FILE *f = fopen("../roms/07-jr,jp,call,ret,rst.gb", "rb");
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    cout<<"ROM size: "<<size<<endl;
    if(size>0){
        fseek(f, 0L, SEEK_SET);
        unsigned char *buf = (unsigned char*)malloc(size);
        fread(buf, size,1, f);
        fclose(f);

        cpu.cartridge.init(buf, size); //Never use sizeof(pointer) as it's always 8 bytes, the real malloc size is lost. Pass raw size
        free(buf);

        cout<<"Successfully load rom into cartridge"<<endl;

        cout<<"Loading first part of cartridge into memory"<<endl;
        cpu.memory.init();
        copy(cpu.cartridge.address+0,cpu.cartridge.address+0x3fff, cpu.memory.address); //SEGFAULT. Do not try to use sizeof because that the size of the pointer itself
        copy(cpu.cartridge.address+0x4000,cpu.cartridge.address+0x7fff, cpu.memory.address+0x4000);
        cout<<"Loaded cartridge into memory bus"<<endl;
    }
    cpu.exec();
    return 0;
}