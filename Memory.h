//Memory Header File

#ifndef __MEMORY_H
#define __MEMORY_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <iomanip>
#include "Debug.h"
using namespace std;

class Memory {
    private:
        uint32_t *mem;
        uint32_t offset;
        int bytes;
        int words;
        bool isDataMem;
        string type;

    public:
        Memory(int bytes, uint32_t offset, bool isDataMem);
        int getSize() const { return bytes; }
        void storeWord(uint32_t data, uint32_t addr);
        uint32_t loadWord(uint32_t addr);
        static uint32_t swizzle(uint8_t *byte);
        void initFromExe(ifstream &exeFile, int count);
};

#endif