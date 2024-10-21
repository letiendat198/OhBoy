#include "cpu.h"

#include <debugger.h>
#include <format>

#include "util.h"

#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

void Cpu::init(){
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

bool Cpu::tick(){
    // cout<<"Current PC: "<<(int)pc<<" Remaining M-Cycle: "<<(int) mcycle<<" OP skip: "<<(int)opskip<<endl;
    if(cycle_count==0) (this->*jump_table[Memory::read(pc)])();
    cycle_count++;

    if (mcycle == 1 || cycle_count == mcycle) {
        if (ime_next) {
            ime = 1;
            ime_next = 0;
        } // EI is delayed by 1 instr
        logger.write("");
        Debugger::log(std::format("Current mcycle: {}", mcycle).c_str());
        exec_flag = 1;
        (this->*jump_table[Memory::read(pc)])();
        pc+=opskip;
        exec_flag = 0;
        cycle_count = 0;
    }
    return true;
}

void Cpu::write8_mem(u_short addr, u_char a) {
    Memory::write(addr, a);
}

u_char Cpu::read8_mem(u_short addr) {
    return Memory::read(addr);
}

u_short Cpu::read16_mem(u_short addr) {
    return Memory::read(addr+1) << 8 | Memory::read(addr);
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





