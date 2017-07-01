#ifndef PROCESSOR_H
#define PROCESSOR_H

/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2017

   Class for processor

**************************************************************** */

#include "memory_accessible.h"

using namespace std;

class processor {

 private:

  // TODO: Add private members here
  int32_t registers[32];
  uint32_t pc;
  uint32_t instructionCount;
  memory_accessible* instrucCache;
  memory_accessible* dataCache;
  int64_t breakPoint;
  bool pcSet;
  bool verb;
 public:

  // Consructor
  processor(memory_accessible* icache,
	    memory_accessible* dcache,
	    bool verbose);

  // Display PC value
  void show_pc();

  // Set PC to new value
  void set_pc(uint32_t new_pc);

  // Display register value
  void show_reg(unsigned int reg_num);

  // Set register to new value
  void set_reg(unsigned int reg_num, uint32_t new_value);

  // Execute a number of instructions
  void execute(unsigned int num, bool breakpoint_check);

  // Clear breakpoint
  void clear_breakpoint();

  // Set breakpoint at an address
  void set_breakpoint(uint32_t address);

  // Probe the instruction cache interface for an address
  void probe_instruction (uint32_t address);

  // Probe the data cache interface for an address
  void probe_data (uint32_t address);

  uint64_t get_instruction_count();

  uint64_t get_cycle_count();
  
  string intToHex(uint32_t i);
  
  bool rType(uint32_t currentInstruction);
  bool iType(uint32_t currentInstruction);
  bool sType(uint32_t currentInstruction);
  bool sbType(uint32_t currentInstruction);
  bool uType(uint32_t currentInstruction);
  bool ujType(uint32_t currentInstruction);

};

#endif
