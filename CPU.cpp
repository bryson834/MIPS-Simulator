#include "CPU.h"

const string CPU::regNames[] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
                                "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                                "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
                                "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};

CPU::CPU(uint32_t pc, Memory &iMem, Memory &dMem) : pc(pc), iMem(iMem), dMem(dMem)
{
    for (int i = 0; i < NREGS; i++){
        regFile[i] = 0;
    }
    hi = 0;
    lo = 0;
    regFile[28] = 0x10008000;
    regFile[29] = 0x10000000;

    instructions = 0;
    stop = false;
}

void CPU::run()
{
    while(!stop){
        instructions++;

        fetch();
        decode();
        execute();
        mem();
        writeback();

        printRegFile();
    }
}

void CPU::fetch()
{
    instr = iMem.loadWord(pc);
    pc = pc + 4;
}

void CPU::decode() {
  uint32_t opcode;      
  uint32_t rs, rt, rd;  
  uint32_t shamt;      
  uint32_t funct;       
  uint32_t uimm;        
  int32_t simm;         
  uint32_t addr;        

  opcode = instr >>  26 & 0x3F;
  rs = instr >> 21 & 0x1F;
  rt = instr >> 16 & 0x1F;
  rd = instr >> 11 & 0x1F;
  shamt = instr >> 6 & 0x1F;
  funct = instr >> 0 & 0x3F;
  uimm = instr >> 0 & 0xFFFF;
  simm = ((signed)uimm << 16) >> 16;
  addr = instr >> 0 & 0x3FFFFFF;

  writeDest = false;
  opIsLoad = false;
  opIsMultDiv = false;
  opIsStore = false;
  aluSrc1 = REG_ZERO;
  aluSrc2 = REG_ZERO;
  destReg = REG_ZERO;
  storeData = REG_ZERO;
  aluOperations = ADD;

  D(cout << "  " << hex << setw(8) << pc-4 << ": ");
  switch(opcode) {
    case 0x00:
      switch(funct) {
        case 0x00: D(cout << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                // sll datapath which is a shift left stored in rd
                writeDest = true; destReg = rd;
                aluOperations = SHF_L;
                aluSrc1 = regFile[rs];
                aluSrc2 = shamt;
                   break;
        case 0x03: D(cout << "srl " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                // sra  datapath which is a shift right stored in rd
                writeDest = true; destReg = rd;
                aluOperations = SHF_R;
                aluSrc1 = regFile[rs];
                aluSrc2 = shamt;
                   break;
        case 0x18: D(cout << "mult " << regNames[rs] << ", " << regNames[rt]);
               // multiply rs * rt stored in hi & low
               writeDest = false;
               opIsMultDiv = true;
               aluOperations = MUL;
               aluSrc1 = regFile[rs];
               aluSrc2 = regFile[rt];
                   break;
        case 0x21: D(cout << "add " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
               // add unsigned rs + rt stored in rd
               writeDest = true; destReg = rd;
               aluOperations = ADD;
               aluSrc1 = regFile[rs];
               aluSrc2 = regFile[rt];
                   break;
        case 0x23: D(cout << "sub " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
               // subu unsigned rs + rt stored in rd
               writeDest = true; destReg = rd;
               aluOperations = ADD;
               aluSrc1 = regFile[rs];
               aluSrc2 = -regFile[rt];
                   break;
        default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
      }
      break;
    case 0x02: D(cout << "j " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
               writeDest = false;
			   pc = (pc & 0xf0000000) | addr << 2;
               break;
    case 0x04: D(cout << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
                // if rs equals rt change pc
                if(regFile[rs] == regFile[rt])
                {
                pc = pc + (simm << 2);
                }
               break;
    case 0x09: D(cout << "addi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << simm);
               // add simm to rs and store in rt
               writeDest = true; destReg = rt;
               aluOperations = ADD;
               aluSrc1 = regFile[rs];
               aluSrc2 = simm;
               break;
    case 0x0c: D(cout << "andi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << uimm);
               // add uimm to rs and store in rt
               writeDest = true; destReg = rt;
               aluOperations = AND;
               aluSrc1 = regFile[rs];
               aluSrc2 = uimm;
               break;

    case 0x23: D(cout << "lw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
               // load word rt = m(reg file + simm)
               writeDest= true; destReg = rt;
               opIsLoad = true;
               aluOperations = ADD;
               aluSrc1 = regFile[rs];
               aluSrc2 = simm;
               break;
    case 0x2b: D(cout << "sw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
              //store rt
              opIsStore = true;
              storeData = regFile[rt];
              aluOperations = ADD;
              aluSrc1 = regFile[rs];
              aluSrc2 = simm;
               break;
    default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
    }
    D(cout << endl);
}

void CPU::execute()
{
    aluOut = alu.operation(aluOperations, aluSrc1, aluSrc2);
}

void CPU::mem()
{
    if(opIsLoad)
        writeData = dMem.loadWord(aluOut);
    else
    {
        writeData = aluOut;
    }

    if(opIsStore)
        dMem.storeWord(storeData, aluOut);   
}

void CPU::writeback()
{
    if(writeDest && destReg > 0)
        regFile[destReg] = writeData;

    if(opIsMultDiv){
        hi = alu.getHigh();
        lo = alu.getLow();
    }
}

void CPU::printRegFile()
{
    cout << hex;
    for(int i = 0; i < NREGS; i++){
        cout << "    " << regNames[i];
        if(i > 0) cout << "  ";
        cout << ": " << setfill('0') << setw(8) << regFile[i];
        if(i == (NREGS - 1) || (i+1)%4 == 0)
            cout << endl;
    }

    cout << "   hi    : " << setfill('0') << setw(8) << hi;
    cout << "   lo    : " << setfill('0') << setw(8) << lo;
    cout << dec << endl;
}

void CPU::printFinalStats() {
  cout << "Program finished at pc = 0x" << hex << pc << "  ("
       << dec << instructions << " instructions executed)" << endl;
}

