#include "cpu.h"

#include <debugger.h>
#include <format>

#include "util.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "interrupts.h"
using namespace std;

void CPU::init() {
    // a = 0x01;
    // b = 0x00;
    // c = 0x13;
    // d = 0x00;
    // e = 0xD8;
    // h = 0x01;
    // l = 0x4D;
    // sp = 0xFFFE;
    // pc = 0x0100;
    // z_flag = 1;
    // n_flag = 0;
    // h_flag = 1;
    // c_flag = 1;

    // CGB initial value
    a = 0x11;
    b = 0x00;
    c = 0x00;
    d = 0xFF;
    e = 0x56;
    h = 0x00;
    l = 0x0D;
    sp = 0xFFFE;
    pc = 0x0100;
    z_flag = 1;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
    Cartridge::boot_off();
}

void CPU::log_cpu() {
    uint8_t f = z_flag<<7 | n_flag << 6 | h_flag << 5 | c_flag<<4;
    logger.get_logger()->debug("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}", a, f, b, c, d, e, h, l, sp, pc, read8_mem(pc), read8_mem(pc+1), read8_mem(pc+2), read8_mem(pc+3));
}

void CPU::tick(){
    if(cycle_count==0) {
        bool is_servicing_interrupt = handle_interrupts();
        if (halt == 1 || is_servicing_interrupt) return; // No HALT bug
        (this->*jump_table[Memory::read(pc)])();
    }
    cycle_count++;

    if (mcycle == 1 || cycle_count == mcycle) {
        if (ime_next) {
            ime = 1;
            ime_next = false;
        } // EI is delayed by 1 instr
        exec_flag = true;
        // log_cpu();
        (this->*jump_table[Memory::read(pc)])();
        pc+=opskip;
        exec_flag = false;
        cycle_count = 0;
    }
}

bool CPU::handle_interrupts() {
    if (Interrupts::is_pending() || interrupt_addr != 0) {
        if (interrupt_cycle_count == 0) {
            halt = 0;
            interrupt_addr = Interrupts::check_and_service(ime); // Check interrupts
            if (interrupt_addr == 0) return false; // In case IME is disabled
            logger.get_logger()->debug(std::format("Interrupt requested at {:#X}", interrupt_addr));
            mcycle = 5;
        }
        interrupt_cycle_count++;
        if (interrupt_cycle_count == mcycle) {
            logger.get_logger()->debug("Servicing interrupt {:02X}", interrupt_addr);
            write8_mem(--sp, pc >> 8);
            write8_mem(--sp, pc & 0xFF);
            pc = interrupt_addr;
            interrupt_addr = 0;
            interrupt_cycle_count = 0;
        }
        return true;
    }
    return false;
}

void CPU::write8_mem(uint16_t addr, uint8_t a) {
    Memory::write(addr, a);
}

uint8_t CPU::read8_mem(uint16_t addr) {
    return Memory::read(addr);
}

uint16_t CPU::read16_mem(uint16_t addr) {
    return Memory::read(addr+1) << 8 | Memory::read(addr);
}

void CPU::ld8_imm(uint8_t &a, uint8_t imm) {
    a = imm;
}

void CPU::ld16_imm(uint8_t &a, uint8_t &b, uint16_t imm) {
    a = imm >> 8;
    b = imm & 0xff;
}

void CPU::inc8(uint8_t &a) {
    auto [result, carry] = SafeOperations::safe_add(a, 1);
    a = result;

    if ((a & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;
    z_flag = a ? 0 : 1;
    n_flag = 0;
}

void CPU::inc16(uint8_t &a, uint8_t &b) {
    uint16_t t = a << 8 | b;
    auto [result, carry] = SafeOperations::safe_add(t, 1);
    t = result;
    a = t >> 8;
    b = t & 0xFF;
}

void CPU::inc_sp() {
    auto [result, carry] = SafeOperations::safe_add(sp, 1);
    sp = result;
}

void CPU::inc_ind_hl() {
    uint8_t data = read8_mem(h<<8|l);
    auto [result, carry] = SafeOperations::safe_add(data, 1);
    data = result;
    write8_mem(h<<8|l, data);
    if ((data & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 0;
}

void CPU::dec8(uint8_t &a) {
    auto [result, carry] = SafeOperations::safe_sub(a, 1);
    a = result;

    if ((a & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = a ? 0 : 1;
    n_flag = 1;
}

void CPU::dec16(uint8_t &a, uint8_t &b) {
    uint16_t t = a << 8 | b;
    auto [result, carry] = SafeOperations::safe_sub(t, 1);
    t = result;
    a = t >> 8;
    b = t & 0xFF;
}

void CPU::dec_sp() {
    auto [result, carry] = SafeOperations::safe_sub(sp, 1);
    sp = result;
}

void CPU::dec_ind_hl() {
    uint8_t data = read8_mem(h<<8|l);
    auto [result, carry] = SafeOperations::safe_sub(data, 1);
    data = result;
    write8_mem(h<<8|l, data);
    if ((data & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 1;
}

void CPU::add8(uint8_t &a, uint8_t b) {
    auto [result, carry] = SafeOperations::safe_add(a , b);
    h_flag = (((a & 0xF) + (b & 0xF)) & 0x10) >> 4;

    a = result;
    n_flag = 0;
    z_flag = a==0;
    c_flag  = carry;
}

void CPU::add_hl(uint16_t imm) {
    auto [result, carry] = SafeOperations::safe_add(h<<8|l, imm);
    h_flag = ((((h<<8|l) & 0x0FFF) + (imm & 0x0FFF)) & 0x1000) >> 12;
    h = result >> 8;
    l = result & 0xFF;

    n_flag = 0;
    c_flag = carry;
}

void CPU::add_sp(uint8_t imm) {
    if ((imm & 0x80) >> 7 == 0) {
        auto [result, carry] = SafeOperations::safe_add(sp, (int8_t) imm);
        h_flag = (((sp & 0x0F) + (imm & 0x0F)) & 0x10) >> 4;
        c_flag = (((sp & 0xFF) + imm) & 0x100) >> 8;
        sp = result;
    }
    else {
        auto [result, carry] = SafeOperations::safe_sub(sp,  -((int8_t) imm));
        // Carry and half carry still get calculated by addition
        c_flag = (((sp & 0xFF) + imm) & 0x100) >> 8;
        h_flag = (((sp & 0xF) + ((imm) & 0xF)) & 0x10) >> 4;
        sp = result;
    }
    z_flag = 0;
    n_flag = 0;
}


void CPU::adc8(uint8_t&a, uint8_t b) {
    auto [result_c, carry_c] = SafeOperations::safe_add(b,c_flag);
    auto [result, carry] = SafeOperations::safe_add(a, result_c);

    h_flag = (((a & 0xF) + (b & 0xF) + c_flag) & 0x10) >> 4;
    a = result;
    n_flag = 0;
    z_flag = a==0;
    c_flag  = carry > carry_c ? carry : carry_c;
}


void CPU::sub8(uint8_t &a, uint8_t b) {
    auto [result, carry] = SafeOperations::safe_sub(a, b);
    h_flag = ((a & 0xF) < (b & 0xF));
    a = result;
    n_flag = 1;
    z_flag = a==0;
    c_flag  = carry;
}

void CPU::sbc8(uint8_t &a, uint8_t b) { // A - B - C
    auto [result_c, carry_c] = SafeOperations::safe_sub(a,b);
    auto [result, carry] = SafeOperations::safe_sub(result_c, c_flag);
    h_flag = (((a & 0xF) - (b & 0xF) - c_flag) & 0x10) >> 4;
    a = result;

    n_flag = 1;
    z_flag = a==0;
    c_flag  = carry > carry_c ? carry : carry_c;
}

void CPU::and8(uint8_t &a, uint8_t b) {
    a = a & b;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 1;
    c_flag = 0;
}

void CPU::or8(uint8_t &a, uint8_t b) {
    a = a | b;
    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
}
void CPU::xor8(uint8_t &a, uint8_t b) {
    a = a ^ b;
    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
}

void CPU::cp8(uint8_t a, uint8_t b) {
    auto [result, carry] = SafeOperations::safe_sub(a, b);

    n_flag = 1;
    z_flag = result==0;
    c_flag  = carry;
    h_flag = ((a & 0xF) < (b & 0xF));
}

void CPU::jr(uint8_t e) {
    pc += (signed char) (e);
}

void CPU::jrc(uint8_t flag, uint8_t e) {
    if (flag) pc += (signed char) (e);
}

void CPU::jp(uint16_t e) {
    opskip = 0;
    pc =  e;
}

void CPU::jpc(uint8_t flag, uint16_t e) {
    if (flag) {
        opskip = 0;
        pc = e;
    }
}

void CPU::call() {
    write8_mem(--sp, pc+3 >> 8);
    write8_mem(--sp, pc+3 & 0xFF);
    pc = read16_mem(pc+1);
    opskip = 0;
}

void CPU::callc(uint8_t flag) {
    if(flag) {
        write8_mem(--sp, pc+3 >> 8);
        write8_mem(--sp, pc+3 & 0xFF);
        pc = read16_mem(pc+1);
        opskip = 0;
    }
}

void CPU::ret() {
    pc = read8_mem(sp++) | read8_mem(sp++) << 8;
    opskip = 0;
}

void CPU::retc(uint8_t flag) {
    if (flag) {
        pc = read8_mem(sp++) | read8_mem(sp++) << 8;
        opskip = 0;
    }
}

void CPU::rst(uint8_t n) {
    write8_mem(--sp, pc+1 >> 8);
    write8_mem(--sp, pc+1 & 0xFF);
    pc = 0x00 << 8 | n;
    opskip = 0;
}

void CPU::pop(uint8_t &a, uint8_t &b) {
    b = read8_mem(sp++);
    a = read8_mem(sp++);
}

void CPU::push(uint8_t a, uint8_t b) {
    write8_mem(--sp, a);
    write8_mem(--sp, b);
}

void CPU::rlc(uint8_t &a) {
    c_flag = (a & 0x80) >> 7;
    a = a << 1;
    a = a | c_flag;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::rlc_hl() {
    uint8_t a = read8_mem(h<<8|l);
    c_flag = (a & 0x80) >> 7;
    a = a << 1;
    a = a | c_flag;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::rrc(uint8_t &a) {
    c_flag = a & 0x01;
    a = a >> 1;
    a = c_flag<<7 | a;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::rrc_hl() {
    uint8_t a = read8_mem(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    a = c_flag<<7 | a;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::rl(uint8_t &a) {
    uint8_t t = (a & 0x80) >> 7;
    a = a << 1;
    a = a | c_flag;
    c_flag = t;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::rl_hl() {
    uint8_t a = read8_mem(h<<8|l);
    uint8_t t = (a & 0x80) >> 7;
    a = a << 1;
    a = a | c_flag;
    c_flag = t;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::rr(uint8_t &a) {
    uint8_t t = a & 0x01;
    a = a >> 1;
    a = c_flag << 7 | a;
    c_flag = t;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::rr_hl() {
    uint8_t a = read8_mem(h<<8|l);
    uint8_t t = a & 0x01;
    a = a >> 1;
    a = c_flag << 7 | a;
    c_flag = t;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::sla(uint8_t &a) {
    c_flag = (a & 0x80) >> 7;
    a = a << 1;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::sla_hl() {
    uint8_t a = read8_mem(h<<8|l);
    c_flag = (a & 0x80) >> 7;
    a = a << 1;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::sra(uint8_t &a) { // KEEP BIT 7
    c_flag = a & 0x01;
    a = a >> 1;
    a = (a & 0x40) << 1 | a;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::sra_hl() {
    uint8_t a = read8_mem(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    a = (a & 0x40) << 1 | a;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::swap(uint8_t &a) {
    a = ((a & 0xF) << 4) | (a >> 4);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
}

void CPU::swap_hl() {
    uint8_t a = read8_mem(h<<8|l);
    a = ((a & 0xF) << 4) | (a >> 4);
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
    c_flag = 0;
}

void CPU::srl(uint8_t &a) { // SRL IS WHAT SRA SUPPOSED TO BE
    c_flag = a & 0x01;
    a = a >> 1;

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::srl_hl() {
    uint8_t a = read8_mem(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    write8_mem(h<<8|l, a);

    z_flag = a==0;
    n_flag = 0;
    h_flag = 0;
}

void CPU::bit(uint8_t i, uint8_t a) {
    z_flag = ((a >> i) & 0x01) == 0;
    n_flag = 0;
    h_flag = 1;
}

void CPU::set(uint8_t i, uint8_t &a) {
    a = a | (0x01 << i);
}

void CPU::set_hl(uint8_t i) {
    uint8_t a = read8_mem(h<<8|l);
    a = a | (0x01 << i);
    write8_mem(h<<8|l , a);
}

void CPU::res(uint8_t i, uint8_t &a) {
    a = a & ~(0x01 << i);
}

void CPU::res_hl(uint8_t i) {
    uint8_t a = read8_mem(h<<8|l);
    a = a & ~(0x01 << i);
    write8_mem(h<<8|l , a);
}





