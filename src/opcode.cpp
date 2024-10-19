#include "cpu.h"

void Cpu::op_00() {
    mcycle = 1;
    opskip = 1;
} // NOP
void Cpu::op_01() {
    if(exec_flag) ld16_imm(b, c, read16_mem(pc+1));
    mcycle = 3;
    opskip = 3;
} // LD BC n16
void Cpu::op_02() {
    if(exec_flag) write8_mem(b<<8 | c, a);
    mcycle = 2;
    opskip = 1;
} // LD [BC] A
void Cpu::op_03() {
    if(exec_flag) inc16(b,c);
    mcycle = 2;
    opskip = 1;
} // INC BC
void Cpu::op_04() {
    if(exec_flag) inc8(b);
    mcycle = 1;
    opskip = 1;
} // INC B
void Cpu::op_05() {
    if(exec_flag) dec8(b);
    mcycle = 1;
    opskip = 1;
} // DEC B
void Cpu::op_06() {
    if(exec_flag) ld8_imm(b, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD B n8
void Cpu::op_07() {
} // RLCA
void Cpu::op_08() {
} // LD [a16] SP
void Cpu::op_09() {
} // ADD HL BC
void Cpu::op_0A() {
    if(exec_flag) ld8_imm(a, read8_mem(b<<8 | c));
    mcycle = 2;
    opskip = 1;
} // LD A [BC]
void Cpu::op_0B() {
    if(exec_flag) dec16(b,c);
    mcycle = 2;
    opskip = 1;
} // DEC BC
void Cpu::op_0C() {
    if(exec_flag) inc8(c);
    mcycle = 1;
    opskip = 1;
} // INC C
void Cpu::op_0D() {
    if(exec_flag) dec8(c);
    mcycle = 1;
    opskip = 1;
} // DEC C
void Cpu::op_0E() {
    if(exec_flag) ld8_imm(c, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD C n8
void Cpu::op_0F() {
} // RRCA
void Cpu::op_10() {
} // STOP n8
void Cpu::op_11() {
    if(exec_flag) ld16_imm(d, e, read16_mem(pc+1));
    mcycle = 3;
    opskip = 3;
} // LD DE n16
void Cpu::op_12() {
    if(exec_flag) write8_mem(d<<8|e, a);
    mcycle = 2;
    opskip = 1;
} // LD [DE] A
void Cpu::op_13() {
    if(exec_flag) inc16(d,e);
    mcycle = 2;
    opskip = 1;
} // INC DE
void Cpu::op_14() {
    if(exec_flag) inc8(d);
    mcycle = 1;
    opskip = 1;
} // INC D
void Cpu::op_15() {
    if(exec_flag) dec8(d);
    mcycle = 1;
    opskip = 1;
} // DEC D
void Cpu::op_16() {
    if(exec_flag) ld8_imm(d, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD D n8
void Cpu::op_17() {
} // RLA
void Cpu::op_18() {
    if(exec_flag) jr(read8_mem(pc+1));
    mcycle=3;
    opskip=2;
} // JR e8
void Cpu::op_19() {
} // ADD HL DE
void Cpu::op_1A() {
    if(exec_flag) ld8_imm(a, read8_mem(d<<8|e));
    mcycle=2;
    opskip=1;
} // LD A [DE]
void Cpu::op_1B() {
    if(exec_flag) dec16(d,e);
    mcycle = 2;
    opskip = 1;
} // DEC DE
void Cpu::op_1C() {
    if(exec_flag) inc8(e);
    mcycle = 1;
    opskip = 1;
} // INC E
void Cpu::op_1D() {
    if(exec_flag) dec8(e);
    mcycle = 1;
    opskip = 1;
} // DEC E
void Cpu::op_1E() {
    if(exec_flag) ld8_imm(e, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD E n8
void Cpu::op_1F() {
} // RRA
void Cpu::op_20() {
    if(exec_flag) jrc(!z_flag, read8_mem(pc+1));
    mcycle=2;
    opskip=2;
} // JR NZ e8
void Cpu::op_21() {
    if(exec_flag) ld16_imm(h, l, read16_mem(pc+1));
    mcycle = 3;
    opskip = 3;
} // LD HL n16
void Cpu::op_22() {
    if(exec_flag) {
        write8_mem(h<<8|l, a);
        inc16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL+] A
void Cpu::op_23() {
    if(exec_flag) inc16(h,l);
    mcycle = 2;
    opskip = 1;
} // INC HL
void Cpu::op_24() {
    if(exec_flag) inc8(h);
    mcycle = 1;
    opskip = 1;
} // INC H
void Cpu::op_25() {
    if(exec_flag) dec8(h);
    mcycle = 1;
    opskip = 1;
} // DEC H
void Cpu::op_26() {
    if(exec_flag) ld8_imm(h, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD H n8
void Cpu::op_27() {
} // DAA
void Cpu::op_28() {
    if(exec_flag) jrc(z_flag, read8_mem(pc+1));
    mcycle=2;
    opskip=2;
} // JR Z e8
void Cpu::op_29() {
} // ADD HL HL
void Cpu::op_2A() {
    if(exec_flag) {
        ld8_imm(a, read8_mem(h<<8|l));
        inc16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD A [HL+]
void Cpu::op_2B() {
    if(exec_flag) dec16(h,l);
    mcycle = 2;
    opskip = 1;
} // DEC HL
void Cpu::op_2C() {
    if(exec_flag) inc8(l);
    mcycle = 1;
    opskip = 1;
} // INC L
void Cpu::op_2D() {
    if(exec_flag) dec8(l);
    mcycle = 1;
    opskip = 1;
} // DEC L
void Cpu::op_2E() {
    if(exec_flag) ld8_imm(l, read8_mem(pc+1));
    mcycle = 2;
    opskip = 2;
} // LD L n8
void Cpu::op_2F() {
} // CPL
void Cpu::op_30() {
    if(exec_flag) jrc(!c_flag, read8_mem(pc+1));
    mcycle=2;
    opskip=2;
} // JR NC e8
void Cpu::op_31() {
    if(exec_flag) sp = read16_mem(pc+1);
    mcycle = 3;
    opskip = 3;
} // LD SP n16
void Cpu::op_32() {
    if(exec_flag) {
        write8_mem(h<<8|l, a);
        dec16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL-] A
void Cpu::op_33() {
    if(exec_flag) sp++;
    mcycle = 2;
    opskip = 1;
} // INC SP
void Cpu::op_34() {
    if(exec_flag) inc_hl();
    mcycle = 3;
    opskip = 1;
} // INC [HL]
void Cpu::op_35() {
    if(exec_flag) dec_hl();
    mcycle = 3;
    opskip = 1;
} // DEC [HL]
void Cpu::op_36() {
    if(exec_flag) write8_mem(h<<8|l, read8_mem(pc+1));
    mcycle = 3;
    opskip = 3;
} // LD [HL] n8
void Cpu::op_37() {
} // SCF
void Cpu::op_38() {
    if(exec_flag) jrc(c_flag, read8_mem(pc+1));
    mcycle=2;
    opskip=2;
} // JR C e8
void Cpu::op_39() {
} // ADD HL SP
void Cpu::op_3A() {
    if(exec_flag) {
        ld8_imm(a, read8_mem(h<<8|l));
        dec16(h,l);
    }
    mcycle = 2;
    opskip = 1;
} // LD A [HL-]
void Cpu::op_3B() {
    if(exec_flag) sp--;
    mcycle = 2;
    opskip = 1;
} // DEC SP
void Cpu::op_3C() {
    if(exec_flag) inc8(a);
    opskip = 1;
    mcycle = 1;
} // INC A
void Cpu::op_3D() {
    if(exec_flag) dec8(a);
    opskip = 1;
    mcycle = 1;
} // DEC A
void Cpu::op_3E() {
    if(exec_flag) {
        ld8_imm(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // LD A n8
void Cpu::op_3F() {
} // CCF
void Cpu::op_40() {
    if(exec_flag) {
        ld8_imm(b,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD B B
void Cpu::op_41() {
    if(exec_flag) {
        ld8_imm(b,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD B C
void Cpu::op_42() {
    if(exec_flag) {
        ld8_imm(b,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD B D
void Cpu::op_43() {
    if(exec_flag) {
        ld8_imm(b,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD B E
void Cpu::op_44() {
    if(exec_flag) {
        ld8_imm(b,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD B H
void Cpu::op_45() {
    if(exec_flag) {
        ld8_imm(b,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD B L
void Cpu::op_46() {
    if(exec_flag) {
        ld8_imm(b,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD B [HL]
void Cpu::op_47() {
    if(exec_flag) {
        ld8_imm(b,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD B A
void Cpu::op_48() {
    if(exec_flag) {
        ld8_imm(c,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD C B
void Cpu::op_49() {
    if(exec_flag) {
        ld8_imm(c,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD C C
void Cpu::op_4A() {
    if(exec_flag) {
        ld8_imm(c,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD C D
void Cpu::op_4B() {
    if(exec_flag) {
        ld8_imm(c,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD C E
void Cpu::op_4C() {
    if(exec_flag) {
        ld8_imm(c,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD C H
void Cpu::op_4D() {
    if(exec_flag) {
        ld8_imm(c,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD C L
void Cpu::op_4E() {
    if(exec_flag) {
        ld8_imm(c,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD C [HL]
void Cpu::op_4F() {
    if(exec_flag) {
        ld8_imm(c,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD C A
void Cpu::op_50() {
    if(exec_flag) {
        ld8_imm(d,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD D B
void Cpu::op_51() {
    if(exec_flag) {
        ld8_imm(d,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD D C
void Cpu::op_52() {
    if(exec_flag) {
        ld8_imm(d,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD D D
void Cpu::op_53() {
    if(exec_flag) {
        ld8_imm(d,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD D E
void Cpu::op_54() {
    if(exec_flag) {
        ld8_imm(d,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD D H
void Cpu::op_55() {
    if(exec_flag) {
        ld8_imm(d,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD D L
void Cpu::op_56() {
    if(exec_flag) {
        ld8_imm(d,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD D [HL]
void Cpu::op_57() {
    if(exec_flag) {
        ld8_imm(d,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD D A
void Cpu::op_58() {
    if(exec_flag) {
        ld8_imm(e,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD E B
void Cpu::op_59() {
    if(exec_flag) {
        ld8_imm(e,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD E C
void Cpu::op_5A() {
    if(exec_flag) {
        ld8_imm(e,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD E D
void Cpu::op_5B() {
    if(exec_flag) {
        ld8_imm(e,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD E E
void Cpu::op_5C() {
    if(exec_flag) {
        ld8_imm(e,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD E H
void Cpu::op_5D() {
    if(exec_flag) {
        ld8_imm(e,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD E L
void Cpu::op_5E() {
    if(exec_flag) {
        ld8_imm(e,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD E [HL]
void Cpu::op_5F() {
    if(exec_flag) {
        ld8_imm(e,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD E A
void Cpu::op_60() {
    if(exec_flag) {
        ld8_imm(h,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD H B
void Cpu::op_61() {
    if(exec_flag) {
        ld8_imm(h,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD H C
void Cpu::op_62() {
    if(exec_flag) {
        ld8_imm(h,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD H D
void Cpu::op_63() {
    if(exec_flag) {
        ld8_imm(h,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD H E
void Cpu::op_64() {
    if(exec_flag) {
        ld8_imm(h,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD H H
void Cpu::op_65() {
    if(exec_flag) {
        ld8_imm(h,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD H L
void Cpu::op_66() {
    if(exec_flag) {
        ld8_imm(h,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD H [HL]
void Cpu::op_67() {
    if(exec_flag) {
        ld8_imm(h,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD H A
void Cpu::op_68() {
    if(exec_flag) {
        ld8_imm(l,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD L B
void Cpu::op_69() {
    if(exec_flag) {
        ld8_imm(l,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD L C
void Cpu::op_6A() {
    if(exec_flag) {
        ld8_imm(l,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD L D
void Cpu::op_6B() {
    if(exec_flag) {
        ld8_imm(l,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD L E
void Cpu::op_6C() {
    if(exec_flag) {
        ld8_imm(l,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD L H
void Cpu::op_6D() {
    if(exec_flag) {
        ld8_imm(l,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD L L
void Cpu::op_6E() {
    if(exec_flag) {
        ld8_imm(l,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD L [HL]
void Cpu::op_6F() {
    if(exec_flag) {
        ld8_imm(l,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD L A
void Cpu::op_70() {
    if(exec_flag) {
        write8_mem(h<<8|l, b);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] B
void Cpu::op_71() {
    if(exec_flag) {
        write8_mem(h<<8|l, c);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] C
void Cpu::op_72() {
    if(exec_flag) {
        write8_mem(h<<8|l, d);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] D
void Cpu::op_73() {
    if(exec_flag) {
        write8_mem(h<<8|l, e);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] E
void Cpu::op_74() {
    if(exec_flag) {
        write8_mem(h<<8|l, h);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] H
void Cpu::op_75() {
    if(exec_flag) {
        write8_mem(h<<8|l, l);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] L
void Cpu::op_76() {
    mcycle = 1;
    opskip = 1;
} // HALT
void Cpu::op_77() {
    if(exec_flag) {
        write8_mem(h<<8|l, a);
    }
    mcycle = 2;
    opskip = 1;
} // LD [HL] A
void Cpu::op_78() {
    if(exec_flag) {
        ld8_imm(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // LD A B
void Cpu::op_79() {
    if(exec_flag) {
        ld8_imm(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // LD A C
void Cpu::op_7A() {
    if(exec_flag) {
        ld8_imm(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // LD A D
void Cpu::op_7B() {
    if(exec_flag) {
        ld8_imm(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // LD A E
void Cpu::op_7C() {
    if(exec_flag) {
        ld8_imm(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // LD A H
void Cpu::op_7D() {
    if(exec_flag) {
        ld8_imm(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // LD A L
void Cpu::op_7E() {
    if(exec_flag) {
        ld8_imm(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // LD A [HL]
void Cpu::op_7F() {
    if(exec_flag) {
        ld8_imm(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // LD A A
void Cpu::op_80() {
    if(exec_flag) {
        add8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A B
void Cpu::op_81() {
    if(exec_flag) {
        add8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A C
void Cpu::op_82() {
    if(exec_flag) {
        add8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A D
void Cpu::op_83() {
    if(exec_flag) {
        add8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A E
void Cpu::op_84() {
    if(exec_flag) {
        add8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A H
void Cpu::op_85() {
    if(exec_flag) {
        add8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A L
void Cpu::op_86() {
    if(exec_flag) {
        add8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // ADD A [HL]
void Cpu::op_87() {
    if(exec_flag) {
        add8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // ADD A A
void Cpu::op_88() {
    if(exec_flag) {
        adc8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A B
void Cpu::op_89() {
    if(exec_flag) {
        adc8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A C
void Cpu::op_8A() {
    if(exec_flag) {
        adc8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A D
void Cpu::op_8B() {
    if(exec_flag) {
        adc8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A E
void Cpu::op_8C() {
    if(exec_flag) {
        adc8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A H
void Cpu::op_8D() {
    if(exec_flag) {
        adc8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A L
void Cpu::op_8E() {
    if(exec_flag) {
        adc8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // ADC A [HL]
void Cpu::op_8F() {
    if(exec_flag) {
        adc8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // ADC A A
void Cpu::op_90() {
    if(exec_flag) {
        sub8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A B
void Cpu::op_91() {
    if(exec_flag) {
        sub8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A C
void Cpu::op_92() {
    if(exec_flag) {
        sub8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A D
void Cpu::op_93() {
    if(exec_flag) {
        sub8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A E
void Cpu::op_94() {
    if(exec_flag) {
        sub8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A H
void Cpu::op_95() {
    if(exec_flag) {
        sub8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A L
void Cpu::op_96() {
    if(exec_flag) {
        sub8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // SUB A [HL]
void Cpu::op_97() {
    if(exec_flag) {
        sub8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // SUB A A
void Cpu::op_98() {
    if(exec_flag) {
        sbc8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A B
void Cpu::op_99() {
    if(exec_flag) {
        sbc8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A C
void Cpu::op_9A() {
    if(exec_flag) {
        sbc8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A D
void Cpu::op_9B() {
    if(exec_flag) {
        sbc8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A E
void Cpu::op_9C() {
    if(exec_flag) {
        sbc8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A H
void Cpu::op_9D() {
    if(exec_flag) {
        sbc8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A L
void Cpu::op_9E() {
    if(exec_flag) {
        sbc8(a,read8_mem(h<<8|l));
    }
    mcycle = 1;
    opskip = 1;
} // SBC A [HL]
void Cpu::op_9F() {
    if(exec_flag) {
        sbc8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // SBC A A
void Cpu::op_A0() {
    if(exec_flag) {
        and8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // AND A B
void Cpu::op_A1() {
    if(exec_flag) {
        and8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // AND A C
void Cpu::op_A2() {
    if(exec_flag) {
        and8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // AND A D
void Cpu::op_A3() {
    if(exec_flag) {
        and8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // AND A E
void Cpu::op_A4() {
    if(exec_flag) {
        and8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // AND A H
void Cpu::op_A5() {
    if(exec_flag) {
        and8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // AND A L
void Cpu::op_A6() {
    if(exec_flag) {
        and8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // AND A [HL]
void Cpu::op_A7() {
    if(exec_flag) {
        and8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // AND A A
void Cpu::op_A8() {
    if(exec_flag) {
        xor8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A B
void Cpu::op_A9() {
    if(exec_flag) {
        xor8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A C
void Cpu::op_AA() {
    if(exec_flag) {
        xor8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A D
void Cpu::op_AB() {
    if(exec_flag) {
        xor8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A E
void Cpu::op_AC() {
    if(exec_flag) {
        xor8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A H
void Cpu::op_AD() {
    if(exec_flag) {
        xor8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A L
void Cpu::op_AE() {
    if(exec_flag) {
        xor8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // XOR A [HL]
void Cpu::op_AF() {
    if(exec_flag) {
        xor8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // XOR A A
void Cpu::op_B0() {
    if(exec_flag) {
        or8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // OR A B
void Cpu::op_B1() {
    if(exec_flag) {
        or8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // OR A C
void Cpu::op_B2() {
    if(exec_flag) {
        or8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // OR A D
void Cpu::op_B3() {
    if(exec_flag) {
        or8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // OR A E
void Cpu::op_B4() {
    if(exec_flag) {
        or8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // OR A H
void Cpu::op_B5() {
    if(exec_flag) {
        or8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // OR A L
void Cpu::op_B6() {
    if(exec_flag) {
        or8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // OR A [HL]
void Cpu::op_B7() {
    if(exec_flag) {
        or8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // OR A A
void Cpu::op_B8() {
    if(exec_flag) {
        cp8(a,b);
    }
    mcycle = 1;
    opskip = 1;
} // CP A B
void Cpu::op_B9() {
    if(exec_flag) {
        cp8(a,c);
    }
    mcycle = 1;
    opskip = 1;
} // CP A C
void Cpu::op_BA() {
    if(exec_flag) {
        cp8(a,d);
    }
    mcycle = 1;
    opskip = 1;
} // CP A D
void Cpu::op_BB() {
    if(exec_flag) {
        cp8(a,e);
    }
    mcycle = 1;
    opskip = 1;
} // CP A E
void Cpu::op_BC() {
    if(exec_flag) {
        cp8(a,h);
    }
    mcycle = 1;
    opskip = 1;
} // CP A H
void Cpu::op_BD() {
    if(exec_flag) {
        cp8(a,l);
    }
    mcycle = 1;
    opskip = 1;
} // CP A L
void Cpu::op_BE() {
    if(exec_flag) {
        cp8(a,read8_mem(h<<8|l));
    }
    mcycle = 2;
    opskip = 1;
} // CP A [HL]
void Cpu::op_BF() {
    if(exec_flag) {
        cp8(a,a);
    }
    mcycle = 1;
    opskip = 1;
} // CP A A
void Cpu::op_C0() {
} // RET NZ
void Cpu::op_C1() {
} // POP BC
void Cpu::op_C2() {
    mcycle=2;
    opskip=3;
    if(exec_flag) jpc(!z_flag, read8_mem(pc+1)); // May override opskip if jump
} // JP NZ a16
void Cpu::op_C3() {
    mcycle=4;
    opskip=3;
    if(exec_flag) jp(read16_mem(pc+1)); // May override opskip if jump
} // JP a16
void Cpu::op_C4() {
} // CALL NZ a16
void Cpu::op_C5() {
} // PUSH BC
void Cpu::op_C6() {
    if(exec_flag) {
        add8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // ADD A n8
void Cpu::op_C7() {
} // RST $00
void Cpu::op_C8() {
} // RET Z
void Cpu::op_C9() {
} // RET
void Cpu::op_CA() {
    mcycle=2;
    opskip=3;
    if(exec_flag) jpc(z_flag, read8_mem(pc+1));
} // JP Z a16
void Cpu::op_CB() {
} // PREFIX
void Cpu::op_CC() {
} // CALL Z a16
void Cpu::op_CD() {
} // CALL a16
void Cpu::op_CE() {
    if(exec_flag) {
        adc8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // ADC A n8
void Cpu::op_CF() {
} // RST $08
void Cpu::op_D0() {
} // RET NC
void Cpu::op_D1() {
} // POP DE
void Cpu::op_D2() {
    mcycle=2;
    opskip=3;
    if(exec_flag) jpc(!c_flag, read8_mem(pc+1));
} // JP NC a16
void Cpu::op_D3() {
} // ILLEGAL_D3
void Cpu::op_D4() {
} // CALL NC a16
void Cpu::op_D5() {
} // PUSH DE
void Cpu::op_D6() {
    if(exec_flag) {
        sub8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // SUB A n8
void Cpu::op_D7() {
} // RST $10
void Cpu::op_D8() {
} // RET C
void Cpu::op_D9() {
} // RETI
void Cpu::op_DA() {
    mcycle=2;
    opskip=3;
    if(exec_flag) jpc(c_flag, read8_mem(pc+1));
} // JP C a16
void Cpu::op_DB() {
} // ILLEGAL_DB
void Cpu::op_DC() {
} // CALL C a16
void Cpu::op_DD() {
} // ILLEGAL_DD
void Cpu::op_DE() {
    if(exec_flag) {
        sbc8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // SBC A n8
void Cpu::op_DF() {
} // RST $18
void Cpu::op_E0() {
} // LDH [a8] A
void Cpu::op_E1() {
} // POP HL
void Cpu::op_E2() {

} // LD [C] A
void Cpu::op_E3() {
} // ILLEGAL_E3
void Cpu::op_E4() {
} // ILLEGAL_E4
void Cpu::op_E5() {
} // PUSH HL
void Cpu::op_E6() {
    if(exec_flag) {
        and8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // AND A n8
void Cpu::op_E7() {
} // RST $20
void Cpu::op_E8() {
} // ADD SP e8
void Cpu::op_E9() {
    mcycle=1;
    opskip=1;
    if(exec_flag) jpc(z_flag, h<<8|l);
} // JP HL
void Cpu::op_EA() {
} // LD [a16] A
void Cpu::op_EB() {
} // ILLEGAL_EB
void Cpu::op_EC() {
} // ILLEGAL_EC
void Cpu::op_ED() {
} // ILLEGAL_ED
void Cpu::op_EE() {
    if(exec_flag) {
        xor8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // XOR A n8
void Cpu::op_EF() {
} // RST $28
void Cpu::op_F0() {
} // LDH A [a8]
void Cpu::op_F1() {
} // POP AF
void Cpu::op_F2() {
} // LD A [C]
void Cpu::op_F3() {
} // DI
void Cpu::op_F4() {
} // ILLEGAL_F4
void Cpu::op_F5() {
} // PUSH AF
void Cpu::op_F6() {
    if(exec_flag) {
        or8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // OR A n8
void Cpu::op_F7() {
} // RST $30
void Cpu::op_F8() {
} // LD HL SP e8
void Cpu::op_F9() {
} // LD SP HL
void Cpu::op_FA() {
} // LD A [a16]
void Cpu::op_FB() {
} // EI
void Cpu::op_FC() {
} // ILLEGAL_FC
void Cpu::op_FD() {
} // ILLEGAL_FD
void Cpu::op_FE() {
    if(exec_flag) {
        cp8(a, read8_mem(pc+1));
    }
    mcycle = 2;
    opskip = 2;
} // CP A n8
void Cpu::op_FF() {
} // RST $38
