# MIPS-Simulator

1. Verify that there are no errors in the code for each component.

2. Create output files with .o extension to use for simulator.

  - g++ -c ALU.cpp
  - g++ -c Memory.cpp
  - g++ -c CPU.cpp
  - g++ -c Simulator.cpp
  
  OR
  
  - g++ -c ALU.cpp Memory.cpp CPU.cpp Simulator.cpp
  
3. Create an executable for testing (simulator)
  - g++ ALU.o CPU.o Memory.o Simulator.o -o simulator
  
4. Run this command line state alongside test programs.
Example:
  - ./simulator test1.mips
  - ./simulator test2.mips
  - ./simulator hello.mips
  
  - ./simulator "_____".mips
  
Further Notes:
In changes are made to code and new output files need to be created, run this statement
- rm -f ALU.o CPU.o Memory.o Simulator.o simulator

Extra Work:
If you want to use the disassembler, you can run the following commands:

  - g++ Disassembler.cpp
  - g++ -o Disassembler Disassembler.cpp
  - ./Disassembler test.mips
               or
  - ./Disassembler ".mips test file"
  
