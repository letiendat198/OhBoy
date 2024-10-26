#ifndef CPU_H
#define CPU_H

#include <string>

#include "logger.h"
#include "memory.h"
#include "types.h"

class Cpu{
public:
    unsigned char a=0;
    unsigned char b=0;
    unsigned char c=0;
    unsigned char d=0;
    unsigned char e=0;
    unsigned char h=0;
    unsigned char l=0;
    unsigned char z_flag=0;
    unsigned char n_flag=0;
    unsigned char h_flag=0;
    unsigned char c_flag=0;
    unsigned short pc=0x0100;
    unsigned short sp=0;
    unsigned char ime=0;
    unsigned char halt = 0;

    bool exec_flag = false;
    bool ime_next = false;
    u_char interrupt_addr;

    Logger logger;

    unsigned char opskip;
    unsigned char mcycle;
    unsigned char cycle_count=0;
    void init();
    void init_noboot();
    bool tick();

    std::string craft_debug();

    void write8_mem(u_short addr, u_char a);
    u_char read8_mem(u_short addr);
    u_short read16_mem(u_short addr);

    void ld8_imm(u_char &a, u_char imm);
    void ld16_imm(u_char &a, u_char &b, u_short imm);

    void inc8(u_char &a);
    void inc16(u_char &a, u_char &b);
    void inc_sp();
    void inc_ind_hl();

    void dec8(u_char &a);
    void dec16(u_char &a, u_char &b);
    void dec_sp();
    void dec_ind_hl();

    void add8(u_char &a, u_char b);
    void add_hl(u_short imm);
    void add_sp(u_char imm);
    void adc8(u_char &a, u_char b);

    void sub8(u_char &a, u_char b);
    void sbc8(u_char &a, u_char b);

    void and8(u_char &a, u_char b);
    void or8(u_char &a, u_char b);
    void xor8(u_char &a, u_char b);
    void cp8(u_char a, u_char b);

    void jr(u_char e);
    void jrc(u_char flag, u_char e);

    void jp(u_short e);
    void jpc(u_char flag, u_short e);

    void call();
    void callc(u_char flag);

    void ret();
    void retc(u_char flag);

    void rst(u_char n);

    void pop(u_char &a, u_char &b);
    void push(u_char a, u_char b);

    void rlc(u_char &a);
    void rlc_hl();

    void rrc(u_char &a);
    void rrc_hl();

    void rl(u_char &a);
    void rl_hl();

    void rr(u_char &a);
    void rr_hl();

    void sla(u_char &a);
    void sla_hl();

    void sra(u_char &a);
    void sra_hl();

    void swap(u_char &a);
    void swap_hl();

    void srl(u_char &a);
    void srl_hl();

    void bit(u_char i, u_char a);

    void set(u_char i, u_char &a);
    void set_hl(u_char i);

    void res(u_char i, u_char &a);
    void res_hl(u_char i);

    void op_00(); void op_01(); void op_02(); void op_03(); void op_04(); void op_05(); void op_06(); void op_07(); void op_08(); void op_09(); void op_0A(); void op_0B(); void op_0C(); void op_0D(); void op_0E(); void op_0F();
    void op_10(); void op_11(); void op_12(); void op_13(); void op_14(); void op_15(); void op_16(); void op_17(); void op_18(); void op_19(); void op_1A(); void op_1B(); void op_1C(); void op_1D(); void op_1E(); void op_1F();
    void op_20(); void op_21(); void op_22(); void op_23(); void op_24(); void op_25(); void op_26(); void op_27(); void op_28(); void op_29(); void op_2A(); void op_2B(); void op_2C(); void op_2D(); void op_2E(); void op_2F();
    void op_30(); void op_31(); void op_32(); void op_33(); void op_34(); void op_35(); void op_36(); void op_37(); void op_38(); void op_39(); void op_3A(); void op_3B(); void op_3C(); void op_3D(); void op_3E(); void op_3F();
    void op_40(); void op_41(); void op_42(); void op_43(); void op_44(); void op_45(); void op_46(); void op_47(); void op_48(); void op_49(); void op_4A(); void op_4B(); void op_4C(); void op_4D(); void op_4E(); void op_4F();
    void op_50(); void op_51(); void op_52(); void op_53(); void op_54(); void op_55(); void op_56(); void op_57(); void op_58(); void op_59(); void op_5A(); void op_5B(); void op_5C(); void op_5D(); void op_5E(); void op_5F();
    void op_60(); void op_61(); void op_62(); void op_63(); void op_64(); void op_65(); void op_66(); void op_67(); void op_68(); void op_69(); void op_6A(); void op_6B(); void op_6C(); void op_6D(); void op_6E(); void op_6F();
    void op_70(); void op_71(); void op_72(); void op_73(); void op_74(); void op_75(); void op_76(); void op_77(); void op_78(); void op_79(); void op_7A(); void op_7B(); void op_7C(); void op_7D(); void op_7E(); void op_7F();
    void op_80(); void op_81(); void op_82(); void op_83(); void op_84(); void op_85(); void op_86(); void op_87(); void op_88(); void op_89(); void op_8A(); void op_8B(); void op_8C(); void op_8D(); void op_8E(); void op_8F();
    void op_90(); void op_91(); void op_92(); void op_93(); void op_94(); void op_95(); void op_96(); void op_97(); void op_98(); void op_99(); void op_9A(); void op_9B(); void op_9C(); void op_9D(); void op_9E(); void op_9F();
    void op_A0(); void op_A1(); void op_A2(); void op_A3(); void op_A4(); void op_A5(); void op_A6(); void op_A7(); void op_A8(); void op_A9(); void op_AA(); void op_AB(); void op_AC(); void op_AD(); void op_AE(); void op_AF();
    void op_B0(); void op_B1(); void op_B2(); void op_B3(); void op_B4(); void op_B5(); void op_B6(); void op_B7(); void op_B8(); void op_B9(); void op_BA(); void op_BB(); void op_BC(); void op_BD(); void op_BE(); void op_BF();
    void op_C0(); void op_C1(); void op_C2(); void op_C3(); void op_C4(); void op_C5(); void op_C6(); void op_C7(); void op_C8(); void op_C9(); void op_CA(); void op_CB(); void op_CC(); void op_CD(); void op_CE(); void op_CF();
    void op_D0(); void op_D1(); void op_D2(); void op_D3(); void op_D4(); void op_D5(); void op_D6(); void op_D7(); void op_D8(); void op_D9(); void op_DA(); void op_DB(); void op_DC(); void op_DD(); void op_DE(); void op_DF();
    void op_E0(); void op_E1(); void op_E2(); void op_E3(); void op_E4(); void op_E5(); void op_E6(); void op_E7(); void op_E8(); void op_E9(); void op_EA(); void op_EB(); void op_EC(); void op_ED(); void op_EE(); void op_EF();
    void op_F0(); void op_F1(); void op_F2(); void op_F3(); void op_F4(); void op_F5(); void op_F6(); void op_F7(); void op_F8(); void op_F9(); void op_FA(); void op_FB(); void op_FC(); void op_FD(); void op_FE(); void op_FF();

    void op_CB00(); void op_CB01(); void op_CB02(); void op_CB03(); void op_CB04(); void op_CB05(); void op_CB06(); void op_CB07(); void op_CB08(); void op_CB09(); void op_CB0A(); void op_CB0B(); void op_CB0C(); void op_CB0D(); void op_CB0E(); void op_CB0F();
    void op_CB10(); void op_CB11(); void op_CB12(); void op_CB13(); void op_CB14(); void op_CB15(); void op_CB16(); void op_CB17(); void op_CB18(); void op_CB19(); void op_CB1A(); void op_CB1B(); void op_CB1C(); void op_CB1D(); void op_CB1E(); void op_CB1F();
    void op_CB20(); void op_CB21(); void op_CB22(); void op_CB23(); void op_CB24(); void op_CB25(); void op_CB26(); void op_CB27(); void op_CB28(); void op_CB29(); void op_CB2A(); void op_CB2B(); void op_CB2C(); void op_CB2D(); void op_CB2E(); void op_CB2F();
    void op_CB30(); void op_CB31(); void op_CB32(); void op_CB33(); void op_CB34(); void op_CB35(); void op_CB36(); void op_CB37(); void op_CB38(); void op_CB39(); void op_CB3A(); void op_CB3B(); void op_CB3C(); void op_CB3D(); void op_CB3E(); void op_CB3F();
    void op_CB40(); void op_CB41(); void op_CB42(); void op_CB43(); void op_CB44(); void op_CB45(); void op_CB46(); void op_CB47(); void op_CB48(); void op_CB49(); void op_CB4A(); void op_CB4B(); void op_CB4C(); void op_CB4D(); void op_CB4E(); void op_CB4F();
    void op_CB50(); void op_CB51(); void op_CB52(); void op_CB53(); void op_CB54(); void op_CB55(); void op_CB56(); void op_CB57(); void op_CB58(); void op_CB59(); void op_CB5A(); void op_CB5B(); void op_CB5C(); void op_CB5D(); void op_CB5E(); void op_CB5F();
    void op_CB60(); void op_CB61(); void op_CB62(); void op_CB63(); void op_CB64(); void op_CB65(); void op_CB66(); void op_CB67(); void op_CB68(); void op_CB69(); void op_CB6A(); void op_CB6B(); void op_CB6C(); void op_CB6D(); void op_CB6E(); void op_CB6F();
    void op_CB70(); void op_CB71(); void op_CB72(); void op_CB73(); void op_CB74(); void op_CB75(); void op_CB76(); void op_CB77(); void op_CB78(); void op_CB79(); void op_CB7A(); void op_CB7B(); void op_CB7C(); void op_CB7D(); void op_CB7E(); void op_CB7F();
    void op_CB80(); void op_CB81(); void op_CB82(); void op_CB83(); void op_CB84(); void op_CB85(); void op_CB86(); void op_CB87(); void op_CB88(); void op_CB89(); void op_CB8A(); void op_CB8B(); void op_CB8C(); void op_CB8D(); void op_CB8E(); void op_CB8F();
    void op_CB90(); void op_CB91(); void op_CB92(); void op_CB93(); void op_CB94(); void op_CB95(); void op_CB96(); void op_CB97(); void op_CB98(); void op_CB99(); void op_CB9A(); void op_CB9B(); void op_CB9C(); void op_CB9D(); void op_CB9E(); void op_CB9F();
    void op_CBA0(); void op_CBA1(); void op_CBA2(); void op_CBA3(); void op_CBA4(); void op_CBA5(); void op_CBA6(); void op_CBA7(); void op_CBA8(); void op_CBA9(); void op_CBAA(); void op_CBAB(); void op_CBAC(); void op_CBAD(); void op_CBAE(); void op_CBAF();
    void op_CBB0(); void op_CBB1(); void op_CBB2(); void op_CBB3(); void op_CBB4(); void op_CBB5(); void op_CBB6(); void op_CBB7(); void op_CBB8(); void op_CBB9(); void op_CBBA(); void op_CBBB(); void op_CBBC(); void op_CBBD(); void op_CBBE(); void op_CBBF();
    void op_CBC0(); void op_CBC1(); void op_CBC2(); void op_CBC3(); void op_CBC4(); void op_CBC5(); void op_CBC6(); void op_CBC7(); void op_CBC8(); void op_CBC9(); void op_CBCA(); void op_CBCB(); void op_CBCC(); void op_CBCD(); void op_CBCE(); void op_CBCF();
    void op_CBD0(); void op_CBD1(); void op_CBD2(); void op_CBD3(); void op_CBD4(); void op_CBD5(); void op_CBD6(); void op_CBD7(); void op_CBD8(); void op_CBD9(); void op_CBDA(); void op_CBDB(); void op_CBDC(); void op_CBDD(); void op_CBDE(); void op_CBDF();
    void op_CBE0(); void op_CBE1(); void op_CBE2(); void op_CBE3(); void op_CBE4(); void op_CBE5(); void op_CBE6(); void op_CBE7(); void op_CBE8(); void op_CBE9(); void op_CBEA(); void op_CBEB(); void op_CBEC(); void op_CBED(); void op_CBEE(); void op_CBEF();
    void op_CBF0(); void op_CBF1(); void op_CBF2(); void op_CBF3(); void op_CBF4(); void op_CBF5(); void op_CBF6(); void op_CBF7(); void op_CBF8(); void op_CBF9(); void op_CBFA(); void op_CBFB(); void op_CBFC(); void op_CBFD(); void op_CBFE(); void op_CBFF();

    void (Cpu::*jump_table[0xFF+1])() =
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

    void (Cpu::*jump_table_prefixed[0xFF+1])() =
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
};

#endif