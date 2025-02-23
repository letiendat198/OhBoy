#include "cpu.h"

void CPU::op_CB00(){
	opskip=2;
	mcycle=2;
	rlc(b);
} // RLC B
void CPU::op_CB01(){
	opskip=2;
	mcycle=2;
	rlc(c);
} // RLC C
void CPU::op_CB02(){
	opskip=2;
	mcycle=2;
	rlc(d);
} // RLC D
void CPU::op_CB03(){
	opskip=2;
	mcycle=2;
	rlc(e);
} // RLC E
void CPU::op_CB04(){
	opskip=2;
	mcycle=2;
	rlc(h);
} // RLC H
void CPU::op_CB05(){
	opskip=2;
	mcycle=2;
	rlc(l);
} // RLC L
void CPU::op_CB06(){
	opskip=2;
	mcycle=4;
	rlc_hl();
} // RLC [HL]
void CPU::op_CB07(){
	opskip=2;
	mcycle=2;
	rlc(a);
} // RLC A
void CPU::op_CB08(){
	opskip=2;
	mcycle=2;
	rrc(b);
} // RRC B
void CPU::op_CB09(){
	opskip=2;
	mcycle=2;
	rrc(c);
} // RRC C
void CPU::op_CB0A(){
	opskip=2;
	mcycle=2;
	rrc(d);
} // RRC D
void CPU::op_CB0B(){
	opskip=2;
	mcycle=2;
	rrc(e);
} // RRC E
void CPU::op_CB0C(){
	opskip=2;
	mcycle=2;
	rrc(h);
} // RRC H
void CPU::op_CB0D(){
	opskip=2;
	mcycle=2;
	rrc(l);
} // RRC L
void CPU::op_CB0E(){
	opskip=2;
	mcycle=4;
	rrc_hl();
} // RRC [HL]
void CPU::op_CB0F(){
	opskip=2;
	mcycle=2;
	rrc(a);
} // RRC A
void CPU::op_CB10(){
	opskip=2;
	mcycle=2;
	rl(b);
} // RL B
void CPU::op_CB11(){
	opskip=2;
	mcycle=2;
	rl(c);
} // RL C
void CPU::op_CB12(){
	opskip=2;
	mcycle=2;
	rl(d);
} // RL D
void CPU::op_CB13(){
	opskip=2;
	mcycle=2;
	rl(e);
} // RL E
void CPU::op_CB14(){
	opskip=2;
	mcycle=2;
	rl(h);
} // RL H
void CPU::op_CB15(){
	opskip=2;
	mcycle=2;
	rl(l);
} // RL L
void CPU::op_CB16(){
	opskip=2;
	mcycle=4;
	rl_hl();
} // RL [HL]
void CPU::op_CB17(){
	opskip=2;
	mcycle=2;
	rl(a);
} // RL A
void CPU::op_CB18(){
	opskip=2;
	mcycle=2;
	rr(b);
} // RR B
void CPU::op_CB19(){
	opskip=2;
	mcycle=2;
	rr(c);
} // RR C
void CPU::op_CB1A(){
	opskip=2;
	mcycle=2;
	rr(d);
} // RR D
void CPU::op_CB1B(){
	opskip=2;
	mcycle=2;
	rr(e);
} // RR E
void CPU::op_CB1C(){
	opskip=2;
	mcycle=2;
	rr(h);
} // RR H
void CPU::op_CB1D(){
	opskip=2;
	mcycle=2;
	rr(l);
} // RR L
void CPU::op_CB1E(){
	opskip=2;
	mcycle=4;
	rr_hl();
} // RR [HL]
void CPU::op_CB1F(){
	opskip=2;
	mcycle=2;
	rr(a);
} // RR A
void CPU::op_CB20(){
	opskip=2;
	mcycle=2;
	sla(b);
} // SLA B
void CPU::op_CB21(){
	opskip=2;
	mcycle=2;
	sla(c);
} // SLA C
void CPU::op_CB22(){
	opskip=2;
	mcycle=2;
	sla(d);
} // SLA D
void CPU::op_CB23(){
	opskip=2;
	mcycle=2;
	sla(e);
} // SLA E
void CPU::op_CB24(){
	opskip=2;
	mcycle=2;
	sla(h);
} // SLA H
void CPU::op_CB25(){
	opskip=2;
	mcycle=2;
	sla(l);
} // SLA L
void CPU::op_CB26(){
	opskip=2;
	mcycle=4;
	sla_hl();
} // SLA [HL]
void CPU::op_CB27(){
	opskip=2;
	mcycle=2;
	sla(a);
} // SLA A
void CPU::op_CB28(){
	opskip=2;
	mcycle=2;
	sra(b);
} // SRA B
void CPU::op_CB29(){
	opskip=2;
	mcycle=2;
	sra(c);
} // SRA C
void CPU::op_CB2A(){
	opskip=2;
	mcycle=2;
	sra(d);
} // SRA D
void CPU::op_CB2B(){
	opskip=2;
	mcycle=2;
	sra(e);
} // SRA E
void CPU::op_CB2C(){
	opskip=2;
	mcycle=2;
	sra(h);
} // SRA H
void CPU::op_CB2D(){
	opskip=2;
	mcycle=2;
	sra(l);
} // SRA L
void CPU::op_CB2E(){
	opskip=2;
	mcycle=4;
	sra_hl();
} // SRA [HL]
void CPU::op_CB2F(){
	opskip=2;
	mcycle=2;
	sra(a);
} // SRA A
void CPU::op_CB30(){
	opskip=2;
	mcycle=2;
	swap(b);
} // SWAP B
void CPU::op_CB31(){
	opskip=2;
	mcycle=2;
	swap(c);
} // SWAP C
void CPU::op_CB32(){
	opskip=2;
	mcycle=2;
	swap(d);
} // SWAP D
void CPU::op_CB33(){
	opskip=2;
	mcycle=2;
	swap(e);
} // SWAP E
void CPU::op_CB34(){
	opskip=2;
	mcycle=2;
	swap(h);
} // SWAP H
void CPU::op_CB35(){
	opskip=2;
	mcycle=2;
	swap(l);
} // SWAP L
void CPU::op_CB36(){
	opskip=2;
	mcycle=4;
	swap_hl();
} // SWAP [HL]
void CPU::op_CB37(){
	opskip=2;
	mcycle=2;
	swap(a);
} // SWAP A
void CPU::op_CB38(){
	opskip=2;
	mcycle=2;
	srl(b);
} // SRL B
void CPU::op_CB39(){
	opskip=2;
	mcycle=2;
	srl(c);
} // SRL C
void CPU::op_CB3A(){
	opskip=2;
	mcycle=2;
	srl(d);
} // SRL D
void CPU::op_CB3B(){
	opskip=2;
	mcycle=2;
	srl(e);
} // SRL E
void CPU::op_CB3C(){
	opskip=2;
	mcycle=2;
	srl(h);
} // SRL H
void CPU::op_CB3D(){
	opskip=2;
	mcycle=2;
	srl(l);
} // SRL L
void CPU::op_CB3E(){
	opskip=2;
	mcycle=4;
	srl_hl();
} // SRL [HL]
void CPU::op_CB3F(){
	opskip=2;
	mcycle=2;
	srl(a);
} // SRL A
void CPU::op_CB40(){
	opskip=2;
	mcycle=2;
	bit(0, b);
} // BIT 0 B
void CPU::op_CB41(){
	opskip=2;
	mcycle=2;
	bit(0, c);
} // BIT 0 C
void CPU::op_CB42(){
	opskip=2;
	mcycle=2;
	bit(0, d);
} // BIT 0 D
void CPU::op_CB43(){
	opskip=2;
	mcycle=2;
	bit(0, e);
} // BIT 0 E
void CPU::op_CB44(){
	opskip=2;
	mcycle=2;
	bit(0, h);
} // BIT 0 H
void CPU::op_CB45(){
	opskip=2;
	mcycle=2;
	bit(0, l);
} // BIT 0 L
void CPU::op_CB46(){
	opskip=2;
	mcycle=3;
	bit(0, bus.read(h<<8|l));
} // BIT 0 [HL]
void CPU::op_CB47(){
	opskip=2;
	mcycle=2;
	bit(0, a);
} // BIT 0 A
void CPU::op_CB48(){
	opskip=2;
	mcycle=2;
	bit(1, b);
} // BIT 1 B
void CPU::op_CB49(){
	opskip=2;
	mcycle=2;
	bit(1, c);
} // BIT 1 C
void CPU::op_CB4A(){
	opskip=2;
	mcycle=2;
	bit(1, d);
} // BIT 1 D
void CPU::op_CB4B(){
	opskip=2;
	mcycle=2;
	bit(1, e);
} // BIT 1 E
void CPU::op_CB4C(){
	opskip=2;
	mcycle=2;
	bit(1, h);
} // BIT 1 H
void CPU::op_CB4D(){
	opskip=2;
	mcycle=2;
	bit(1, l);
} // BIT 1 L
void CPU::op_CB4E(){
	opskip=2;
	mcycle=3;
	bit(1, bus.read(h<<8|l));
} // BIT 1 [HL]
void CPU::op_CB4F(){
	opskip=2;
	mcycle=2;
	bit(1, a);
} // BIT 1 A
void CPU::op_CB50(){
	opskip=2;
	mcycle=2;
	bit(2, b);
} // BIT 2 B
void CPU::op_CB51(){
	opskip=2;
	mcycle=2;
	bit(2, c);
} // BIT 2 C
void CPU::op_CB52(){
	opskip=2;
	mcycle=2;
	bit(2, d);
} // BIT 2 D
void CPU::op_CB53(){
	opskip=2;
	mcycle=2;
	bit(2, e);
} // BIT 2 E
void CPU::op_CB54(){
	opskip=2;
	mcycle=2;
	bit(2, h);
} // BIT 2 H
void CPU::op_CB55(){
	opskip=2;
	mcycle=2;
	bit(2, l);
} // BIT 2 L
void CPU::op_CB56(){
	opskip=2;
	mcycle=3;
	bit(2, bus.read(h<<8|l));
} // BIT 2 [HL]
void CPU::op_CB57(){
	opskip=2;
	mcycle=2;
	bit(2, a);
} // BIT 2 A
void CPU::op_CB58(){
	opskip=2;
	mcycle=2;
	bit(3, b);
} // BIT 3 B
void CPU::op_CB59(){
	opskip=2;
	mcycle=2;
	bit(3, c);
} // BIT 3 C
void CPU::op_CB5A(){
	opskip=2;
	mcycle=2;
	bit(3, d);
} // BIT 3 D
void CPU::op_CB5B(){
	opskip=2;
	mcycle=2;
	bit(3, e);
} // BIT 3 E
void CPU::op_CB5C(){
	opskip=2;
	mcycle=2;
	bit(3, h);
} // BIT 3 H
void CPU::op_CB5D(){
	opskip=2;
	mcycle=2;
	bit(3, l);
} // BIT 3 L
void CPU::op_CB5E(){
	opskip=2;
	mcycle=3;
	bit(3, bus.read(h<<8|l));
} // BIT 3 [HL]
void CPU::op_CB5F(){
	opskip=2;
	mcycle=2;
	bit(3, a);
} // BIT 3 A
void CPU::op_CB60(){
	opskip=2;
	mcycle=2;
	bit(4, b);
} // BIT 4 B
void CPU::op_CB61(){
	opskip=2;
	mcycle=2;
	bit(4, c);
} // BIT 4 C
void CPU::op_CB62(){
	opskip=2;
	mcycle=2;
	bit(4, d);
} // BIT 4 D
void CPU::op_CB63(){
	opskip=2;
	mcycle=2;
	bit(4, e);
} // BIT 4 E
void CPU::op_CB64(){
	opskip=2;
	mcycle=2;
	bit(4, h);
} // BIT 4 H
void CPU::op_CB65(){
	opskip=2;
	mcycle=2;
	bit(4, l);
} // BIT 4 L
void CPU::op_CB66(){
	opskip=2;
	mcycle=3;
	bit(4, bus.read(h<<8|l));
} // BIT 4 [HL]
void CPU::op_CB67(){
	opskip=2;
	mcycle=2;
	bit(4, a);
} // BIT 4 A
void CPU::op_CB68(){
	opskip=2;
	mcycle=2;
	bit(5, b);
} // BIT 5 B
void CPU::op_CB69(){
	opskip=2;
	mcycle=2;
	bit(5, c);
} // BIT 5 C
void CPU::op_CB6A(){
	opskip=2;
	mcycle=2;
	bit(5, d);
} // BIT 5 D
void CPU::op_CB6B(){
	opskip=2;
	mcycle=2;
	bit(5, e);
} // BIT 5 E
void CPU::op_CB6C(){
	opskip=2;
	mcycle=2;
	bit(5, h);
} // BIT 5 H
void CPU::op_CB6D(){
	opskip=2;
	mcycle=2;
	bit(5, l);
} // BIT 5 L
void CPU::op_CB6E(){
	opskip=2;
	mcycle=3;
	bit(5, bus.read(h<<8|l));
} // BIT 5 [HL]
void CPU::op_CB6F(){
	opskip=2;
	mcycle=2;
	bit(5, a);
} // BIT 5 A
void CPU::op_CB70(){
	opskip=2;
	mcycle=2;
	bit(6, b);
} // BIT 6 B
void CPU::op_CB71(){
	opskip=2;
	mcycle=2;
	bit(6, c);
} // BIT 6 C
void CPU::op_CB72(){
	opskip=2;
	mcycle=2;
	bit(6, d);
} // BIT 6 D
void CPU::op_CB73(){
	opskip=2;
	mcycle=2;
	bit(6, e);
} // BIT 6 E
void CPU::op_CB74(){
	opskip=2;
	mcycle=2;
	bit(6, h);
} // BIT 6 H
void CPU::op_CB75(){
	opskip=2;
	mcycle=2;
	bit(6, l);
} // BIT 6 L
void CPU::op_CB76(){
	opskip=2;
	mcycle=3;
	bit(6, bus.read(h<<8|l));
} // BIT 6 [HL]
void CPU::op_CB77(){
	opskip=2;
	mcycle=2;
	bit(6, a);
} // BIT 6 A
void CPU::op_CB78(){
	opskip=2;
	mcycle=2;
	bit(7, b);
} // BIT 7 B
void CPU::op_CB79(){
	opskip=2;
	mcycle=2;
	bit(7, c);
} // BIT 7 C
void CPU::op_CB7A(){
	opskip=2;
	mcycle=2;
	bit(7, d);
} // BIT 7 D
void CPU::op_CB7B(){
	opskip=2;
	mcycle=2;
	bit(7, e);
} // BIT 7 E
void CPU::op_CB7C(){
	opskip=2;
	mcycle=2;
	bit(7, h);
} // BIT 7 H
void CPU::op_CB7D(){
	opskip=2;
	mcycle=2;
	bit(7, l);
} // BIT 7 L
void CPU::op_CB7E(){
	opskip=2;
	mcycle=3;
	bit(7, bus.read(h<<8|l));
} // BIT 7 [HL]
void CPU::op_CB7F(){
	opskip=2;
	mcycle=2;
	bit(7, a);
} // BIT 7 A
void CPU::op_CB80(){
	opskip=2;
	mcycle=2;
	res(0, b);
} // RES 0 B
void CPU::op_CB81(){
	opskip=2;
	mcycle=2;
	res(0, c);
} // RES 0 C
void CPU::op_CB82(){
	opskip=2;
	mcycle=2;
	res(0, d);
} // RES 0 D
void CPU::op_CB83(){
	opskip=2;
	mcycle=2;
	res(0, e);
} // RES 0 E
void CPU::op_CB84(){
	opskip=2;
	mcycle=2;
	res(0, h);
} // RES 0 H
void CPU::op_CB85(){
	opskip=2;
	mcycle=2;
	res(0, l);
} // RES 0 L
void CPU::op_CB86(){
	opskip=2;
	mcycle=4;
	res_hl(0);
} // RES 0 [HL]
void CPU::op_CB87(){
	opskip=2;
	mcycle=2;
	res(0, a);
} // RES 0 A
void CPU::op_CB88(){
	opskip=2;
	mcycle=2;
	res(1, b);
} // RES 1 B
void CPU::op_CB89(){
	opskip=2;
	mcycle=2;
	res(1, c);
} // RES 1 C
void CPU::op_CB8A(){
	opskip=2;
	mcycle=2;
	res(1, d);
} // RES 1 D
void CPU::op_CB8B(){
	opskip=2;
	mcycle=2;
	res(1, e);
} // RES 1 E
void CPU::op_CB8C(){
	opskip=2;
	mcycle=2;
	res(1, h);
} // RES 1 H
void CPU::op_CB8D(){
	opskip=2;
	mcycle=2;
	res(1, l);
} // RES 1 L
void CPU::op_CB8E(){
	opskip=2;
	mcycle=4;
	res_hl(1);
} // RES 1 [HL]
void CPU::op_CB8F(){
	opskip=2;
	mcycle=2;
	res(1, a);
} // RES 1 A
void CPU::op_CB90(){
	opskip=2;
	mcycle=2;
	res(2, b);
} // RES 2 B
void CPU::op_CB91(){
	opskip=2;
	mcycle=2;
	res(2, c);
} // RES 2 C
void CPU::op_CB92(){
	opskip=2;
	mcycle=2;
	res(2, d);
} // RES 2 D
void CPU::op_CB93(){
	opskip=2;
	mcycle=2;
	res(2, e);
} // RES 2 E
void CPU::op_CB94(){
	opskip=2;
	mcycle=2;
	res(2, h);
} // RES 2 H
void CPU::op_CB95(){
	opskip=2;
	mcycle=2;
	res(2, l);
} // RES 2 L
void CPU::op_CB96(){
	opskip=2;
	mcycle=4;
	res_hl(2);
} // RES 2 [HL]
void CPU::op_CB97(){
	opskip=2;
	mcycle=2;
	res(2, a);
} // RES 2 A
void CPU::op_CB98(){
	opskip=2;
	mcycle=2;
	res(3, b);
} // RES 3 B
void CPU::op_CB99(){
	opskip=2;
	mcycle=2;
	res(3, c);
} // RES 3 C
void CPU::op_CB9A(){
	opskip=2;
	mcycle=2;
	res(3, d);
} // RES 3 D
void CPU::op_CB9B(){
	opskip=2;
	mcycle=2;
	res(3, e);
} // RES 3 E
void CPU::op_CB9C(){
	opskip=2;
	mcycle=2;
	res(3, h);
} // RES 3 H
void CPU::op_CB9D(){
	opskip=2;
	mcycle=2;
	res(3, l);
} // RES 3 L
void CPU::op_CB9E(){
	opskip=2;
	mcycle=4;
	res_hl(3);
} // RES 3 [HL]
void CPU::op_CB9F(){
	opskip=2;
	mcycle=2;
	res(3, a);
} // RES 3 A
void CPU::op_CBA0(){
	opskip=2;
	mcycle=2;
	res(4, b);
} // RES 4 B
void CPU::op_CBA1(){
	opskip=2;
	mcycle=2;
	res(4, c);
} // RES 4 C
void CPU::op_CBA2(){
	opskip=2;
	mcycle=2;
	res(4, d);
} // RES 4 D
void CPU::op_CBA3(){
	opskip=2;
	mcycle=2;
	res(4, e);
} // RES 4 E
void CPU::op_CBA4(){
	opskip=2;
	mcycle=2;
	res(4, h);
} // RES 4 H
void CPU::op_CBA5(){
	opskip=2;
	mcycle=2;
	res(4, l);
} // RES 4 L
void CPU::op_CBA6(){
	opskip=2;
	mcycle=4;
	res_hl(4);
} // RES 4 [HL]
void CPU::op_CBA7(){
	opskip=2;
	mcycle=2;
	res(4, a);
} // RES 4 A
void CPU::op_CBA8(){
	opskip=2;
	mcycle=2;
	res(5, b);
} // RES 5 B
void CPU::op_CBA9(){
	opskip=2;
	mcycle=2;
	res(5, c);
} // RES 5 C
void CPU::op_CBAA(){
	opskip=2;
	mcycle=2;
	res(5, d);
} // RES 5 D
void CPU::op_CBAB(){
	opskip=2;
	mcycle=2;
	res(5, e);
} // RES 5 E
void CPU::op_CBAC(){
	opskip=2;
	mcycle=2;
	res(5, h);
} // RES 5 H
void CPU::op_CBAD(){
	opskip=2;
	mcycle=2;
	res(5, l);
} // RES 5 L
void CPU::op_CBAE(){
	opskip=2;
	mcycle=4;
	res_hl(5);
} // RES 5 [HL]
void CPU::op_CBAF(){
	opskip=2;
	mcycle=2;
	res(5, a);
} // RES 5 A
void CPU::op_CBB0(){
	opskip=2;
	mcycle=2;
	res(6, b);
} // RES 6 B
void CPU::op_CBB1(){
	opskip=2;
	mcycle=2;
	res(6, c);
} // RES 6 C
void CPU::op_CBB2(){
	opskip=2;
	mcycle=2;
	res(6, d);
} // RES 6 D
void CPU::op_CBB3(){
	opskip=2;
	mcycle=2;
	res(6, e);
} // RES 6 E
void CPU::op_CBB4(){
	opskip=2;
	mcycle=2;
	res(6, h);
} // RES 6 H
void CPU::op_CBB5(){
	opskip=2;
	mcycle=2;
	res(6, l);
} // RES 6 L
void CPU::op_CBB6(){
	opskip=2;
	mcycle=4;
	res_hl(6);
} // RES 6 [HL]
void CPU::op_CBB7(){
	opskip=2;
	mcycle=2;
	res(6, a);
} // RES 6 A
void CPU::op_CBB8(){
	opskip=2;
	mcycle=2;
	res(7, b);
} // RES 7 B
void CPU::op_CBB9(){
	opskip=2;
	mcycle=2;
	res(7, c);
} // RES 7 C
void CPU::op_CBBA(){
	opskip=2;
	mcycle=2;
	res(7, d);
} // RES 7 D
void CPU::op_CBBB(){
	opskip=2;
	mcycle=2;
	res(7, e);
} // RES 7 E
void CPU::op_CBBC(){
	opskip=2;
	mcycle=2;
	res(7, h);
} // RES 7 H
void CPU::op_CBBD(){
	opskip=2;
	mcycle=2;
	res(7, l);
} // RES 7 L
void CPU::op_CBBE(){
	opskip=2;
	mcycle=4;
	res_hl(7);
} // RES 7 [HL]
void CPU::op_CBBF(){
	opskip=2;
	mcycle=2;
	res(7, a);
} // RES 7 A
void CPU::op_CBC0(){
	opskip=2;
	mcycle=2;
	set(0, b);
} // SET 0 B
void CPU::op_CBC1(){
	opskip=2;
	mcycle=2;
	set(0, c);
} // SET 0 C
void CPU::op_CBC2(){
	opskip=2;
	mcycle=2;
	set(0, d);
} // SET 0 D
void CPU::op_CBC3(){
	opskip=2;
	mcycle=2;
	set(0, e);
} // SET 0 E
void CPU::op_CBC4(){
	opskip=2;
	mcycle=2;
	set(0, h);
} // SET 0 H
void CPU::op_CBC5(){
	opskip=2;
	mcycle=2;
	set(0, l);
} // SET 0 L
void CPU::op_CBC6(){
	opskip=2;
	mcycle=4;
	set_hl(0);
} // SET 0 [HL]
void CPU::op_CBC7(){
	opskip=2;
	mcycle=2;
	set(0, a);
} // SET 0 A
void CPU::op_CBC8(){
	opskip=2;
	mcycle=2;
	set(1, b);
} // SET 1 B
void CPU::op_CBC9(){
	opskip=2;
	mcycle=2;
	set(1, c);
} // SET 1 C
void CPU::op_CBCA(){
	opskip=2;
	mcycle=2;
	set(1, d);
} // SET 1 D
void CPU::op_CBCB(){
	opskip=2;
	mcycle=2;
	set(1, e);
} // SET 1 E
void CPU::op_CBCC(){
	opskip=2;
	mcycle=2;
	set(1, h);
} // SET 1 H
void CPU::op_CBCD(){
	opskip=2;
	mcycle=2;
	set(1, l);
} // SET 1 L
void CPU::op_CBCE(){
	opskip=2;
	mcycle=4;
	set_hl(1);
} // SET 1 [HL]
void CPU::op_CBCF(){
	opskip=2;
	mcycle=2;
	set(1, a);
} // SET 1 A
void CPU::op_CBD0(){
	opskip=2;
	mcycle=2;
	set(2, b);
} // SET 2 B
void CPU::op_CBD1(){
	opskip=2;
	mcycle=2;
	set(2, c);
} // SET 2 C
void CPU::op_CBD2(){
	opskip=2;
	mcycle=2;
	set(2, d);
} // SET 2 D
void CPU::op_CBD3(){
	opskip=2;
	mcycle=2;
	set(2, e);
} // SET 2 E
void CPU::op_CBD4(){
	opskip=2;
	mcycle=2;
	set(2, h);
} // SET 2 H
void CPU::op_CBD5(){
	opskip=2;
	mcycle=2;
	set(2, l);
} // SET 2 L
void CPU::op_CBD6(){
	opskip=2;
	mcycle=4;
	set_hl(2);
} // SET 2 [HL]
void CPU::op_CBD7(){
	opskip=2;
	mcycle=2;
	set(2, a);
} // SET 2 A
void CPU::op_CBD8(){
	opskip=2;
	mcycle=2;
	set(3, b);
} // SET 3 B
void CPU::op_CBD9(){
	opskip=2;
	mcycle=2;
	set(3, c);
} // SET 3 C
void CPU::op_CBDA(){
	opskip=2;
	mcycle=2;
	set(3, d);
} // SET 3 D
void CPU::op_CBDB(){
	opskip=2;
	mcycle=2;
	set(3, e);
} // SET 3 E
void CPU::op_CBDC(){
	opskip=2;
	mcycle=2;
	set(3, h);
} // SET 3 H
void CPU::op_CBDD(){
	opskip=2;
	mcycle=2;
	set(3, l);
} // SET 3 L
void CPU::op_CBDE(){
	opskip=2;
	mcycle=4;
	set_hl(3);
} // SET 3 [HL]
void CPU::op_CBDF(){
	opskip=2;
	mcycle=2;
	set(3, a);
} // SET 3 A
void CPU::op_CBE0(){
	opskip=2;
	mcycle=2;
	set(4, b);
} // SET 4 B
void CPU::op_CBE1(){
	opskip=2;
	mcycle=2;
	set(4, c);
} // SET 4 C
void CPU::op_CBE2(){
	opskip=2;
	mcycle=2;
	set(4, d);
} // SET 4 D
void CPU::op_CBE3(){
	opskip=2;
	mcycle=2;
	set(4, e);
} // SET 4 E
void CPU::op_CBE4(){
	opskip=2;
	mcycle=2;
	set(4, h);
} // SET 4 H
void CPU::op_CBE5(){
	opskip=2;
	mcycle=2;
	set(4, l);
} // SET 4 L
void CPU::op_CBE6(){
	opskip=2;
	mcycle=4;
	set_hl(4);
} // SET 4 [HL]
void CPU::op_CBE7(){
	opskip=2;
	mcycle=2;
	set(4, a);
} // SET 4 A
void CPU::op_CBE8(){
	opskip=2;
	mcycle=2;
	set(5, b);
} // SET 5 B
void CPU::op_CBE9(){
	opskip=2;
	mcycle=2;
	set(5, c);
} // SET 5 C
void CPU::op_CBEA(){
	opskip=2;
	mcycle=2;
	set(5, d);
} // SET 5 D
void CPU::op_CBEB(){
	opskip=2;
	mcycle=2;
	set(5, e);
} // SET 5 E
void CPU::op_CBEC(){
	opskip=2;
	mcycle=2;
	set(5, h);
} // SET 5 H
void CPU::op_CBED(){
	opskip=2;
	mcycle=2;
	set(5, l);
} // SET 5 L
void CPU::op_CBEE(){
	opskip=2;
	mcycle=4;
	set_hl(5);
} // SET 5 [HL]
void CPU::op_CBEF(){
	opskip=2;
	mcycle=2;
	set(5, a);
} // SET 5 A
void CPU::op_CBF0(){
	opskip=2;
	mcycle=2;
	set(6, b);
} // SET 6 B
void CPU::op_CBF1(){
	opskip=2;
	mcycle=2;
	set(6, c);
} // SET 6 C
void CPU::op_CBF2(){
	opskip=2;
	mcycle=2;
	set(6, d);
} // SET 6 D
void CPU::op_CBF3(){
	opskip=2;
	mcycle=2;
	set(6, e);
} // SET 6 E
void CPU::op_CBF4(){
	opskip=2;
	mcycle=2;
	set(6, h);
} // SET 6 H
void CPU::op_CBF5(){
	opskip=2;
	mcycle=2;
	set(6, l);
} // SET 6 L
void CPU::op_CBF6(){
	opskip=2;
	mcycle=4;
	set_hl(6);
} // SET 6 [HL]
void CPU::op_CBF7(){
	opskip=2;
	mcycle=2;
	set(6, a);
} // SET 6 A
void CPU::op_CBF8(){
	opskip=2;
	mcycle=2;
	set(7, b);
} // SET 7 B
void CPU::op_CBF9(){
	opskip=2;
	mcycle=2;
	set(7, c);
} // SET 7 C
void CPU::op_CBFA(){
	opskip=2;
	mcycle=2;
	set(7, d);
} // SET 7 D
void CPU::op_CBFB(){
	opskip=2;
	mcycle=2;
	set(7, e);
} // SET 7 E
void CPU::op_CBFC(){
	opskip=2;
	mcycle=2;
	set(7, h);
} // SET 7 H
void CPU::op_CBFD(){
	opskip=2;
	mcycle=2;
	set(7, l);
} // SET 7 L
void CPU::op_CBFE(){
	opskip=2;
	mcycle=4;
	set_hl(7);
} // SET 7 [HL]
void CPU::op_CBFF(){
	opskip=2;
	mcycle=2;
	set(7, a);
} // SET 7 A
