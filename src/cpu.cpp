#include "cpu.h"

#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

void (Cpu::*jump_table[])() =
        {Cpu::op_00, Cpu::op_01, Cpu::op_02, Cpu::op_03, Cpu::op_04, Cpu::op_05, Cpu::op_06, Cpu::op_07, Cpu::op_08, Cpu::op_09, Cpu::op_0A, Cpu::op_0B, Cpu::op_0C, Cpu::op_0D, Cpu::op_0E, Cpu::op_0F,
        Cpu::op_10, Cpu::op_11, Cpu::op_12, Cpu::op_13, Cpu::op_14, Cpu::op_15, Cpu::op_16, Cpu::op_17, Cpu::op_18, Cpu::op_19, Cpu::op_1A, Cpu::op_1B, Cpu::op_1C, Cpu::op_1D, Cpu::op_1E, Cpu::op_1F,
        Cpu::op_20, Cpu::op_21, Cpu::op_22, Cpu::op_23, Cpu::op_24, Cpu::op_25, Cpu::op_26, Cpu::op_27, Cpu::op_28, Cpu::op_29, Cpu::op_2A, Cpu::op_2B, Cpu::op_2C, Cpu::op_2D, Cpu::op_2E, Cpu::op_2F,
        Cpu::op_30, Cpu::op_31, Cpu::op_32, Cpu::op_33, Cpu::op_34, Cpu::op_35, Cpu::op_36, Cpu::op_37, Cpu::op_38, Cpu::op_39, Cpu::op_3A, Cpu::op_3B, Cpu::op_3C, Cpu::op_3D, Cpu::op_3E, Cpu::op_3F,
        Cpu::op_40, Cpu::op_41, Cpu::op_42, Cpu::op_43, Cpu::op_44, Cpu::op_45, Cpu::op_46, Cpu::op_47, Cpu::op_48, Cpu::op_49, Cpu::op_4A, Cpu::op_4B, Cpu::op_4C, Cpu::op_4D, Cpu::op_4E, Cpu::op_4F,
        Cpu::op_50, Cpu::op_51, Cpu::op_52, Cpu::op_53, Cpu::op_54, Cpu::op_55, Cpu::op_56, Cpu::op_57, Cpu::op_58, Cpu::op_59, Cpu::op_5A, Cpu::op_5B, Cpu::op_5C, Cpu::op_5D, Cpu::op_5E, Cpu::op_5F,
        Cpu::op_60, Cpu::op_61, Cpu::op_62, Cpu::op_63, Cpu::op_64, Cpu::op_65, Cpu::op_66, Cpu::op_67, Cpu::op_68, Cpu::op_69, Cpu::op_6A, Cpu::op_6B, Cpu::op_6C, Cpu::op_6D, Cpu::op_6E, Cpu::op_6F,
        Cpu::op_70, Cpu::op_71, Cpu::op_72, Cpu::op_73, Cpu::op_74, Cpu::op_75, Cpu::op_76, Cpu::op_77, Cpu::op_78, Cpu::op_79, Cpu::op_7A, Cpu::op_7B, Cpu::op_7C, Cpu::op_7D, Cpu::op_7E, Cpu::op_7F,
        Cpu::op_80, Cpu::op_81, Cpu::op_82, Cpu::op_83, Cpu::op_84, Cpu::op_85, Cpu::op_86, Cpu::op_87, Cpu::op_88, Cpu::op_89, Cpu::op_8A, Cpu::op_8B, Cpu::op_8C, Cpu::op_8D, Cpu::op_8E, Cpu::op_8F,
        Cpu::op_90, Cpu::op_91, Cpu::op_92, Cpu::op_93, Cpu::op_94, Cpu::op_95, Cpu::op_96, Cpu::op_97, Cpu::op_98, Cpu::op_99, Cpu::op_9A, Cpu::op_9B, Cpu::op_9C, Cpu::op_9D, Cpu::op_9E, Cpu::op_9F,
        Cpu::op_A0, Cpu::op_A1, Cpu::op_A2, Cpu::op_A3, Cpu::op_A4, Cpu::op_A5, Cpu::op_A6, Cpu::op_A7, Cpu::op_A8, Cpu::op_A9, Cpu::op_AA, Cpu::op_AB, Cpu::op_AC, Cpu::op_AD, Cpu::op_AE, Cpu::op_AF,
        Cpu::op_B0, Cpu::op_B1, Cpu::op_B2, Cpu::op_B3, Cpu::op_B4, Cpu::op_B5, Cpu::op_B6, Cpu::op_B7, Cpu::op_B8, Cpu::op_B9, Cpu::op_BA, Cpu::op_BB, Cpu::op_BC, Cpu::op_BD, Cpu::op_BE, Cpu::op_BF,
        Cpu::op_C0, Cpu::op_C1, Cpu::op_C2, Cpu::op_C3, Cpu::op_C4, Cpu::op_C5, Cpu::op_C6, Cpu::op_C7, Cpu::op_C8, Cpu::op_C9, Cpu::op_CA, Cpu::op_CB, Cpu::op_CC, Cpu::op_CD, Cpu::op_CE, Cpu::op_CF,
        Cpu::op_D0, Cpu::op_D1, Cpu::op_D2, Cpu::op_D3, Cpu::op_D4, Cpu::op_D5, Cpu::op_D6, Cpu::op_D7, Cpu::op_D8, Cpu::op_D9, Cpu::op_DA, Cpu::op_DB, Cpu::op_DC, Cpu::op_DD, Cpu::op_DE, Cpu::op_DF,
        Cpu::op_E0, Cpu::op_E1, Cpu::op_E2, Cpu::op_E3, Cpu::op_E4, Cpu::op_E5, Cpu::op_E6, Cpu::op_E7, Cpu::op_E8, Cpu::op_E9, Cpu::op_EA, Cpu::op_EB, Cpu::op_EC, Cpu::op_ED, Cpu::op_EE, Cpu::op_EF,
        Cpu::op_F0, Cpu::op_F1, Cpu::op_F2, Cpu::op_F3, Cpu::op_F4, Cpu::op_F5, Cpu::op_F6, Cpu::op_F7, Cpu::op_F8, Cpu::op_F9, Cpu::op_FA, Cpu::op_FB, Cpu::op_FC, Cpu::op_FD, Cpu::op_FE, Cpu::op_FF};

void Cpu::init(){
    memory.init();
    a = 0x01;
    b = 0x00;
    c = 0x13;
    d = 0x00;
    e = 0xD8;
    h = 0x01;
    l = 0x4D;
    sp = 0xFFFE;
    pc = 0x0100;
    z_flag = 1;
    n_flag = 0;
    h_flag = 1;
    c_flag = 1;
}

std::string Cpu::craft_debug() {
    std::ostringstream ss;
    ss << std::uppercase << std::hex;
    ss << "A:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) a<<" ";
    ss << "F:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) (z_flag<<7 | n_flag << 6 | h_flag << 5 | c_flag<<4) <<" ";
    ss << "B:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) b<<" ";
    ss << "C:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) c<<" ";
    ss << "D:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) d<<" ";
    ss << "E:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) e<<" ";
    ss << "H:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) h<<" ";
    ss << "L:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) l<<" ";
    ss << "SP:"<< std::setfill ('0') << std::setw(sizeof(u_short)*2) <<(int) sp<<" ";
    ss << "PC:"<< std::setfill ('0') << std::setw(sizeof(u_short)*2) <<(int) pc<<" ";
    ss << "PCMEM:"<< std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) read8_mem(pc)<<",";
    ss << std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) read8_mem(pc+1)<<",";
    ss << std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) read8_mem(pc+2)<<",";
    ss << std::setfill ('0') << std::setw(sizeof(u_char)*2) <<(int) read8_mem(pc+3);
    ss << "\n";
    return ss.str();
}


void Cpu::exec() {
    while(true) {
        tick();
        _sleep(1/1000000);
    }
}

bool Cpu::tick(){
    // cout<<"Current PC: "<<(int)pc<<" Remaining M-Cycle: "<<(int) mcycle<<" OP skip: "<<(int)opskip<<endl;
    if (mcycle) {
        mcycle--;
        return true;
    }
    printf("%.4X %.2X\n", pc, memory.address[pc]);
    pc+=opskip;
    (this->*jump_table[memory.address[pc]])();
    logger.write(craft_debug());
    return true;
}

void Cpu::write8_mem(u_short addr, u_char a) {
    memory.address[addr] = a;
}

u_char Cpu::read8_mem(u_short addr) {
    return memory.address[addr];
}

u_short Cpu::read16_mem(u_short addr) {
    return memory.address[addr] << 8 | memory.address[addr+1];
}

void Cpu::ld8_imm(u_char &a, u_char imm) {
    a = imm;
}

void Cpu::ld16_imm(u_char &a, u_char &b, u_short imm) {
    a = imm >> 8;
    b = imm & 0xff;
}

void Cpu::inc8(u_char &a) {
    a = a+1;

    if ((a & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = a ? 0 : 1;
    n_flag = 0;
}

void Cpu::inc16(u_char &a, u_char &b) {
    u_short t = a << 8 | b;
    t++;
    a = t >> 8;
    b = t & 0xF;
}

void Cpu::inc_hl() {
    u_char data = read8_mem(h<<8|l);
    data += 1;
    write8_mem(h<<8|l, data);
    if ((data & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 0;
}

void Cpu::dec8(u_char &a) {
    a = a-1;

    if ((a & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = a ? 0 : 1;
    n_flag = 1;
}

void Cpu::dec16(u_char &a, u_char &b) {
    u_short t = a << 8 | b;
    t--;
    a = t >> 8;
    b = t & 0xF;
}

void Cpu::dec_hl() {
    u_char data = read8_mem(h<<8|l);
    data -= 1;
    write8_mem(h<<8|l, data);
    if ((data & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 1;
}

