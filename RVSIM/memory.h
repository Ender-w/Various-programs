#ifndef MEMORY_H
#define MEMORY_H

/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2017

   Class for memory

**************************************************************** */

#include "memory_accessible.h"

using namespace std;

struct MemoryReference {
	public:
	MemoryReference* ref[16];
	uint8_t* access;
	
	MemoryReference();
	
	
};

class memory: public memory_accessible {

 private:

  // TODO: Add private members here
  MemoryReference* memoryRef;
  uint8_t* accessRefCache;
  uint32_t accessRefCacheAddress;
  bool verb;

 public:

  // Constructor
  memory(bool verbose,
	 bool cycle_reporting,
	 unsigned long int address_cycles,
  	 unsigned long int data_cycles);
  	 
  // Read a word of data from an address
  uint32_t read_word (uint32_t address);

  // Write a word of data to an address, mask contains 1s for bytes to be updated
  void write_word (uint32_t address, uint32_t data, uint32_t mask);

  // Read a block of data of a given size, starting at an address
  // Data size is a power of 2 number of words, and address is block aligned.
  void read_block (uint32_t address, uint32_t data[], unsigned int data_size);

  // Write a block of data of a given size, starting at an address
  // Data size is a power of 2 number of words, and address is block aligned.
  void write_block (uint32_t address, uint32_t data[], unsigned int data_size);

  // Display on cout whether the address is present, and if so, display the data
  void probe_address (uint32_t address);

  // Report access stats
  void report_accesses ();

  // Display the word of data at an address
  void show_address (uint32_t address);

  // Set the word of data at an address
  void set_address (uint32_t address, uint32_t data);

  // Return the accumulated number of cycles for read accesses
  unsigned long int get_read_cycle_count ();
  
  uint8_t* getRefToMem(uint32_t address);

  // Load a hex image file
  bool load_file(string file_name, uint32_t &start_address);
  
  string intToHex(uint32_t i);

};

#endif
