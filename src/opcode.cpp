#include "cpu.h"

void CPU::op_00() {
    mcycle = 1;
    opskip = 1;
} // NOP
void CPU::op_01() {
    if(exec_flag) ld16_imm(b, c, read16_mem(pc+1));
    mcycle = 3;
    opskip = 3;
} // LD BC n16
void CPU::op_02() {
    if(exec_flag) write8_mem(b<<8 | c, a);
    mcycle = 2;
    opskip = 1;
} // LD [BC] A
void CPU::op_03() {
    if(exec_flag) inc16(b,c);
    mcycle = 2;
    opskip = 1;
} // INC BC
void CPU::op_04() {
    if(exec_flag) inc8(b);
    mcycle = 1;
    opskip = 1;
} // INC B
void CPU::op_05() {
    if(exec_flag) dec8(b);
    mcycle = 1;
    opskip = 1;
} // DEC B
void CPU::op_06() {
    if(exec_flag) ld8_imm(b, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD B n8
void CPU::op_07() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        rlc(a);
        z_flag = 0;
    }
} // RLCA
void CPU::op_08() {
    if(exec_flag) {
        uint16_t addr = read16_mem(pc+1);
        write8_mem(addr, sp & 0xFF);
        write8_mem(addr+1, sp >> 8);
    }
    mcycle = 5;
    opskip = 3;
} // LD [a16] SP
void CPU::op_09() {
    opskip  = 1;
    mcycle  = 2;
    if (exec_flag) add_hl(b<<8 | c);
} // ADD HL BC
void CPU::op_0A() {
    if(exec_flag) ld8_imm(a, read8_mem(b<<8 | c));
    mcycle = 2;
    opskip = 1;
} // LD A [BC]
void CPU::op_0B() {
    if(exec_flag) dec16(b,c);
    mcycle = 2;
    opskip = 1;
} // DEC BC
void CPU::op_0C() {
    if(exec_flag) inc8(c);
    mcycle = 1;
    opskip = 1;
} // INC C
void CPU::op_0D() {
    if(exec_flag) dec8(c);
    mcycle = 1;
    opskip = 1;
} // DEC C
void CPU::op_0E() {
    if(exec_flag) ld8_imm(c, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD C n8
void CPU::op_0F() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        rrc(a);
        z_flag = 0;
    }
} // RRCA
void CPU::op_10() {
    if (Cartridge::cgb_mode) {
        uint8_t spd_switch_req = Memory::read(0xFF4D);
        uint8_t switch_armed = spd_switch_req & 0x1;
        uint8_t current_spd = (spd_switch_req >> 7) & 0x1;

        if (switch_armed) {
            Memory::write(0xFF04, 0); // Reset DIV
            double_spd_mode = !current_spd;
            Memory::write(0xFF4D, (double_spd_mode & 0x1) << 7);
        }
    }
    opskip = 2;
    mcycle = 1;
} // STOP n8
void CPU::op_11() {
    if(exec_flag) ld16_imm(d, e, read16_mem(pc+1));
    mcycle = 3;
    opskip = 3;
} // LD DE n16
void CPU::op_12() {
    if(exec_flag) write8_mem(d<<8|e, a);
    mcycle = 2;
    opskip = 1;
} // LD [DE] A
void CPU::op_13() {
    if(exec_flag) inc16(d,e);
    mcycle = 2;
    opskip = 1;
} // INC DE
void CPU::op_14() {
    if(exec_flag) inc8(d);
    mcycle = 1;
    opskip = 1;
} // INC D
void CPU::op_15() {
    if(exec_flag) dec8(d);
    mcycle = 1;
    opskip = 1;
} // DEC D
void CPU::op_16() {
    if(exec_flag) ld8_imm(d, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD D n8
void CPU::op_17() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        rl(a);
        z_flag = 0;
    }
} // RLA
void CPU::op_18() {
    if(exec_flag) jr(read8_mem(pc+1));
    mcycle=3;
    opskip=2;
} // JR e8
void CPU::op_19() {
    opskip  = 1;
    mcycle  = 2;
    if (exec_flag) add_hl(d<<8 | e);
} // ADD HL DE
void CPU::op_1A() {
    if(exec_flag) ld8_imm(a, read8_mem(d<<8|e));
    mcycle=2;
    opskip=1;
} // LD A [DE]
void CPU::op_1B() {
    if(exec_flag) dec16(d,e);
    mcycle = 2;
    opskip = 1;
} // DEC DE
void CPU::op_1C() {
    if(exec_flag) inc8(e);
    mcycle = 1;
    opskip = 1;
} // INC E
void CPU::op_1D() {
    if(exec_flag) dec8(e);
    mcycle = 1;
    opskip = 1;
} // DEC E
void CPU::op_1E() {
    if(exec_flag) ld8_imm(e, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD E n8
void CPU::op_1F() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        rr(a);
        z_flag = 0;
    }
} // RRA
void CPU::op_20() {
    if(exec_flag) jrc(!z_flag, read8_mem(pc+1));
    if (!z_flag) mcycle = 3;
    else mcycle = 2;
    opskip=2;
} // JR NZ e8
void CPU::op_21() {
    if(exec_flag) ld16_imm(h, l, read16_mem(pc+1));
    mcycle = 3;
    opskip = 3;
} // LD HL n16
void CPU::op_22() {
    if(exec_flag) {
        write8_mem(h<<8|l, a);
        inc16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL+] A
void CPU::op_23() {
    if(exec_flag) inc16(h,l);
    mcycle = 2;
    opskip = 1;
} // INC HL
void CPU::op_24() {
    if(exec_flag) inc8(h);
    mcycle = 1;
    opskip = 1;
} // INC H
void CPU::op_25() {
    if(exec_flag) dec8(h);
    mcycle = 1;
    opskip = 1;
} // DEC H
void CPU::op_26() {
    if(exec_flag) ld8_imm(h, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD H n8
void CPU::op_27() {
    opskip = 1;
    mcycle = 1;
    // https://forums.nesdev.org/viewtopic.php?t=15944 - Great explanation btw
    if (exec_flag) {
        // note: assumes a is a uint8_t and wraps from 0xff to 0
        if (!n_flag) {  // after an addition, adjust if (half-)carry occurred or if result is out of bounds
            if (c_flag || a > 0x99) { a += 0x60; c_flag = 1; }
            if (h_flag || (a & 0x0f) > 0x09) { a += 0x6; }
        } else {  // after a subtraction, only adjust if (half-)carry occurred
            if (c_flag) { a -= 0x60; }
            if (h_flag) { a -= 0x6; }
        }
        // these flags are always updated
        z_flag = (a == 0); // the usual z flag
        h_flag = 0; // h flag is always cleared
    }
} // DAA
void CPU::op_28() {
    if(exec_flag) jrc(z_flag, read8_mem(pc+1));
    if (z_flag) mcycle = 3;
    else mcycle = 2;
    opskip=2;
} // JR Z e8
void CPU::op_29() {
    opskip  = 1;
    mcycle  = 2;
    if (exec_flag) add_hl(h<<8 | l);
} // ADD HL HL
void CPU::op_2A() {
    if(exec_flag) {
        ld8_imm(a, read8_mem(h<<8|l));
        inc16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD A [HL+]
void CPU::op_2B() {
    if(exec_flag) dec16(h,l);
    mcycle = 2;
    opskip = 1;
} // DEC HL
void CPU::op_2C() {
    if(exec_flag) inc8(l);
    mcycle = 1;
    opskip = 1;
} // INC L
void CPU::op_2D() {
    if(exec_flag) dec8(l);
    mcycle = 1;
    opskip = 1;
} // DEC L
void CPU::op_2E() {
    if(exec_flag) ld8_imm(l, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD L n8
void CPU::op_2F() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        a = a ^ 0xFF;
        n_flag = 1;
        h_flag = 1;
    }
} // CPL
void CPU::op_30() {
    if(exec_flag) jrc(!c_flag, read8_mem(pc+1));
    if (!c_flag) mcycle = 3;
    else mcycle = 2;
    opskip=2;
} // JR NC e8
void CPU::op_31() {
    if(exec_flag) sp = read16_mem(pc+1);
    mcycle = 3;
    opskip = 3;
} // LD SP n16
void CPU::op_32() {
    if(exec_flag) {
        write8_mem(h<<8|l, a);
        dec16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL-] A
void CPU::op_33() {
    if(exec_flag) inc_sp();
    mcycle = 2;
    opskip = 1;
} // INC SP
void CPU::op_34() {
    if(exec_flag) inc_ind_hl();
    mcycle = 3;
    opskip = 1;
} // INC [HL]
void CPU::op_35() {
    if(exec_flag) dec_ind_hl();
    mcycle = 3;
    opskip = 1;
} // DEC [HL]
void CPU::op_36() {
    if(exec_flag) write8_mem(h<<8|l, read8_mem(pc+1));
    mcycle = 3;
    opskip = 2;
} // LD [HL] n8
void CPU::op_37() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        c_flag = 1;
        n_flag = 0;
        h_flag = 0;
    }
} // SCF
void CPU::op_38() {
    if(exec_flag) jrc(c_flag, read8_mem(pc+1));
    if (c_flag) mcycle = 3;
    else mcycle = 2;
    opskip=2;
} // JR C e8
void CPU::op_39() {
    opskip  = 1;
    mcycle  = 2;
    if (exec_flag) add_hl(sp);
} // ADD HL SP
void CPU::op_3A() {
    if(exec_flag) {
        ld8_imm(a, read8_mem(h<<8|l));
        dec16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD A [HL-]
void CPU::op_3B() {
    if(exec_flag) dec_sp();
    mcycle = 2;
    opskip = 1;
} // DEC SP
void CPU::op_3C() {
    if(exec_flag) inc8(a);
    opskip = 1;
    mcycle = 1;
} // INC A
void CPU::op_3D() {
    if(exec_flag) dec8(a);
    opskip = 1;
    mcycle = 1;
} // DEC A
void CPU::op_3E() {
    if(exec_flag) {
        ld8_imm(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // LD A n8
void CPU::op_3F() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        c_flag = !c_flag;
        n_flag = 0;
        h_flag = 0;
    }
} // CCF
void CPU::op_40() {
    if(exec_flag) {
        ld8_imm(b,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD B B
void CPU::op_41() {
    if(exec_flag) {
        ld8_imm(b,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD B C
void CPU::op_42() {
    if(exec_flag) {
        ld8_imm(b,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD B D
void CPU::op_43() {
    if(exec_flag) {
        ld8_imm(b,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD B E
void CPU::op_44() {
    if(exec_flag) {
        ld8_imm(b,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD B H
void CPU::op_45() {
    if(exec_flag) {
        ld8_imm(b,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD B L
void CPU::op_46() {
    if(exec_flag) {
        ld8_imm(b,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD B [HL]
void CPU::op_47() {
    if(exec_flag) {
        ld8_imm(b,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD B A
void CPU::op_48() {
    if(exec_flag) {
        ld8_imm(c,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD C B
void CPU::op_49() {
    if(exec_flag) {
        ld8_imm(c,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD C C
void CPU::op_4A() {
    if(exec_flag) {
        ld8_imm(c,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD C D
void CPU::op_4B() {
    if(exec_flag) {
        ld8_imm(c,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD C E
void CPU::op_4C() {
    if(exec_flag) {
        ld8_imm(c,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD C H
void CPU::op_4D() {
    if(exec_flag) {
        ld8_imm(c,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD C L
void CPU::op_4E() {
    if(exec_flag) {
        ld8_imm(c,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD C [HL]
void CPU::op_4F() {
    if(exec_flag) {
        ld8_imm(c,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD C A
void CPU::op_50() {
    if(exec_flag) {
        ld8_imm(d,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD D B
void CPU::op_51() {
    if(exec_flag) {
        ld8_imm(d,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD D C
void CPU::op_52() {
    if(exec_flag) {
        ld8_imm(d,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD D D
void CPU::op_53() {
    if(exec_flag) {
        ld8_imm(d,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD D E
void CPU::op_54() {
    if(exec_flag) {
        ld8_imm(d,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD D H
void CPU::op_55() {
    if(exec_flag) {
        ld8_imm(d,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD D L
void CPU::op_56() {
    if(exec_flag) {
        ld8_imm(d,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD D [HL]
void CPU::op_57() {
    if(exec_flag) {
        ld8_imm(d,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD D A
void CPU::op_58() {
    if(exec_flag) {
        ld8_imm(e,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD E B
void CPU::op_59() {
    if(exec_flag) {
        ld8_imm(e,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD E C
void CPU::op_5A() {
    if(exec_flag) {
        ld8_imm(e,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD E D
void CPU::op_5B() {
    if(exec_flag) {
        ld8_imm(e,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD E E
void CPU::op_5C() {
    if(exec_flag) {
        ld8_imm(e,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD E H
void CPU::op_5D() {
    if(exec_flag) {
        ld8_imm(e,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD E L
void CPU::op_5E() {
    if(exec_flag) {
        ld8_imm(e,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD E [HL]
void CPU::op_5F() {
    if(exec_flag) {
        ld8_imm(e,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD E A
void CPU::op_60() {
    if(exec_flag) {
        ld8_imm(h,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD H B
void CPU::op_61() {
    if(exec_flag) {
        ld8_imm(h,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD H C
void CPU::op_62() {
    if(exec_flag) {
        ld8_imm(h,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD H D
void CPU::op_63() {
    if(exec_flag) {
        ld8_imm(h,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD H E
void CPU::op_64() {
    if(exec_flag) {
        ld8_imm(h,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD H H
void CPU::op_65() {
    if(exec_flag) {
        ld8_imm(h,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD H L
void CPU::op_66() {
    if(exec_flag) {
        ld8_imm(h,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD H [HL]
void CPU::op_67() {
    if(exec_flag) {
        ld8_imm(h,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD H A
void CPU::op_68() {
    if(exec_flag) {
        ld8_imm(l,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD L B
void CPU::op_69() {
    if(exec_flag) {
        ld8_imm(l,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD L C
void CPU::op_6A() {
    if(exec_flag) {
        ld8_imm(l,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD L D
void CPU::op_6B() {
    if(exec_flag) {
        ld8_imm(l,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD L E
void CPU::op_6C() {
    if(exec_flag) {
        ld8_imm(l,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD L H
void CPU::op_6D() {
    if(exec_flag) {
        ld8_imm(l,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD L L
void CPU::op_6E() {
    if(exec_flag) {
        ld8_imm(l,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD L [HL]
void CPU::op_6F() {
    if(exec_flag) {
        ld8_imm(l,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD L A
void CPU::op_70() {
    if(exec_flag) {
        write8_mem(h<<8|l, b);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] B
void CPU::op_71() {
    if(exec_flag) {
        write8_mem(h<<8|l, c);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] C
void CPU::op_72() {
    if(exec_flag) {
        write8_mem(h<<8|l, d);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] D
void CPU::op_73() {
    if(exec_flag) {
        write8_mem(h<<8|l, e);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] E
void CPU::op_74() {
    if(exec_flag) {
        write8_mem(h<<8|l, h);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] H
void CPU::op_75() {
    if(exec_flag) {
        write8_mem(h<<8|l, l);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] L
void CPU::op_76() {
    mcycle = 1;
    opskip = 1;
    if (exec_flag) halt = 1;
} // HALT
void CPU::op_77() {
    if(exec_flag) {
        write8_mem(h<<8|l, a);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] A
void CPU::op_78() {
    if(exec_flag) {
        ld8_imm(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD A B
void CPU::op_79() {
    if(exec_flag) {
        ld8_imm(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD A C
void CPU::op_7A() {
    if(exec_flag) {
        ld8_imm(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD A D
void CPU::op_7B() {
    if(exec_flag) {
        ld8_imm(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD A E
void CPU::op_7C() {
    if(exec_flag) {
        ld8_imm(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD A H
void CPU::op_7D() {
    if(exec_flag) {
        ld8_imm(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD A L
void CPU::op_7E() {
    if(exec_flag) {
        ld8_imm(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD A [HL]
void CPU::op_7F() {
    if(exec_flag) {
        ld8_imm(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD A A
void CPU::op_80() {
    if(exec_flag) {
        add8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A B
void CPU::op_81() {
    if(exec_flag) {
        add8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A C
void CPU::op_82() {
    if(exec_flag) {
        add8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A D
void CPU::op_83() {
    if(exec_flag) {
        add8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A E
void CPU::op_84() {
    if(exec_flag) {
        add8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A H
void CPU::op_85() {
    if(exec_flag) {
        add8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A L
void CPU::op_86() {
    if(exec_flag) {
        add8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // ADD A [HL]
void CPU::op_87() {
    if(exec_flag) {
        add8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A A
void CPU::op_88() {
    if(exec_flag) {
        adc8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A B
void CPU::op_89() {
    if(exec_flag) {
        adc8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A C
void CPU::op_8A() {
    if(exec_flag) {
        adc8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A D
void CPU::op_8B() {
    if(exec_flag) {
        adc8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A E
void CPU::op_8C() {
    if(exec_flag) {
        adc8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A H
void CPU::op_8D() {
    if(exec_flag) {
        adc8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A L
void CPU::op_8E() {
    if(exec_flag) {
        adc8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // ADC A [HL]
void CPU::op_8F() {
    if(exec_flag) {
        adc8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A A
void CPU::op_90() {
    if(exec_flag) {
        sub8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A B
void CPU::op_91() {
    if(exec_flag) {
        sub8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A C
void CPU::op_92() {
    if(exec_flag) {
        sub8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A D
void CPU::op_93() {
    if(exec_flag) {
        sub8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A E
void CPU::op_94() {
    if(exec_flag) {
        sub8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A H
void CPU::op_95() {
    if(exec_flag) {
        sub8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A L
void CPU::op_96() {
    if(exec_flag) {
        sub8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // SUB A [HL]
void CPU::op_97() {
    if(exec_flag) {
        sub8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A A
void CPU::op_98() {
    if(exec_flag) {
        sbc8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A B
void CPU::op_99() {
    if(exec_flag) {
        sbc8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A C
void CPU::op_9A() {
    if(exec_flag) {
        sbc8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A D
void CPU::op_9B() {
    if(exec_flag) {
        sbc8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A E
void CPU::op_9C() {
    if(exec_flag) {
        sbc8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A H
void CPU::op_9D() {
    if(exec_flag) {
        sbc8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A L
void CPU::op_9E() {
    if(exec_flag) {
        sbc8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // SBC A [HL]
void CPU::op_9F() {
    if(exec_flag) {
        sbc8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A A
void CPU::op_A0() {
    if(exec_flag) {
        and8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // AND A B
void CPU::op_A1() {
    if(exec_flag) {
        and8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // AND A C
void CPU::op_A2() {
    if(exec_flag) {
        and8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // AND A D
void CPU::op_A3() {
    if(exec_flag) {
        and8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // AND A E
void CPU::op_A4() {
    if(exec_flag) {
        and8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // AND A H
void CPU::op_A5() {
    if(exec_flag) {
        and8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // AND A L
void CPU::op_A6() {
    if(exec_flag) {
        and8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // AND A [HL]
void CPU::op_A7() {
    if(exec_flag) {
        and8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // AND A A
void CPU::op_A8() {
    if(exec_flag) {
        xor8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A B
void CPU::op_A9() {
    if(exec_flag) {
        xor8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A C
void CPU::op_AA() {
    if(exec_flag) {
        xor8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A D
void CPU::op_AB() {
    if(exec_flag) {
        xor8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A E
void CPU::op_AC() {
    if(exec_flag) {
        xor8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A H
void CPU::op_AD() {
    if(exec_flag) {
        xor8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A L
void CPU::op_AE() {
    if(exec_flag) {
        xor8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // XOR A [HL]
void CPU::op_AF() {
    if(exec_flag) {
        xor8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A A
void CPU::op_B0() {
    if(exec_flag) {
        or8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // OR A B
void CPU::op_B1() {
    if(exec_flag) {
        or8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // OR A C
void CPU::op_B2() {
    if(exec_flag) {
        or8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // OR A D
void CPU::op_B3() {
    if(exec_flag) {
        or8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // OR A E
void CPU::op_B4() {
    if(exec_flag) {
        or8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // OR A H
void CPU::op_B5() {
    if(exec_flag) {
        or8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // OR A L
void CPU::op_B6() {
    if(exec_flag) {
        or8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // OR A [HL]
void CPU::op_B7() {
    if(exec_flag) {
        or8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // OR A A
void CPU::op_B8() {
    if(exec_flag) {
        cp8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // CP A B
void CPU::op_B9() {
    if(exec_flag) {
        cp8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // CP A C
void CPU::op_BA() {
    if(exec_flag) {
        cp8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // CP A D
void CPU::op_BB() {
    if(exec_flag) {
        cp8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // CP A E
void CPU::op_BC() {
    if(exec_flag) {
        cp8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // CP A H
void CPU::op_BD() {
    if(exec_flag) {
        cp8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // CP A L
void CPU::op_BE() {
    if(exec_flag) {
        cp8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // CP A [HL]
void CPU::op_BF() {
    if(exec_flag) {
        cp8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // CP A A
void CPU::op_C0() {
    if (!z_flag) mcycle = 5;
    else mcycle = 2;
    opskip=1;
    if(exec_flag) retc(!z_flag); // May override opskip if jump
} // RET NZ
void CPU::op_C1() {
    opskip = 1;
    mcycle = 3;
    if(exec_flag) pop(b, c);
} // POP BC
void CPU::op_C2() {
    opskip=3;
    if(exec_flag) jpc(!z_flag, read16_mem(pc+1)); // May override opskip if jump
    if (!z_flag) mcycle = 4;
    else mcycle = 3;
} // JP NZ a16
void CPU::op_C3() {
    mcycle=4;
    opskip=3;
    if(exec_flag) jp(read16_mem(pc+1)); // May override opskip if jump
} // JP a16
void CPU::op_C4() {
    if (!z_flag) mcycle = 6;
    else mcycle = 3;
    opskip=3;
    if(exec_flag) callc(!z_flag); // May override opskip if jump
} // CALL NZ a16
void CPU::op_C5() {
    opskip = 1;
    mcycle = 4;
    if(exec_flag) push(b, c);
} // PUSH BC
void CPU::op_C6() {
    if(exec_flag) {
        add8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // ADD A n8
void CPU::op_C7() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x00); // May override opskip if jump
} // RST $00
void CPU::op_C8() {
    if (z_flag) mcycle = 5;
    else mcycle = 2;
    opskip=1;
    if(exec_flag) retc(z_flag); // May override opskip if jump
} // RET Z
void CPU::op_C9() {
    mcycle=4;
    opskip=1;
    if(exec_flag) ret(); // May override opskip if jump
} // RET
void CPU::op_CA() {
    opskip=3;
    if(exec_flag) jpc(z_flag, read16_mem(pc+1));
    if (z_flag) mcycle = 4;
    else mcycle = 3;
} // JP Z a16
void CPU::op_CB() {
    (this->*jump_table_prefixed[read8_mem(pc+1)])();
} // PREFIX
void CPU::op_CC() {
    if (z_flag) mcycle = 6;
    else mcycle = 3;
    opskip=3;
    if(exec_flag) callc(z_flag); // May override opskip if jump
} // CALL Z a16
void CPU::op_CD() {
    mcycle=6;
    opskip=3;
    if(exec_flag) call(); // May override opskip if jump
} // CALL a16
void CPU::op_CE() {
    if(exec_flag) {
        adc8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // ADC A n8
void CPU::op_CF() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x08); // May override opskip if jump
} // RST $08
void CPU::op_D0() {
    if (!c_flag) mcycle = 5;
    else mcycle = 2;
    opskip=1;
    if(exec_flag) retc(!c_flag); // May override opskip if jump
} // RET NC
void CPU::op_D1() {
    opskip = 1;
    mcycle = 3;
    if(exec_flag) pop(d, e);
} // POP DE
void CPU::op_D2() {
    opskip=3;
    if(exec_flag) jpc(!c_flag, read16_mem(pc+1));
    if (!c_flag) mcycle = 4;
    else mcycle = 3;
} // JP NC a16
void CPU::op_D3() {
} // ILLEGAL_D3
void CPU::op_D4() {
    if (!c_flag) mcycle = 6;
    else mcycle = 3;
    opskip=3;
    if(exec_flag) callc(!c_flag); // May override opskip if jump
} // CALL NC a16
void CPU::op_D5() {
    opskip = 1;
    mcycle = 4;
    if(exec_flag) push(d, e);
} // PUSH DE
void CPU::op_D6() {
    if(exec_flag) {
        sub8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // SUB A n8
void CPU::op_D7() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x10); // May override opskip if jump
} // RST $10
void CPU::op_D8() {
    if (c_flag) mcycle = 5;
    else mcycle = 2;
    opskip=1;
    if(exec_flag) retc(c_flag); // May override opskip if jump
} // RET C
void CPU::op_D9() {
    opskip = 1;
    mcycle = 4;
    if (exec_flag) {
        ret();
        ime = 1;
    }
} // RETI
void CPU::op_DA() {
    opskip=3;
    if(exec_flag) jpc(c_flag, read16_mem(pc+1));
    if (z_flag) mcycle = 4;
    else mcycle = 3;
} // JP C a16
void CPU::op_DB() {
} // ILLEGAL_DB
void CPU::op_DC() {
    if (c_flag) mcycle = 6;
    else mcycle = 3;
    opskip=3;
    if(exec_flag) callc(c_flag); // May override opskip if jump
} // CALL C a16
void CPU::op_DD() {
} // ILLEGAL_DD
void CPU::op_DE() {
    if(exec_flag) {
        sbc8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // SBC A n8
void CPU::op_DF() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x18); // May override opskip if jump
} // RST $18
void CPU::op_E0() {
    opskip = 2;
    mcycle = 3;
    if(exec_flag) write8_mem(0xFF << 8 | read8_mem(pc+1), a);
} // LDH [a8] A
void CPU::op_E1() {
    opskip = 1;
    mcycle = 3;
    if(exec_flag) pop(h, l);
} // POP HL
void CPU::op_E2() {
    opskip = 1;
    mcycle = 2;
    if(exec_flag) write8_mem(0xFF << 8 | c, a);
} // LD [C] A
void CPU::op_E3() {
} // ILLEGAL_E3
void CPU::op_E4() {
} // ILLEGAL_E4
void CPU::op_E5() {
    opskip = 1;
    mcycle = 4;
    if(exec_flag) push(h, l);
} // PUSH HL
void CPU::op_E6() {
    if(exec_flag) {
        and8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // AND A n8
void CPU::op_E7() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x20); // May override opskip if jump
} // RST $20
void CPU::op_E8() {
    opskip = 2;
    mcycle = 4;
    if (exec_flag) add_sp(read8_mem(pc+1));
} // ADD SP e8
void CPU::op_E9() {
    opskip=1;
    mcycle=1;
    if(exec_flag) jp(h<<8|l);
} // JP HL
void CPU::op_EA() {
    opskip = 3;
    mcycle = 4;
    if(exec_flag) write8_mem(read16_mem(pc+1), a);
} // LD [a16] A
void CPU::op_EB() {
} // ILLEGAL_EB
void CPU::op_EC() {
} // ILLEGAL_EC
void CPU::op_ED() {
} // ILLEGAL_ED
void CPU::op_EE() {
    if(exec_flag) {
        xor8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // XOR A n8
void CPU::op_EF() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x28); // May override opskip if jump
} // RST $28
void CPU::op_F0() {
    opskip = 2;
    mcycle = 3;
    if(exec_flag) ld8_imm(a, read8_mem(0xFF << 8 | read8_mem(pc+1)));
} // LDH A [a8]
void CPU::op_F1() {
    opskip = 1;
    mcycle = 3;
    if(exec_flag) {
        uint8_t f = read8_mem(sp++);
        a = read8_mem(sp++);
        z_flag = f >> 7;
        n_flag = (f >> 6) & 0x1;
        h_flag = (f >> 5) & 0x1;
        c_flag = (f >> 4) & 0x1;
    }
} // POP AF
void CPU::op_F2() {
    opskip = 1;
    mcycle = 2;
    if(exec_flag) ld8_imm(a, read8_mem(0xFF << 8 | c));
} // LD A [C]
void CPU::op_F3() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) {
        ime_next = 0;
        ime = 0;
    }
} // DI
void CPU::op_F4() {
} // ILLEGAL_F4
void CPU::op_F5() {
    opskip = 1;
    mcycle = 4;
    if(exec_flag) {
        write8_mem(--sp, a);
        write8_mem(--sp, z_flag << 7 | n_flag << 6 | h_flag << 5 | c_flag << 4);
    }
} // PUSH AF
void CPU::op_F6() {
    if(exec_flag) {
        or8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // OR A n8
void CPU::op_F7() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x30); // May override opskip if jump
} // RST $30
void CPU::op_F8() {
    opskip = 2;
    mcycle = 3;
    if(exec_flag) {
        uint16_t temp_sp = sp;
        add_sp(read8_mem(pc+1));
        ld16_imm(h, l, sp);
        sp = temp_sp;
    }
} // LD HL SP e8
void CPU::op_F9() {
    opskip = 1;
    mcycle = 2;
    if(exec_flag) sp = h << 8 | l;
} // LD SP HL
void CPU::op_FA() {
    opskip = 3;
    mcycle = 4;
    if(exec_flag) ld8_imm(a, read8_mem(read16_mem(pc+1)));
} // LD A [a16]
void CPU::op_FB() {
    opskip = 1;
    mcycle = 1;
    if (exec_flag) ime_next = 1;
} // EI
void CPU::op_FC() {
} // ILLEGAL_FC
void CPU::op_FD() {
} // ILLEGAL_FD
void CPU::op_FE() {
    if(exec_flag) {
        cp8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // CP A n8
void CPU::op_FF() {
    mcycle=4;
    opskip=1;
    if(exec_flag) rst(0x38); // May override opskip if jump
} // RST $38
