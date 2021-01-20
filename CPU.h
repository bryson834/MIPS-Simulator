#ifndef __CPU_H
#define __CPU_H

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <cstdlib>
#include "Memory.h"
#include "ALU.h"
#include "Debug.h"
using namespace std;

class CPU{

    private:
        static const int NREGS = 32;
        static const string regNames[];
        static const int REG_ZERO = 0;
        static const int REG_RA = 31;
        static const int REG_HILO = 32;
        uint32_t pc;
        uint32_t instr;
        uint32_t regFile[NREGS];
        uint32_t hi,lo;

        ALU alu;
        Memory &iMem;
        Memory &dMem;

        int instructions;
        bool stop;
        
        bool opIsLoad, opIsStore, opIsMultDiv;
        ALU_Operations aluOperations;
        bool writeDest;
        int destReg;
        uint32_t aluSrc1, aluSrc2;
        uint32_t storeData;
        uint32_t aluOut;
        uint32_t writeData;

    public:
        CPU(uint32_t pc, Memory &iMem, Memory &dMem);
        void run();
        void printFinalStats();
        void printRegFile();

    private:
        void fetch();
        void decode();
        void execute();
        void mem();
        void writeback();
};

#endif
