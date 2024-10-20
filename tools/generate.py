import json
import sys
fo = open("output", "w")
sys.stdout = fo

def gen_op_impl_unprefix():
    prefix = "unprefixed"
    f = open("tools/opcodes.json")
    table = json.loads(f.read())
    for op in table[prefix]:
        print("void Cpu::op_", end="")
        print(op.split("x")[1], end="")
        print("(){ ", end="\n")
        print("\topskip={};".format(table[prefix][op]["bytes"]), end="\n")
        print("\tmcycle={};".format(table[prefix][op]["cycles"][0]//4), end="\n")
        print("\tif (exec_flag) ;", end="\n")
        print("} ", end="")
        print("//",table[prefix][op]["mnemonic"], end=" ")
        for operand in table[prefix][op]["operands"]:
            if not operand["immediate"]:
                print("[" + operand["name"], end="")
                if "increment" in operand:
                    print("+",end="")
                elif "decrement" in operand:
                    print("-",end="")
                print("]",end=" ")
            else:
                print(operand["name"], end=" ")
        print()

def gen_op_impl_prefix():
    prefix = "cbprefixed"
    f = open("tools/opcodes.json")
    table = json.loads(f.read())
    for op in table[prefix]:
        print("void Cpu::op_CB", end="")
        print(op.split("x")[1], end="")
        print("(){ ", end="\n")
        print("\topskip={};".format(table[prefix][op]["bytes"]), end="\n")
        print("\tmcycle={};".format(table[prefix][op]["cycles"][0]//4), end="\n")
        print("\tif (exec_flag) ", end="")
        if table[prefix][op]["mnemonic"]!="BIT" and table[prefix][op]["mnemonic"]!="RES" and table[prefix][op]["mnemonic"]!="SET":
            if(table[prefix][op]["immediate"]):
                print(str(table[prefix][op]["mnemonic"]).lower() + "({});".format(table[prefix][op]["operands"][0]["name"].lower()), end="\n")
            else:
                print(str(table[prefix][op]["mnemonic"]).lower() + "_hl" + "();", end="\n")
        elif table[prefix][op]["mnemonic"]=="RES" or table[prefix][op]["mnemonic"]=="SET":
            if(table[prefix][op]["immediate"]):
                print(str(table[prefix][op]["mnemonic"]).lower() + "({}, {});".format(table[prefix][op]["operands"][0]["name"], table[prefix][op]["operands"][1]["name"].lower()), end="\n")
            else:
                print(str(table[prefix][op]["mnemonic"]).lower() + "_hl" + "({});".format(table[prefix][op]["operands"][0]["name"]), end="\n")
        else:
            if(table[prefix][op]["immediate"]):
                print(str(table[prefix][op]["mnemonic"]).lower() + "({}, {});".format(table[prefix][op]["operands"][0]["name"], table[prefix][op]["operands"][1]["name"].lower()), end="\n")
            else:
                print(str(table[prefix][op]["mnemonic"]).lower()  + "({}, {});".format(table[prefix][op]["operands"][0]["name"], "read8_mem(h<<8|l)"), end="\n")
        print("} ", end="")
        print("//",table[prefix][op]["mnemonic"], end=" ")
        for operand in table[prefix][op]["operands"]:
            if not operand["immediate"]:
                print("[" + operand["name"], end="")
                if "increment" in operand:
                    print("+",end="")
                elif "decrement" in operand:
                    print("-",end="")
                print("]",end=" ")
            else:
                print(operand["name"], end=" ")
        print()

def gen_jump_table(prefixed):
    prefix = "" if prefixed==False else "CB"
    a = "0123456789ABCDEF"
    for i in a:
        for j in a:
            print("Cpu::", end="")
            print("op_" + prefix + i + j + ", ", end="")
        print()

def gen_op_def(prefixed):
    prefix = "" if prefixed==False else "CB"
    a = "0123456789ABCDEF"
    for i in a:
        for j in a:
            print("void", end=" ")
            print("op_" + prefix + i + j + "();", end=" ")
        print()

gen_jump_table(True)