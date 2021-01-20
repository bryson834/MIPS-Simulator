//Disassembler

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <fstream>

using namespace std;

const int NREGS = 32;
const string regNames[NREGS] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
                                "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                                "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
                                "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};

void disassembleInstruction(uint32_t pc, uint32_t instruction)
{
    uint32_t opcode;
    uint32_t rs, rt, rd;
    uint32_t shamt, funct, uimm;
    uint32_t address;
    int32_t simm;

    opcode = instruction >> 26 & 0x3F; 
    rs = instruction >> 21 & 0x1F;
    rt = instruction >> 16 & 0x1F;
    rd = instruction >> 11 & 0x1F;
    shamt = instruction >> 6 & 0x1F;
    funct = instruction >> 0 & 0x3F;
    uimm = instruction >> 0 & 0xFFFF;
    simm = instruction >> 0 & 0xFFFF;
    address = instruction >> 0 & 0x3FFFFFF;
    cout << hex << setw(8) << pc << ": ";

    if(simm & 0x8000)
    {
        simm = ( (-instruction & 0xFFFF) + 0x0001) * -1;
    }

    switch(opcode) 
    {
    case 0x00:
      switch(funct) 
      {
          case 0x20: cout << "add " << regNames[rd] << ", " << regNames[rs] << ", "  << regNames[rt]; break;
          case 0x08: cout << "addi " << regNames[rt] << ", " << regNames[rs] << ", " << simm; break;
          case 0x22: cout << "sub " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;
          case 0x00: cout << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt; break;
          case 0x02: cout << "srl " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt; break;
          case 0x18: cout << "mul " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;
          case 0x24: cout << "and " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]; break;
          case 0x25: cout << "or " << endl;
          default: cout << "No Function Implemented.";
      }
      break;

    case 0x02: cout << "j " << hex << ((pc + 4) & 0xf0000000) + address * 4; break;
    case 0x04: cout << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << pc + 4 + (simm << 2); break;
    case 0x2B: cout << "sw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")"; break;
    case 0x23: cout << "lw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")"; break;
    default: cout << "No Function Implemented.";
    }
    cout << endl;
}

uint32_t swizzle(uint8_t *bytes) {
  return (bytes[0] << 24) | (bytes[1] << 16) | bytes[2] << 8 | bytes[3];
}

int main(int argc, char *argv[])
{
    int count, start;
    ifstream inFile;
    uint8_t bytes[4];
    uint32_t *instructions;

    cout << "MIPS Disassembler" << endl;
    if(argc != 2) {
        cout << "Usage: " << argv[0] << " MIPS_Executable" << endl;
        return -1;
    }

    inFile.open(argv[1], ios::binary | ios::in);
    if(!inFile) {
        cout << "Error! Could not open file." << argv[1] << endl;
        return -1;
    }

    if(!inFile.read((char *) &bytes, 4)) {
        cout << "Error! Could not read from file." << argv[1] << endl;
        return -1;
    }

    count = swizzle(bytes);
    if(!inFile.read((char *) &bytes, 4)) {
        cout << "Error! Could not read start address from file." << argv[1] << endl;
        return -1;
    }
    start = swizzle(bytes);

    instructions = new uint32_t[count];
    if(!instructions) {
        cout << "Error! No memory." << endl;
        return -1;
    }

    for(int i = 0; i < count; i++) {
        if(!inFile.read((char *) &bytes, 4)) {
            cout << "Error! Could not read instructions from filename." << endl;
            return -1;
        }
        instructions[i] = swizzle(bytes);
    }

    inFile.close();

    for(int i = 0; i < count; i++) {
        disassembleInstruction(start + i*4, instructions[i]);
    }
    
    return 0;
}


