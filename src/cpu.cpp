#include "cpu.h"
#include "util.h"

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

void (Cpu::*jump_table_prefixed[])() =
        {Cpu::op_CB00, Cpu::op_CB01, Cpu::op_CB02, Cpu::op_CB03, Cpu::op_CB04, Cpu::op_CB05, Cpu::op_CB06, Cpu::op_CB07, Cpu::op_CB08, Cpu::op_CB09, Cpu::op_CB0A, Cpu::op_CB0B, Cpu::op_CB0C, Cpu::op_CB0D, Cpu::op_CB0E, Cpu::op_CB0F,
        Cpu::op_CB10, Cpu::op_CB11, Cpu::op_CB12, Cpu::op_CB13, Cpu::op_CB14, Cpu::op_CB15, Cpu::op_CB16, Cpu::op_CB17, Cpu::op_CB18, Cpu::op_CB19, Cpu::op_CB1A, Cpu::op_CB1B, Cpu::op_CB1C, Cpu::op_CB1D, Cpu::op_CB1E, Cpu::op_CB1F,
        Cpu::op_CB20, Cpu::op_CB21, Cpu::op_CB22, Cpu::op_CB23, Cpu::op_CB24, Cpu::op_CB25, Cpu::op_CB26, Cpu::op_CB27, Cpu::op_CB28, Cpu::op_CB29, Cpu::op_CB2A, Cpu::op_CB2B, Cpu::op_CB2C, Cpu::op_CB2D, Cpu::op_CB2E, Cpu::op_CB2F,
        Cpu::op_CB30, Cpu::op_CB31, Cpu::op_CB32, Cpu::op_CB33, Cpu::op_CB34, Cpu::op_CB35, Cpu::op_CB36, Cpu::op_CB37, Cpu::op_CB38, Cpu::op_CB39, Cpu::op_CB3A, Cpu::op_CB3B, Cpu::op_CB3C, Cpu::op_CB3D, Cpu::op_CB3E, Cpu::op_CB3F,
        Cpu::op_CB40, Cpu::op_CB41, Cpu::op_CB42, Cpu::op_CB43, Cpu::op_CB44, Cpu::op_CB45, Cpu::op_CB46, Cpu::op_CB47, Cpu::op_CB48, Cpu::op_CB49, Cpu::op_CB4A, Cpu::op_CB4B, Cpu::op_CB4C, Cpu::op_CB4D, Cpu::op_CB4E, Cpu::op_CB4F,
        Cpu::op_CB50, Cpu::op_CB51, Cpu::op_CB52, Cpu::op_CB53, Cpu::op_CB54, Cpu::op_CB55, Cpu::op_CB56, Cpu::op_CB57, Cpu::op_CB58, Cpu::op_CB59, Cpu::op_CB5A, Cpu::op_CB5B, Cpu::op_CB5C, Cpu::op_CB5D, Cpu::op_CB5E, Cpu::op_CB5F,
        Cpu::op_CB60, Cpu::op_CB61, Cpu::op_CB62, Cpu::op_CB63, Cpu::op_CB64, Cpu::op_CB65, Cpu::op_CB66, Cpu::op_CB67, Cpu::op_CB68, Cpu::op_CB69, Cpu::op_CB6A, Cpu::op_CB6B, Cpu::op_CB6C, Cpu::op_CB6D, Cpu::op_CB6E, Cpu::op_CB6F,
        Cpu::op_CB70, Cpu::op_CB71, Cpu::op_CB72, Cpu::op_CB73, Cpu::op_CB74, Cpu::op_CB75, Cpu::op_CB76, Cpu::op_CB77, Cpu::op_CB78, Cpu::op_CB79, Cpu::op_CB7A, Cpu::op_CB7B, Cpu::op_CB7C, Cpu::op_CB7D, Cpu::op_CB7E, Cpu::op_CB7F,
        Cpu::op_CB80, Cpu::op_CB81, Cpu::op_CB82, Cpu::op_CB83, Cpu::op_CB84, Cpu::op_CB85, Cpu::op_CB86, Cpu::op_CB87, Cpu::op_CB88, Cpu::op_CB89, Cpu::op_CB8A, Cpu::op_CB8B, Cpu::op_CB8C, Cpu::op_CB8D, Cpu::op_CB8E, Cpu::op_CB8F,
        Cpu::op_CB90, Cpu::op_CB91, Cpu::op_CB92, Cpu::op_CB93, Cpu::op_CB94, Cpu::op_CB95, Cpu::op_CB96, Cpu::op_CB97, Cpu::op_CB98, Cpu::op_CB99, Cpu::op_CB9A, Cpu::op_CB9B, Cpu::op_CB9C, Cpu::op_CB9D, Cpu::op_CB9E, Cpu::op_CB9F,
        Cpu::op_CBA0, Cpu::op_CBA1, Cpu::op_CBA2, Cpu::op_CBA3, Cpu::op_CBA4, Cpu::op_CBA5, Cpu::op_CBA6, Cpu::op_CBA7, Cpu::op_CBA8, Cpu::op_CBA9, Cpu::op_CBAA, Cpu::op_CBAB, Cpu::op_CBAC, Cpu::op_CBAD, Cpu::op_CBAE, Cpu::op_CBAF,
        Cpu::op_CBB0, Cpu::op_CBB1, Cpu::op_CBB2, Cpu::op_CBB3, Cpu::op_CBB4, Cpu::op_CBB5, Cpu::op_CBB6, Cpu::op_CBB7, Cpu::op_CBB8, Cpu::op_CBB9, Cpu::op_CBBA, Cpu::op_CBBB, Cpu::op_CBBC, Cpu::op_CBBD, Cpu::op_CBBE, Cpu::op_CBBF,
        Cpu::op_CBC0, Cpu::op_CBC1, Cpu::op_CBC2, Cpu::op_CBC3, Cpu::op_CBC4, Cpu::op_CBC5, Cpu::op_CBC6, Cpu::op_CBC7, Cpu::op_CBC8, Cpu::op_CBC9, Cpu::op_CBCA, Cpu::op_CBCB, Cpu::op_CBCC, Cpu::op_CBCD, Cpu::op_CBCE, Cpu::op_CBCF,
        Cpu::op_CBD0, Cpu::op_CBD1, Cpu::op_CBD2, Cpu::op_CBD3, Cpu::op_CBD4, Cpu::op_CBD5, Cpu::op_CBD6, Cpu::op_CBD7, Cpu::op_CBD8, Cpu::op_CBD9, Cpu::op_CBDA, Cpu::op_CBDB, Cpu::op_CBDC, Cpu::op_CBDD, Cpu::op_CBDE, Cpu::op_CBDF,
        Cpu::op_CBE0, Cpu::op_CBE1, Cpu::op_CBE2, Cpu::op_CBE3, Cpu::op_CBE4, Cpu::op_CBE5, Cpu::op_CBE6, Cpu::op_CBE7, Cpu::op_CBE8, Cpu::op_CBE9, Cpu::op_CBEA, Cpu::op_CBEB, Cpu::op_CBEC, Cpu::op_CBED, Cpu::op_CBEE, Cpu::op_CBEF,
        Cpu::op_CBF0, Cpu::op_CBF1, Cpu::op_CBF2, Cpu::op_CBF3, Cpu::op_CBF4, Cpu::op_CBF5, Cpu::op_CBF6, Cpu::op_CBF7, Cpu::op_CBF8, Cpu::op_CBF9, Cpu::op_CBFA, Cpu::op_CBFB, Cpu::op_CBFC, Cpu::op_CBFD, Cpu::op_CBFE, Cpu::op_CBFF};

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
        // _sleep(1/1000000);
    }
}

bool Cpu::tick(){
    // cout<<"Current PC: "<<(int)pc<<" Remaining M-Cycle: "<<(int) mcycle<<" OP skip: "<<(int)opskip<<endl;
    // printf("%.4X %.2X\n", pc, memory.address[pc]);
    (this->*jump_table[memory.address[pc]])();
    cycle_count++;

    if (mcycle == 1 || cycle_count == mcycle) {
        if (ime_next) {
            ime = 1;
            ime_next = 0;
        } // EI is delayed by 1 instr
        logger.write(craft_debug());
        exec_flag = 1;
        (this->*jump_table[memory.address[pc]])();
        pc+=opskip;
        exec_flag = 0;
        cycle_count = 0;
    }
    return true;
}

void Cpu::write8_mem(u_short addr, u_char a) {
    memory.address[addr] = a;
}

u_char Cpu::read8_mem(u_short addr) {
    return memory.address[addr];
}

u_short Cpu::read16_mem(u_short addr) {
    return memory.address[addr+1] << 8 | memory.address[addr];
}

void Cpu::ld8_imm(u_char &a, u_char imm) {
    a = imm;
}

void Cpu::ld16_imm(u_char &a, u_char &b, u_short imm) {
    a = imm >> 8;
    b = imm & 0xff;
}

void Cpu::inc8(u_char &a) {
    auto [result, carry] = SafeOperations::safe_add(a, 1);
    a = result;

    if ((a & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;
    z_flag = a ? 0 : 1;
    n_flag = 0;
}

void Cpu::inc16(u_char &a, u_char &b) {
    u_short t = a << 8 | b;
    auto [result, carry] = SafeOperations::safe_add(t, 1);
    t = result;
    a = t >> 8;
    b = t & 0xFF;
}

void Cpu::inc_sp() {
    auto [result, carry] = SafeOperations::safe_add(sp, 1);
    sp = result;
}

void Cpu::inc_ind_hl() {
    u_char data = read8_mem(h<<8|l);
    auto [result, carry] = SafeOperations::safe_add(data, 1);
    data = result;
    write8_mem(h<<8|l, data);
    if ((data & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 0;
}

void Cpu::dec8(u_char &a) {
    auto [result, carry] = SafeOperations::safe_sub(a, 1);
    a = result;

    if ((a & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = a ? 0 : 1;
    n_flag = 1;
}

void Cpu::dec16(u_char &a, u_char &b) {
    u_short t = a << 8 | b;
    auto [result, carry] = SafeOperations::safe_sub(t, 1);
    t = result;
    a = t >> 8;
    b = t & 0xFF;
}

void Cpu::dec_sp() {
    auto [result, carry] = SafeOperations::safe_sub(sp, 1);
    sp = result;
}

void Cpu::dec_ind_hl() {
    u_char data = read8_mem(h<<8|l);
    auto [result, carry] = SafeOperations::safe_sub(data, 1);
    data = result;
    write8_mem(h<<8|l, data);
    if ((data & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 1;
}

void Cpu::add8(u_char &a, u_char b) {
    auto [result, carry] = SafeOperations::safe_add(a , b);
    a = result;
    n_flag = 0;
    z_flag = a==0;
    c_flag  = carry;
    h_flag = ((a & 0xF) + (b & 0xF)) & 0x10;
}

void Cpu::add_hl(u_short imm) {
    auto [result, carry] = SafeOperations::safe_add(h<<8|l, imm);
    h = result >> 8;
    l = result & 0xFF;

    n_flag = 0;
    h_flag = h & 0x100;
    c_flag = carry;
}

void Cpu::add_sp(s_char imm) {
    if (imm > 0) {
        auto [result, carry] = SafeOperations::safe_add(sp, imm);
        sp = result;
        c_flag = carry;
        h_flag = ((sp & 0xFF) + imm) & 0x100;
    }
    else {
        auto [result, carry] = SafeOperations::safe_sub(sp, -imm);
        sp = result;
        c_flag = carry;
        h_flag = ((sp & 0xFF) < -imm);
    }
    z_flag = 0;
    n_flag = 0;
}


void Cpu::adc8(u_char&a, u_char b) {
    auto [result_c, carry_c] = SafeOperations::safe_add(b,c_flag);
    auto [result, carry] = SafeOperations::safe_add(a, result_c);

    n_flag = 0;
    z_flag = a==0;
    c_flag  = carry > carry_c ? carry : carry_c;
    h_flag = ((a & 0xF) + (b & 0xF)) & 0x10;
}


void Cpu::sub8(u_char &a, u_char b) {
    auto [result, carry] = SafeOperations::safe_sub(a, b);
    a = result;
    n_flag = 1;
    z_flag = a==0;
    c_flag  = carry;
    h_flag = ((a & 0xF) < (b & 0xF));
}

void Cpu::sbc8(u_char &a, u_char b) { // A - B - C
    auto [result_c, carry_c] = SafeOperations::safe_sub(a,b);
    auto [result, carry] = SafeOperations::safe_sub(result_c, c_flag);

    n_flag = 1;
    z_flag = a==0;
    c_flag  = carry > carry_c ? carry : carry_c;
    h_flag = ((a & 0xF) < (b & 0xF));
}

void Cpu::and8(u_char &a, u_char b) {
    a = a & b;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 1;
    c_flag = 0;
}

void Cpu::or8(u_char &a, u_char b) {
    a = a | b;
    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
}
void Cpu::xor8(u_char &a, u_char b) {
    a = a ^ b;
    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
}

void Cpu::cp8(u_char a, u_char b) {
    auto [result, carry] = SafeOperations::safe_sub(a, b);

    n_flag = 1;
    z_flag = a==0;
    c_flag  = carry;
    h_flag = ((a & 0xF) < (b & 0xF));
}

void Cpu::jr(u_char e) {
    pc += (signed char) (e);
}

void Cpu::jrc(u_char flag, u_char e) {
    if (flag) pc += (signed char) (e);
}

void Cpu::jp(u_short e) {
    opskip = 0;
    pc =  e;
}

void Cpu::jpc(u_char flag, u_short e) {
    if (flag) {
        opskip = 0;
        pc = e;
    }
}

void Cpu::call() {
    write8_mem(--sp, pc+3 >> 8);
    write8_mem(--sp, pc+3 & 0xFF);
    pc = read16_mem(pc+1);
    opskip = 0;
}

void Cpu::callc(u_char flag) {
    if(flag) {
        write8_mem(--sp, pc+3 >> 8);
        write8_mem(--sp, pc+3 & 0xFF);
        pc = read16_mem(pc+1);
        opskip = 0;
    }
}

void Cpu::ret() {
    pc = read8_mem(sp++) | read8_mem(sp++) << 8;
    opskip = 0;
}

void Cpu::retc(u_char flag) {
    if (flag) {
        pc = read8_mem(sp++) | read8_mem(sp++) << 8;
        opskip = 0;
    }
}

void Cpu::rst(u_char n) {
    write8_mem(--sp, pc+3 >> 8);
    write8_mem(--sp, pc+3 & 0xFF);
    pc = 0x00 << 8 | n;
    opskip = 0;
}

void Cpu::pop(u_char &a, u_char &b) {
    b = read8_mem(sp++);
    a = read8_mem(sp++);
}

void Cpu::push(u_char a, u_char b) {
    write8_mem(--sp, a);
    write8_mem(--sp, b);
}

void Cpu::rlc(u_char &a) {
    c_flag = a & 0x80;
    a = a << 1;
    a = a | c_flag;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::rlc_hl() {
    u_char a = read8_mem(h<<8|l);
    c_flag = a & 0x80;
    a = a << 1;
    a = a | c_flag;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::rrc(u_char &a) {
    c_flag = a & 0x01;
    a = a >> 1;
    a = c_flag<<7 | a;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::rrc_hl() {
    u_char a = read8_mem(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    a = c_flag<<7 | a;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::rl(u_char &a) {
    u_char t = a & 0x80;
    a = a << 1;
    a = a | c_flag;
    c_flag = t;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::rl_hl() {
    u_char a = read8_mem(h<<8|l);
    u_char t = a & 0x80;
    a = a << 1;
    a = a | c_flag;
    c_flag = t;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::rr(u_char &a) {
    u_char t = a & 0x01;
    a = a >> 1;
    a = c_flag << 7 | a;
    c_flag = t;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::rr_hl() {
    u_char a = read8_mem(h<<8|l);
    u_char t = a & 0x01;
    a = a >> 1;
    a = c_flag << 7 | a;
    c_flag = t;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::sla(u_char &a) {
    c_flag = a & 0x80;
    a = a << 1;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::sla_hl() {
    u_char a = read8_mem(h<<8|l);
    c_flag = a & 0x80;
    a = a << 1;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::sra(u_char &a) { // KEEP BIT 7
    c_flag = a & 0x01;
    a = a >> 1;
    a = (a & 0x40) << 1 | a;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::sra_hl() {
    u_char a = read8_mem(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    a = (a & 0x40) << 1 | a;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::swap(u_char &a) {
    a = ((a & 0xF) << 4) | (a >> 4);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
}

void Cpu::swap_hl() {
    u_char a = read8_mem(h<<8|l);
    a = ((a & 0xF) << 4) | (a >> 4);
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::srl(u_char &a) { // SRL IS WHAT SRA SUPPOSED TO BE
    c_flag = a & 0x01;
    a = a >> 1;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::srl_hl() {
    u_char a = read8_mem(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void Cpu::bit(u_char i, u_char a) {
    z_flag = ((a >> i) & 0x01) == 0;
    n_flag = 0;
    h_flag = 1;
}

void Cpu::set(u_char i, u_char &a) {
    a = a | (0x01 << i);
}

void Cpu::set_hl(u_char i) {
    u_char a = read8_mem(h<<8|l);
    a = a | (0x01 << i);
    write8_mem(h<<8|l , a);
}

void Cpu::res(u_char i, u_char &a) {
    a = a & ~(0x01 << i);
}

void Cpu::res_hl(u_char i) {
    u_char a = read8_mem(h<<8|l);
    a = a & ~(0x01 << i);
    write8_mem(h<<8|l , a);
}





