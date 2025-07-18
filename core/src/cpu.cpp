#include "cpu.h"
#include "scheduler.h"
#include "interrupt.h"
using namespace std;

CPU::CPU() {
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
    // bus.is_boot = false;
}

void CPU::log_cpu() {
    uint8_t f = z_flag<<7 | n_flag << 6 | h_flag << 5 | c_flag<<4;
    SPDLOG_LOGGER_DEBUG(logger.get_logger(), "A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}", a, f, b, c, d, e, h, l, sp, pc, bus.read(pc), bus.read(pc+1), bus.read(pc+2), bus.read(pc+3));
}

uint32_t CPU::tick(){
    if (halt == 1) return 1; // No HALT bug

    if (ime_next) {
        ime = 1;
        ime_next = false;
    } // EI is delayed by 1 instr

    // log_cpu();
    (this->*jump_table[bus.read(pc)])();
    pc+=opskip;


    return mcycle;
}

bool CPU::handle_interrupts() {
    if (bus.interrupt.is_pending()) { // This guarantee IF & IE != 0
        halt = 0;
        if (!ime) return false;
        uint8_t interrupt_addr = bus.interrupt.check_and_service(ime); // Check interrupts
        if (interrupt_addr == 0) return false; // TODO: NOT EXACTLY NEEDED
        Scheduler::current_cycle += 5;

        // logger.get_logger()->debug("Servicing interrupt {:02X}", interrupt_addr);
        bus.write(--sp, pc >> 8);
        bus.write(--sp, pc & 0xFF);
        pc = interrupt_addr;
        return true;
    }
    return false;
}

uint16_t CPU::read16_mem(uint16_t addr) {
    return bus.read(addr+1) << 8 | bus.read(addr);
}

void CPU::ld8_imm(uint8_t &a, uint8_t imm) {
    a = imm;
}

void CPU::ld16_imm(uint8_t &a, uint8_t &b, uint16_t imm) {
    a = imm >> 8;
    b = imm & 0xff;
}

void CPU::inc8(uint8_t &a) {
    ++a;

    if ((a & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;
    z_flag = a ? 0 : 1;
    n_flag = 0;
}

void CPU::inc16(uint8_t &a, uint8_t &b) {
    uint16_t t = a << 8 | b;
    ++t;
    a = t >> 8;
    b = t & 0xFF;
}

void CPU::inc_sp() {
    ++sp;
}

void CPU::inc_ind_hl() {
    uint8_t data = bus.read(h<<8|l);
    ++data;
    bus.write(h<<8|l, data);
    if ((data & 0xF) == 0) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 0;
}

void CPU::dec8(uint8_t &a) {
    --a;

    if ((a & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = a ? 0 : 1;
    n_flag = 1;
}

void CPU::dec16(uint8_t &a, uint8_t &b) {
    uint16_t t = a << 8 | b;
    --t;
    a = t >> 8;
    b = t & 0xFF;
}

void CPU::dec_sp() {
    --sp;
}

void CPU::dec_ind_hl() {
    uint8_t data = bus.read(h<<8|l);
    --data;
    bus.write(h<<8|l, data);
    if ((data & 0xF) == 0xF) h_flag = 1; // Lower 4 bit carry
    else h_flag=0;

    z_flag = data ? 0 : 1;
    n_flag = 1;
}

void CPU::add8(uint8_t &a, uint8_t b) {
    h_flag = (((a & 0xF) + (b & 0xF)) & 0x10) >> 4;
    c_flag  = ((a+b) & 0xFF) < a;
    a = a+b;
    n_flag = 0;
    z_flag = a==0;
}

void CPU::add_hl(uint16_t imm) {
    uint16_t reg = h<<8|l;
    uint16_t result = reg + imm;
    h_flag = ((((h<<8|l) & 0x0FFF) + (imm & 0x0FFF)) & 0x1000) >> 12;
    h = result >> 8;
    l = result & 0xFF;

    n_flag = 0;
    c_flag = result < reg;
}

void CPU::add_sp(uint8_t imm) {
    if ((imm & 0x80) >> 7 == 0) {
        h_flag = (((sp & 0x0F) + (imm & 0x0F)) & 0x10) >> 4;
        c_flag = (((sp & 0xFF) + imm) & 0x100) >> 8;
        sp = sp + imm;
    }
    else {
        // Carry and half carry still get calculated by addition
        c_flag = (((sp & 0xFF) + imm) & 0x100) >> 8;
        h_flag = (((sp & 0xF) + ((imm) & 0xF)) & 0x10) >> 4;
        sp = sp + static_cast<int8_t>(imm);
    }
    z_flag = 0;
    n_flag = 0;
}


void CPU::adc8(uint8_t&a, uint8_t b) {
    uint8_t carry_c = ((b + c_flag) & 0xFF) < b;
    uint8_t carry = ((a + b + c_flag) & 0xFF) < a;
    h_flag = (((a & 0xF) + (b & 0xF) + c_flag) & 0x10) >> 4;

    a = a + b + c_flag;
    n_flag = 0;
    z_flag = a==0;
    c_flag  = carry > carry_c ? carry : carry_c;
}


void CPU::sub8(uint8_t &a, uint8_t b) {
    h_flag = ((a & 0xF) < (b & 0xF));
    c_flag  = ((a-b) & 0xFF) > a;
    a = a - b;
    n_flag = 1;
    z_flag = a==0;
}

void CPU::sbc8(uint8_t &a, uint8_t b) { // A - B - C
    uint8_t carry_c = ((a-b) & 0xFF) > a;
    uint8_t carry = ((a-b-c_flag) & 0xFF) > ((a-b) & 0xFF);
    h_flag = (((a & 0xF) - (b & 0xF) - c_flag) & 0x10) >> 4;

    a = a - b - c_flag;
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
    uint8_t result = a - b;

    n_flag = 1;
    z_flag = result==0;
    c_flag  = result > a;
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
    bus.write(--sp, pc+3 >> 8);
    bus.write(--sp, pc+3 & 0xFF);
    pc = read16_mem(pc+1);
    opskip = 0;
}

void CPU::callc(uint8_t flag) {
    if(flag) {
        bus.write(--sp, pc+3 >> 8);
        bus.write(--sp, pc+3 & 0xFF);
        pc = read16_mem(pc+1);
        opskip = 0;
    }
}

void CPU::ret() {
    pc = bus.read(sp++) | bus.read(sp++) << 8;
    opskip = 0;
}

void CPU::retc(uint8_t flag) {
    if (flag) {
        pc = bus.read(sp++) | bus.read(sp++) << 8;
        opskip = 0;
    }
}

void CPU::rst(uint8_t n) {
    bus.write(--sp, pc+1 >> 8);
    bus.write(--sp, pc+1 & 0xFF);
    pc = 0x00 << 8 | n;
    opskip = 0;
}

void CPU::pop(uint8_t &a, uint8_t &b) {
    b = bus.read(sp++);
    a = bus.read(sp++);
}

void CPU::push(uint8_t a, uint8_t b) {
    bus.write(--sp, a);
    bus.write(--sp, b);
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
    uint8_t a = bus.read(h<<8|l);
    c_flag = (a & 0x80) >> 7;
    a = a << 1;
    a = a | c_flag;
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    a = c_flag<<7 | a;
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    uint8_t t = (a & 0x80) >> 7;
    a = a << 1;
    a = a | c_flag;
    c_flag = t;
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    uint8_t t = a & 0x01;
    a = a >> 1;
    a = c_flag << 7 | a;
    c_flag = t;
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    c_flag = (a & 0x80) >> 7;
    a = a << 1;
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    a = (a & 0x40) << 1 | a;
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    a = ((a & 0xF) << 4) | (a >> 4);
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    c_flag = a & 0x01;
    a = a >> 1;
    bus.write(h<<8|l, a);

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
    uint8_t a = bus.read(h<<8|l);
    a = a | (0x01 << i);
    bus.write(h<<8|l , a);
}

void CPU::res(uint8_t i, uint8_t &a) {
    a = a & ~(0x01 << i);
}

void CPU::res_hl(uint8_t i) {
    uint8_t a = bus.read(h<<8|l);
    a = a & ~(0x01 << i);
    bus.write(h<<8|l , a);
}





