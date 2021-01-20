#include <iostream>
#include <fstream>
#include <cstdint>
#include <iomanip>
#include "CPU.h"
#include "Memory.h"
#include "Debug.h"
using namespace std;

const int MEMSIZE = 1 << 20;

int main(int argc, char *argv[])
{
    int count, start;
    ifstream exeFile;
    uint8_t byte[4];

    cout << "MIPS Simulator" << endl;
    if(argc != 2) {
        cerr << "usage: " << argv[0] << " mips_executable" << endl;
        return -1;
    }

    exeFile.open(argv[1], ios::binary | ios::in);
    if(!exeFile) {
        cerr << "error: could not open executable file " << argv[1] << endl;
        return -1;
    }

    if(!exeFile.read((char *)&byte, 4)) {
        cerr << "error could not read count from file " << argv[1] << endl;
        return -1;
    }
    start = Memory::swizzle(byte);

    Memory instMem(count << 4, 0x400000, false);
    Memory dataMem(MEMSIZE, 0x10000000, true);

    CPU cpu(start, instMem, dataMem);

    instMem.initFromExe(exeFile, count);
    exeFile.close();

    cout << "Running: " << argv[1] << endl << endl;
    cpu.run();
    cout << endl;
    cpu.printFinalStats();

    return 0;
}