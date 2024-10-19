#include <string>

#include "cartridge.h"
#include "logger.h"
#include "memory.h"
#include "types.h"

class Cpu{
    private:
        unsigned char a=0;
        unsigned char b=0;
        unsigned char c=0;
        unsigned char d=0;
        unsigned char e=0;
        unsigned char f=0;
        unsigned char h=0;
        unsigned char l=0;
        unsigned char z_flag=0;
        unsigned char n_flag=0;
        unsigned char h_flag=0;
        unsigned char c_flag=0;
        unsigned short pc=0x0100;
        unsigned short sp=0;

        unsigned char exec_flag = 0;

        Logger logger;
    public:
        Cartridge cartridge;
        Memory memory;
        unsigned char opskip;
        unsigned char mcycle;
        unsigned char cycle_count=0;
        void init();
        bool tick();
        void exec();

        std::string craft_debug();

        void write8_mem(u_short addr, u_char a);
        u_char read8_mem(u_short addr);
        u_short read16_mem(u_short addr);

        void ld8_imm(u_char &a, u_char imm);
        void ld16_imm(u_char &a, u_char &b, u_short imm);

        void inc8(u_char &a);
        void inc16(u_char &a, u_char &b);
        void inc_hl();

        void dec8(u_char &a);
        void dec16(u_char &a, u_char &b);
        void dec_hl();

        void add8(u_char &a, u_char b);
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
};