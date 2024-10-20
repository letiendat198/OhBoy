#include "cpu.h"

void Cpu::op_CB00(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rlc(b);
} // RLC B
void Cpu::op_CB01(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rlc(c);
} // RLC C
void Cpu::op_CB02(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rlc(d);
} // RLC D
void Cpu::op_CB03(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rlc(e);
} // RLC E
void Cpu::op_CB04(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rlc(h);
} // RLC H
void Cpu::op_CB05(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rlc(l);
} // RLC L
void Cpu::op_CB06(){
	opskip=2;
	mcycle=4;
	if (exec_flag) rlc_hl();
} // RLC [HL]
void Cpu::op_CB07(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rlc(a);
} // RLC A
void Cpu::op_CB08(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rrc(b);
} // RRC B
void Cpu::op_CB09(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rrc(c);
} // RRC C
void Cpu::op_CB0A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rrc(d);
} // RRC D
void Cpu::op_CB0B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rrc(e);
} // RRC E
void Cpu::op_CB0C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rrc(h);
} // RRC H
void Cpu::op_CB0D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rrc(l);
} // RRC L
void Cpu::op_CB0E(){
	opskip=2;
	mcycle=4;
	if (exec_flag) rrc_hl();
} // RRC [HL]
void Cpu::op_CB0F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rrc(a);
} // RRC A
void Cpu::op_CB10(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rl(b);
} // RL B
void Cpu::op_CB11(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rl(c);
} // RL C
void Cpu::op_CB12(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rl(d);
} // RL D
void Cpu::op_CB13(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rl(e);
} // RL E
void Cpu::op_CB14(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rl(h);
} // RL H
void Cpu::op_CB15(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rl(l);
} // RL L
void Cpu::op_CB16(){
	opskip=2;
	mcycle=4;
	if (exec_flag) rl_hl();
} // RL [HL]
void Cpu::op_CB17(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rl(a);
} // RL A
void Cpu::op_CB18(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rr(b);
} // RR B
void Cpu::op_CB19(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rr(c);
} // RR C
void Cpu::op_CB1A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rr(d);
} // RR D
void Cpu::op_CB1B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rr(e);
} // RR E
void Cpu::op_CB1C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rr(h);
} // RR H
void Cpu::op_CB1D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rr(l);
} // RR L
void Cpu::op_CB1E(){
	opskip=2;
	mcycle=4;
	if (exec_flag) rr_hl();
} // RR [HL]
void Cpu::op_CB1F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) rr(a);
} // RR A
void Cpu::op_CB20(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sla(b);
} // SLA B
void Cpu::op_CB21(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sla(c);
} // SLA C
void Cpu::op_CB22(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sla(d);
} // SLA D
void Cpu::op_CB23(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sla(e);
} // SLA E
void Cpu::op_CB24(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sla(h);
} // SLA H
void Cpu::op_CB25(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sla(l);
} // SLA L
void Cpu::op_CB26(){
	opskip=2;
	mcycle=4;
	if (exec_flag) sla_hl();
} // SLA [HL]
void Cpu::op_CB27(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sla(a);
} // SLA A
void Cpu::op_CB28(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sra(b);
} // SRA B
void Cpu::op_CB29(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sra(c);
} // SRA C
void Cpu::op_CB2A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sra(d);
} // SRA D
void Cpu::op_CB2B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sra(e);
} // SRA E
void Cpu::op_CB2C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sra(h);
} // SRA H
void Cpu::op_CB2D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sra(l);
} // SRA L
void Cpu::op_CB2E(){
	opskip=2;
	mcycle=4;
	if (exec_flag) sra_hl();
} // SRA [HL]
void Cpu::op_CB2F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) sra(a);
} // SRA A
void Cpu::op_CB30(){
	opskip=2;
	mcycle=2;
	if (exec_flag) swap(b);
} // SWAP B
void Cpu::op_CB31(){
	opskip=2;
	mcycle=2;
	if (exec_flag) swap(c);
} // SWAP C
void Cpu::op_CB32(){
	opskip=2;
	mcycle=2;
	if (exec_flag) swap(d);
} // SWAP D
void Cpu::op_CB33(){
	opskip=2;
	mcycle=2;
	if (exec_flag) swap(e);
} // SWAP E
void Cpu::op_CB34(){
	opskip=2;
	mcycle=2;
	if (exec_flag) swap(h);
} // SWAP H
void Cpu::op_CB35(){
	opskip=2;
	mcycle=2;
	if (exec_flag) swap(l);
} // SWAP L
void Cpu::op_CB36(){
	opskip=2;
	mcycle=4;
	if (exec_flag) swap_hl();
} // SWAP [HL]
void Cpu::op_CB37(){
	opskip=2;
	mcycle=2;
	if (exec_flag) swap(a);
} // SWAP A
void Cpu::op_CB38(){
	opskip=2;
	mcycle=2;
	if (exec_flag) srl(b);
} // SRL B
void Cpu::op_CB39(){
	opskip=2;
	mcycle=2;
	if (exec_flag) srl(c);
} // SRL C
void Cpu::op_CB3A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) srl(d);
} // SRL D
void Cpu::op_CB3B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) srl(e);
} // SRL E
void Cpu::op_CB3C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) srl(h);
} // SRL H
void Cpu::op_CB3D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) srl(l);
} // SRL L
void Cpu::op_CB3E(){
	opskip=2;
	mcycle=4;
	if (exec_flag) srl_hl();
} // SRL [HL]
void Cpu::op_CB3F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) srl(a);
} // SRL A
void Cpu::op_CB40(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(0, b);
} // BIT 0 B
void Cpu::op_CB41(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(0, c);
} // BIT 0 C
void Cpu::op_CB42(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(0, d);
} // BIT 0 D
void Cpu::op_CB43(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(0, e);
} // BIT 0 E
void Cpu::op_CB44(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(0, h);
} // BIT 0 H
void Cpu::op_CB45(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(0, l);
} // BIT 0 L
void Cpu::op_CB46(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(0, read8_mem(h<<8|l));
} // BIT 0 [HL]
void Cpu::op_CB47(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(0, a);
} // BIT 0 A
void Cpu::op_CB48(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(1, b);
} // BIT 1 B
void Cpu::op_CB49(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(1, c);
} // BIT 1 C
void Cpu::op_CB4A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(1, d);
} // BIT 1 D
void Cpu::op_CB4B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(1, e);
} // BIT 1 E
void Cpu::op_CB4C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(1, h);
} // BIT 1 H
void Cpu::op_CB4D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(1, l);
} // BIT 1 L
void Cpu::op_CB4E(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(1, read8_mem(h<<8|l));
} // BIT 1 [HL]
void Cpu::op_CB4F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(1, a);
} // BIT 1 A
void Cpu::op_CB50(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(2, b);
} // BIT 2 B
void Cpu::op_CB51(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(2, c);
} // BIT 2 C
void Cpu::op_CB52(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(2, d);
} // BIT 2 D
void Cpu::op_CB53(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(2, e);
} // BIT 2 E
void Cpu::op_CB54(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(2, h);
} // BIT 2 H
void Cpu::op_CB55(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(2, l);
} // BIT 2 L
void Cpu::op_CB56(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(2, read8_mem(h<<8|l));
} // BIT 2 [HL]
void Cpu::op_CB57(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(2, a);
} // BIT 2 A
void Cpu::op_CB58(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(3, b);
} // BIT 3 B
void Cpu::op_CB59(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(3, c);
} // BIT 3 C
void Cpu::op_CB5A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(3, d);
} // BIT 3 D
void Cpu::op_CB5B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(3, e);
} // BIT 3 E
void Cpu::op_CB5C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(3, h);
} // BIT 3 H
void Cpu::op_CB5D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(3, l);
} // BIT 3 L
void Cpu::op_CB5E(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(3, read8_mem(h<<8|l));
} // BIT 3 [HL]
void Cpu::op_CB5F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(3, a);
} // BIT 3 A
void Cpu::op_CB60(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(4, b);
} // BIT 4 B
void Cpu::op_CB61(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(4, c);
} // BIT 4 C
void Cpu::op_CB62(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(4, d);
} // BIT 4 D
void Cpu::op_CB63(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(4, e);
} // BIT 4 E
void Cpu::op_CB64(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(4, h);
} // BIT 4 H
void Cpu::op_CB65(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(4, l);
} // BIT 4 L
void Cpu::op_CB66(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(4, read8_mem(h<<8|l));
} // BIT 4 [HL]
void Cpu::op_CB67(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(4, a);
} // BIT 4 A
void Cpu::op_CB68(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(5, b);
} // BIT 5 B
void Cpu::op_CB69(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(5, c);
} // BIT 5 C
void Cpu::op_CB6A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(5, d);
} // BIT 5 D
void Cpu::op_CB6B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(5, e);
} // BIT 5 E
void Cpu::op_CB6C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(5, h);
} // BIT 5 H
void Cpu::op_CB6D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(5, l);
} // BIT 5 L
void Cpu::op_CB6E(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(5, read8_mem(h<<8|l));
} // BIT 5 [HL]
void Cpu::op_CB6F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(5, a);
} // BIT 5 A
void Cpu::op_CB70(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(6, b);
} // BIT 6 B
void Cpu::op_CB71(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(6, c);
} // BIT 6 C
void Cpu::op_CB72(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(6, d);
} // BIT 6 D
void Cpu::op_CB73(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(6, e);
} // BIT 6 E
void Cpu::op_CB74(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(6, h);
} // BIT 6 H
void Cpu::op_CB75(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(6, l);
} // BIT 6 L
void Cpu::op_CB76(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(6, read8_mem(h<<8|l));
} // BIT 6 [HL]
void Cpu::op_CB77(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(6, a);
} // BIT 6 A
void Cpu::op_CB78(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(7, b);
} // BIT 7 B
void Cpu::op_CB79(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(7, c);
} // BIT 7 C
void Cpu::op_CB7A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(7, d);
} // BIT 7 D
void Cpu::op_CB7B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(7, e);
} // BIT 7 E
void Cpu::op_CB7C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(7, h);
} // BIT 7 H
void Cpu::op_CB7D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(7, l);
} // BIT 7 L
void Cpu::op_CB7E(){
	opskip=2;
	mcycle=3;
	if (exec_flag) bit(7, read8_mem(h<<8|l));
} // BIT 7 [HL]
void Cpu::op_CB7F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) bit(7, a);
} // BIT 7 A
void Cpu::op_CB80(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(0, b);
} // RES 0 B
void Cpu::op_CB81(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(0, c);
} // RES 0 C
void Cpu::op_CB82(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(0, d);
} // RES 0 D
void Cpu::op_CB83(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(0, e);
} // RES 0 E
void Cpu::op_CB84(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(0, h);
} // RES 0 H
void Cpu::op_CB85(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(0, l);
} // RES 0 L
void Cpu::op_CB86(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(0);
} // RES 0 [HL]
void Cpu::op_CB87(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(0, a);
} // RES 0 A
void Cpu::op_CB88(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(1, b);
} // RES 1 B
void Cpu::op_CB89(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(1, c);
} // RES 1 C
void Cpu::op_CB8A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(1, d);
} // RES 1 D
void Cpu::op_CB8B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(1, e);
} // RES 1 E
void Cpu::op_CB8C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(1, h);
} // RES 1 H
void Cpu::op_CB8D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(1, l);
} // RES 1 L
void Cpu::op_CB8E(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(1);
} // RES 1 [HL]
void Cpu::op_CB8F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(1, a);
} // RES 1 A
void Cpu::op_CB90(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(2, b);
} // RES 2 B
void Cpu::op_CB91(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(2, c);
} // RES 2 C
void Cpu::op_CB92(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(2, d);
} // RES 2 D
void Cpu::op_CB93(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(2, e);
} // RES 2 E
void Cpu::op_CB94(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(2, h);
} // RES 2 H
void Cpu::op_CB95(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(2, l);
} // RES 2 L
void Cpu::op_CB96(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(2);
} // RES 2 [HL]
void Cpu::op_CB97(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(2, a);
} // RES 2 A
void Cpu::op_CB98(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(3, b);
} // RES 3 B
void Cpu::op_CB99(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(3, c);
} // RES 3 C
void Cpu::op_CB9A(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(3, d);
} // RES 3 D
void Cpu::op_CB9B(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(3, e);
} // RES 3 E
void Cpu::op_CB9C(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(3, h);
} // RES 3 H
void Cpu::op_CB9D(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(3, l);
} // RES 3 L
void Cpu::op_CB9E(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(3);
} // RES 3 [HL]
void Cpu::op_CB9F(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(3, a);
} // RES 3 A
void Cpu::op_CBA0(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(4, b);
} // RES 4 B
void Cpu::op_CBA1(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(4, c);
} // RES 4 C
void Cpu::op_CBA2(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(4, d);
} // RES 4 D
void Cpu::op_CBA3(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(4, e);
} // RES 4 E
void Cpu::op_CBA4(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(4, h);
} // RES 4 H
void Cpu::op_CBA5(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(4, l);
} // RES 4 L
void Cpu::op_CBA6(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(4);
} // RES 4 [HL]
void Cpu::op_CBA7(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(4, a);
} // RES 4 A
void Cpu::op_CBA8(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(5, b);
} // RES 5 B
void Cpu::op_CBA9(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(5, c);
} // RES 5 C
void Cpu::op_CBAA(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(5, d);
} // RES 5 D
void Cpu::op_CBAB(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(5, e);
} // RES 5 E
void Cpu::op_CBAC(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(5, h);
} // RES 5 H
void Cpu::op_CBAD(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(5, l);
} // RES 5 L
void Cpu::op_CBAE(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(5);
} // RES 5 [HL]
void Cpu::op_CBAF(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(5, a);
} // RES 5 A
void Cpu::op_CBB0(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(6, b);
} // RES 6 B
void Cpu::op_CBB1(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(6, c);
} // RES 6 C
void Cpu::op_CBB2(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(6, d);
} // RES 6 D
void Cpu::op_CBB3(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(6, e);
} // RES 6 E
void Cpu::op_CBB4(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(6, h);
} // RES 6 H
void Cpu::op_CBB5(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(6, l);
} // RES 6 L
void Cpu::op_CBB6(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(6);
} // RES 6 [HL]
void Cpu::op_CBB7(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(6, a);
} // RES 6 A
void Cpu::op_CBB8(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(7, b);
} // RES 7 B
void Cpu::op_CBB9(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(7, c);
} // RES 7 C
void Cpu::op_CBBA(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(7, d);
} // RES 7 D
void Cpu::op_CBBB(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(7, e);
} // RES 7 E
void Cpu::op_CBBC(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(7, h);
} // RES 7 H
void Cpu::op_CBBD(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(7, l);
} // RES 7 L
void Cpu::op_CBBE(){
	opskip=2;
	mcycle=4;
	if (exec_flag) res_hl(7);
} // RES 7 [HL]
void Cpu::op_CBBF(){
	opskip=2;
	mcycle=2;
	if (exec_flag) res(7, a);
} // RES 7 A
void Cpu::op_CBC0(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(0, b);
} // SET 0 B
void Cpu::op_CBC1(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(0, c);
} // SET 0 C
void Cpu::op_CBC2(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(0, d);
} // SET 0 D
void Cpu::op_CBC3(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(0, e);
} // SET 0 E
void Cpu::op_CBC4(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(0, h);
} // SET 0 H
void Cpu::op_CBC5(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(0, l);
} // SET 0 L
void Cpu::op_CBC6(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(0);
} // SET 0 [HL]
void Cpu::op_CBC7(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(0, a);
} // SET 0 A
void Cpu::op_CBC8(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(1, b);
} // SET 1 B
void Cpu::op_CBC9(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(1, c);
} // SET 1 C
void Cpu::op_CBCA(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(1, d);
} // SET 1 D
void Cpu::op_CBCB(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(1, e);
} // SET 1 E
void Cpu::op_CBCC(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(1, h);
} // SET 1 H
void Cpu::op_CBCD(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(1, l);
} // SET 1 L
void Cpu::op_CBCE(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(1);
} // SET 1 [HL]
void Cpu::op_CBCF(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(1, a);
} // SET 1 A
void Cpu::op_CBD0(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(2, b);
} // SET 2 B
void Cpu::op_CBD1(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(2, c);
} // SET 2 C
void Cpu::op_CBD2(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(2, d);
} // SET 2 D
void Cpu::op_CBD3(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(2, e);
} // SET 2 E
void Cpu::op_CBD4(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(2, h);
} // SET 2 H
void Cpu::op_CBD5(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(2, l);
} // SET 2 L
void Cpu::op_CBD6(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(2);
} // SET 2 [HL]
void Cpu::op_CBD7(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(2, a);
} // SET 2 A
void Cpu::op_CBD8(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(3, b);
} // SET 3 B
void Cpu::op_CBD9(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(3, c);
} // SET 3 C
void Cpu::op_CBDA(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(3, d);
} // SET 3 D
void Cpu::op_CBDB(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(3, e);
} // SET 3 E
void Cpu::op_CBDC(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(3, h);
} // SET 3 H
void Cpu::op_CBDD(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(3, l);
} // SET 3 L
void Cpu::op_CBDE(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(3);
} // SET 3 [HL]
void Cpu::op_CBDF(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(3, a);
} // SET 3 A
void Cpu::op_CBE0(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(4, b);
} // SET 4 B
void Cpu::op_CBE1(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(4, c);
} // SET 4 C
void Cpu::op_CBE2(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(4, d);
} // SET 4 D
void Cpu::op_CBE3(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(4, e);
} // SET 4 E
void Cpu::op_CBE4(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(4, h);
} // SET 4 H
void Cpu::op_CBE5(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(4, l);
} // SET 4 L
void Cpu::op_CBE6(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(4);
} // SET 4 [HL]
void Cpu::op_CBE7(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(4, a);
} // SET 4 A
void Cpu::op_CBE8(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(5, b);
} // SET 5 B
void Cpu::op_CBE9(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(5, c);
} // SET 5 C
void Cpu::op_CBEA(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(5, d);
} // SET 5 D
void Cpu::op_CBEB(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(5, e);
} // SET 5 E
void Cpu::op_CBEC(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(5, h);
} // SET 5 H
void Cpu::op_CBED(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(5, l);
} // SET 5 L
void Cpu::op_CBEE(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(5);
} // SET 5 [HL]
void Cpu::op_CBEF(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(5, a);
} // SET 5 A
void Cpu::op_CBF0(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(6, b);
} // SET 6 B
void Cpu::op_CBF1(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(6, c);
} // SET 6 C
void Cpu::op_CBF2(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(6, d);
} // SET 6 D
void Cpu::op_CBF3(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(6, e);
} // SET 6 E
void Cpu::op_CBF4(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(6, h);
} // SET 6 H
void Cpu::op_CBF5(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(6, l);
} // SET 6 L
void Cpu::op_CBF6(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(6);
} // SET 6 [HL]
void Cpu::op_CBF7(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(6, a);
} // SET 6 A
void Cpu::op_CBF8(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(7, b);
} // SET 7 B
void Cpu::op_CBF9(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(7, c);
} // SET 7 C
void Cpu::op_CBFA(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(7, d);
} // SET 7 D
void Cpu::op_CBFB(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(7, e);
} // SET 7 E
void Cpu::op_CBFC(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(7, h);
} // SET 7 H
void Cpu::op_CBFD(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(7, l);
} // SET 7 L
void Cpu::op_CBFE(){
	opskip=2;
	mcycle=4;
	if (exec_flag) set_hl(7);
} // SET 7 [HL]
void Cpu::op_CBFF(){
	opskip=2;
	mcycle=2;
	if (exec_flag) set(7, a);
} // SET 7 A
