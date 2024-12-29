#ifndef CPU_H
#define CPU_H

#include <string>

#include "logger.h"
#include "memory.h"
#include "types.h"

class CPU{
private:
    Logger logger = Logger("CPU");
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
    unsigned short pc=0;
    unsigned short sp=0;
    unsigned char ime=0;
    unsigned char halt=0;

    inline static bool double_spd_mode = false;

    bool exec_flag = true;
    bool ime_next = false;

    unsigned char opskip;
    unsigned char mcycle;
    uint8_t tick();
    bool handle_interrupts();

    void log_cpu();

    void write8_mem(uint16_t addr, uint8_t a);
    uint8_t read8_mem(uint16_t addr);
    uint16_t read16_mem(uint16_t addr);

    void ld8_imm(uint8_t &a, uint8_t imm);
    void ld16_imm(uint8_t &a, uint8_t &b, uint16_t imm);

    void inc8(uint8_t &a);
    void inc16(uint8_t &a, uint8_t &b);
    void inc_sp();
    void inc_ind_hl();

    void dec8(uint8_t &a);
    void dec16(uint8_t &a, uint8_t &b);
    void dec_sp();
    void dec_ind_hl();

    void add8(uint8_t &a, uint8_t b);
    void add_hl(uint16_t imm);
    void add_sp(uint8_t imm);
    void adc8(uint8_t &a, uint8_t b);

    void sub8(uint8_t &a, uint8_t b);
    void sbc8(uint8_t &a, uint8_t b);

    void and8(uint8_t &a, uint8_t b);
    void or8(uint8_t &a, uint8_t b);
    void xor8(uint8_t &a, uint8_t b);
    void cp8(uint8_t a, uint8_t b);

    void jr(uint8_t e);
    void jrc(uint8_t flag, uint8_t e);

    void jp(uint16_t e);
    void jpc(uint8_t flag, uint16_t e);

    void call();
    void callc(uint8_t flag);

    void ret();
    void retc(uint8_t flag);

    void rst(uint8_t n);

    void pop(uint8_t &a, uint8_t &b);
    void push(uint8_t a, uint8_t b);

    void rlc(uint8_t &a);
    void rlc_hl();

    void rrc(uint8_t &a);
    void rrc_hl();

    void rl(uint8_t &a);
    void rl_hl();

    void rr(uint8_t &a);
    void rr_hl();

    void sla(uint8_t &a);
    void sla_hl();

    void sra(uint8_t &a);
    void sra_hl();

    void swap(uint8_t &a);
    void swap_hl();

    void srl(uint8_t &a);
    void srl_hl();

    void bit(uint8_t i, uint8_t a);

    void set(uint8_t i, uint8_t &a);
    void set_hl(uint8_t i);

    void res(uint8_t i, uint8_t &a);
    void res_hl(uint8_t i);

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

    void (CPU::*jump_table[0xFF+1])() =
        {CPU::op_00, CPU::op_01, CPU::op_02, CPU::op_03, CPU::op_04, CPU::op_05, CPU::op_06, CPU::op_07, CPU::op_08, CPU::op_09, CPU::op_0A, CPU::op_0B, CPU::op_0C, CPU::op_0D, CPU::op_0E, CPU::op_0F,
        CPU::op_10, CPU::op_11, CPU::op_12, CPU::op_13, CPU::op_14, CPU::op_15, CPU::op_16, CPU::op_17, CPU::op_18, CPU::op_19, CPU::op_1A, CPU::op_1B, CPU::op_1C, CPU::op_1D, CPU::op_1E, CPU::op_1F,
        CPU::op_20, CPU::op_21, CPU::op_22, CPU::op_23, CPU::op_24, CPU::op_25, CPU::op_26, CPU::op_27, CPU::op_28, CPU::op_29, CPU::op_2A, CPU::op_2B, CPU::op_2C, CPU::op_2D, CPU::op_2E, CPU::op_2F,
        CPU::op_30, CPU::op_31, CPU::op_32, CPU::op_33, CPU::op_34, CPU::op_35, CPU::op_36, CPU::op_37, CPU::op_38, CPU::op_39, CPU::op_3A, CPU::op_3B, CPU::op_3C, CPU::op_3D, CPU::op_3E, CPU::op_3F,
        CPU::op_40, CPU::op_41, CPU::op_42, CPU::op_43, CPU::op_44, CPU::op_45, CPU::op_46, CPU::op_47, CPU::op_48, CPU::op_49, CPU::op_4A, CPU::op_4B, CPU::op_4C, CPU::op_4D, CPU::op_4E, CPU::op_4F,
        CPU::op_50, CPU::op_51, CPU::op_52, CPU::op_53, CPU::op_54, CPU::op_55, CPU::op_56, CPU::op_57, CPU::op_58, CPU::op_59, CPU::op_5A, CPU::op_5B, CPU::op_5C, CPU::op_5D, CPU::op_5E, CPU::op_5F,
        CPU::op_60, CPU::op_61, CPU::op_62, CPU::op_63, CPU::op_64, CPU::op_65, CPU::op_66, CPU::op_67, CPU::op_68, CPU::op_69, CPU::op_6A, CPU::op_6B, CPU::op_6C, CPU::op_6D, CPU::op_6E, CPU::op_6F,
        CPU::op_70, CPU::op_71, CPU::op_72, CPU::op_73, CPU::op_74, CPU::op_75, CPU::op_76, CPU::op_77, CPU::op_78, CPU::op_79, CPU::op_7A, CPU::op_7B, CPU::op_7C, CPU::op_7D, CPU::op_7E, CPU::op_7F,
        CPU::op_80, CPU::op_81, CPU::op_82, CPU::op_83, CPU::op_84, CPU::op_85, CPU::op_86, CPU::op_87, CPU::op_88, CPU::op_89, CPU::op_8A, CPU::op_8B, CPU::op_8C, CPU::op_8D, CPU::op_8E, CPU::op_8F,
        CPU::op_90, CPU::op_91, CPU::op_92, CPU::op_93, CPU::op_94, CPU::op_95, CPU::op_96, CPU::op_97, CPU::op_98, CPU::op_99, CPU::op_9A, CPU::op_9B, CPU::op_9C, CPU::op_9D, CPU::op_9E, CPU::op_9F,
        CPU::op_A0, CPU::op_A1, CPU::op_A2, CPU::op_A3, CPU::op_A4, CPU::op_A5, CPU::op_A6, CPU::op_A7, CPU::op_A8, CPU::op_A9, CPU::op_AA, CPU::op_AB, CPU::op_AC, CPU::op_AD, CPU::op_AE, CPU::op_AF,
        CPU::op_B0, CPU::op_B1, CPU::op_B2, CPU::op_B3, CPU::op_B4, CPU::op_B5, CPU::op_B6, CPU::op_B7, CPU::op_B8, CPU::op_B9, CPU::op_BA, CPU::op_BB, CPU::op_BC, CPU::op_BD, CPU::op_BE, CPU::op_BF,
        CPU::op_C0, CPU::op_C1, CPU::op_C2, CPU::op_C3, CPU::op_C4, CPU::op_C5, CPU::op_C6, CPU::op_C7, CPU::op_C8, CPU::op_C9, CPU::op_CA, CPU::op_CB, CPU::op_CC, CPU::op_CD, CPU::op_CE, CPU::op_CF,
        CPU::op_D0, CPU::op_D1, CPU::op_D2, CPU::op_D3, CPU::op_D4, CPU::op_D5, CPU::op_D6, CPU::op_D7, CPU::op_D8, CPU::op_D9, CPU::op_DA, CPU::op_DB, CPU::op_DC, CPU::op_DD, CPU::op_DE, CPU::op_DF,
        CPU::op_E0, CPU::op_E1, CPU::op_E2, CPU::op_E3, CPU::op_E4, CPU::op_E5, CPU::op_E6, CPU::op_E7, CPU::op_E8, CPU::op_E9, CPU::op_EA, CPU::op_EB, CPU::op_EC, CPU::op_ED, CPU::op_EE, CPU::op_EF,
        CPU::op_F0, CPU::op_F1, CPU::op_F2, CPU::op_F3, CPU::op_F4, CPU::op_F5, CPU::op_F6, CPU::op_F7, CPU::op_F8, CPU::op_F9, CPU::op_FA, CPU::op_FB, CPU::op_FC, CPU::op_FD, CPU::op_FE, CPU::op_FF};

    void (CPU::*jump_table_prefixed[0xFF+1])() =
        {CPU::op_CB00, CPU::op_CB01, CPU::op_CB02, CPU::op_CB03, CPU::op_CB04, CPU::op_CB05, CPU::op_CB06, CPU::op_CB07, CPU::op_CB08, CPU::op_CB09, CPU::op_CB0A, CPU::op_CB0B, CPU::op_CB0C, CPU::op_CB0D, CPU::op_CB0E, CPU::op_CB0F,
        CPU::op_CB10, CPU::op_CB11, CPU::op_CB12, CPU::op_CB13, CPU::op_CB14, CPU::op_CB15, CPU::op_CB16, CPU::op_CB17, CPU::op_CB18, CPU::op_CB19, CPU::op_CB1A, CPU::op_CB1B, CPU::op_CB1C, CPU::op_CB1D, CPU::op_CB1E, CPU::op_CB1F,
        CPU::op_CB20, CPU::op_CB21, CPU::op_CB22, CPU::op_CB23, CPU::op_CB24, CPU::op_CB25, CPU::op_CB26, CPU::op_CB27, CPU::op_CB28, CPU::op_CB29, CPU::op_CB2A, CPU::op_CB2B, CPU::op_CB2C, CPU::op_CB2D, CPU::op_CB2E, CPU::op_CB2F,
        CPU::op_CB30, CPU::op_CB31, CPU::op_CB32, CPU::op_CB33, CPU::op_CB34, CPU::op_CB35, CPU::op_CB36, CPU::op_CB37, CPU::op_CB38, CPU::op_CB39, CPU::op_CB3A, CPU::op_CB3B, CPU::op_CB3C, CPU::op_CB3D, CPU::op_CB3E, CPU::op_CB3F,
        CPU::op_CB40, CPU::op_CB41, CPU::op_CB42, CPU::op_CB43, CPU::op_CB44, CPU::op_CB45, CPU::op_CB46, CPU::op_CB47, CPU::op_CB48, CPU::op_CB49, CPU::op_CB4A, CPU::op_CB4B, CPU::op_CB4C, CPU::op_CB4D, CPU::op_CB4E, CPU::op_CB4F,
        CPU::op_CB50, CPU::op_CB51, CPU::op_CB52, CPU::op_CB53, CPU::op_CB54, CPU::op_CB55, CPU::op_CB56, CPU::op_CB57, CPU::op_CB58, CPU::op_CB59, CPU::op_CB5A, CPU::op_CB5B, CPU::op_CB5C, CPU::op_CB5D, CPU::op_CB5E, CPU::op_CB5F,
        CPU::op_CB60, CPU::op_CB61, CPU::op_CB62, CPU::op_CB63, CPU::op_CB64, CPU::op_CB65, CPU::op_CB66, CPU::op_CB67, CPU::op_CB68, CPU::op_CB69, CPU::op_CB6A, CPU::op_CB6B, CPU::op_CB6C, CPU::op_CB6D, CPU::op_CB6E, CPU::op_CB6F,
        CPU::op_CB70, CPU::op_CB71, CPU::op_CB72, CPU::op_CB73, CPU::op_CB74, CPU::op_CB75, CPU::op_CB76, CPU::op_CB77, CPU::op_CB78, CPU::op_CB79, CPU::op_CB7A, CPU::op_CB7B, CPU::op_CB7C, CPU::op_CB7D, CPU::op_CB7E, CPU::op_CB7F,
        CPU::op_CB80, CPU::op_CB81, CPU::op_CB82, CPU::op_CB83, CPU::op_CB84, CPU::op_CB85, CPU::op_CB86, CPU::op_CB87, CPU::op_CB88, CPU::op_CB89, CPU::op_CB8A, CPU::op_CB8B, CPU::op_CB8C, CPU::op_CB8D, CPU::op_CB8E, CPU::op_CB8F,
        CPU::op_CB90, CPU::op_CB91, CPU::op_CB92, CPU::op_CB93, CPU::op_CB94, CPU::op_CB95, CPU::op_CB96, CPU::op_CB97, CPU::op_CB98, CPU::op_CB99, CPU::op_CB9A, CPU::op_CB9B, CPU::op_CB9C, CPU::op_CB9D, CPU::op_CB9E, CPU::op_CB9F,
        CPU::op_CBA0, CPU::op_CBA1, CPU::op_CBA2, CPU::op_CBA3, CPU::op_CBA4, CPU::op_CBA5, CPU::op_CBA6, CPU::op_CBA7, CPU::op_CBA8, CPU::op_CBA9, CPU::op_CBAA, CPU::op_CBAB, CPU::op_CBAC, CPU::op_CBAD, CPU::op_CBAE, CPU::op_CBAF,
        CPU::op_CBB0, CPU::op_CBB1, CPU::op_CBB2, CPU::op_CBB3, CPU::op_CBB4, CPU::op_CBB5, CPU::op_CBB6, CPU::op_CBB7, CPU::op_CBB8, CPU::op_CBB9, CPU::op_CBBA, CPU::op_CBBB, CPU::op_CBBC, CPU::op_CBBD, CPU::op_CBBE, CPU::op_CBBF,
        CPU::op_CBC0, CPU::op_CBC1, CPU::op_CBC2, CPU::op_CBC3, CPU::op_CBC4, CPU::op_CBC5, CPU::op_CBC6, CPU::op_CBC7, CPU::op_CBC8, CPU::op_CBC9, CPU::op_CBCA, CPU::op_CBCB, CPU::op_CBCC, CPU::op_CBCD, CPU::op_CBCE, CPU::op_CBCF,
        CPU::op_CBD0, CPU::op_CBD1, CPU::op_CBD2, CPU::op_CBD3, CPU::op_CBD4, CPU::op_CBD5, CPU::op_CBD6, CPU::op_CBD7, CPU::op_CBD8, CPU::op_CBD9, CPU::op_CBDA, CPU::op_CBDB, CPU::op_CBDC, CPU::op_CBDD, CPU::op_CBDE, CPU::op_CBDF,
        CPU::op_CBE0, CPU::op_CBE1, CPU::op_CBE2, CPU::op_CBE3, CPU::op_CBE4, CPU::op_CBE5, CPU::op_CBE6, CPU::op_CBE7, CPU::op_CBE8, CPU::op_CBE9, CPU::op_CBEA, CPU::op_CBEB, CPU::op_CBEC, CPU::op_CBED, CPU::op_CBEE, CPU::op_CBEF,
        CPU::op_CBF0, CPU::op_CBF1, CPU::op_CBF2, CPU::op_CBF3, CPU::op_CBF4, CPU::op_CBF5, CPU::op_CBF6, CPU::op_CBF7, CPU::op_CBF8, CPU::op_CBF9, CPU::op_CBFA, CPU::op_CBFB, CPU::op_CBFC, CPU::op_CBFD, CPU::op_CBFE, CPU::op_CBFF};
};

#endif