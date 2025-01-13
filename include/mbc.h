#ifndef MBC_H
#define MBC_H
#include <cstdint>

class Cartridge;

enum MBC_TYPES {
    MBC0,
    MBC1,
    MBC3,
    MBC5
};

class MBC {
private:
    Cartridge *cartridge;

    uint8_t rom_bank_low = 1;
    uint8_t rom_bank_high = 0;
    uint8_t ram_bank = 0;
    uint8_t bank_mode = 0;
public:
    MBC_TYPES mbc_type{};

    MBC(Cartridge *cartridge);
    void update_registers(uint16_t addr, uint8_t data);
};

#endif //MBC_H
