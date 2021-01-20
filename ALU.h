#ifndef __ALU_H
#define __ALU_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
//ALU Header File

#include <cstdint>
#include "Debug.h"
using namespace std;

enum ALU_Operations {ADD, AND, SHF_L, SHF_R, CMP_LT, MUL, DIV};

class ALU {
    private:
        uint32_t high, low;
    
    public:
        uint32_t operation(ALU_Operations op, uint32_t src1, uint32_t src2);
        uint32_t getHigh() const { return high; }
        uint32_t getLow() const { return low; }
};

#endif