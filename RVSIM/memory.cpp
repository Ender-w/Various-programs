/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2017

   Class members for memory

**************************************************************** */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <cstdio>
#include <sstream>

#include "memory.h"
using namespace std;

//ref Constructor
MemoryReference::MemoryReference(){
	for(int i = 0; i < 16; i++){
		ref[i] = NULL;
	}
	access = NULL;
}

// Constructor
memory::memory(bool verbose,
	       bool cycle_reporting,
	       unsigned long int address_cycles,
	       unsigned long int data_cycles) {
  // TODO: ...
  //accessRefCacheAddress = 5000;
  //getRefToMem(0);
  verb = verbose;
  memoryRef = NULL;
  //cerr << "HELP IM STUCK IN A MEMORY CONSTRUCTOR" << endl;
  	//cout << intToHex(read_word(0x00400010)) << endl;
  	//write_word(0x00400010, 0xFFFFFFFF, 0x3B3A3938);
	//cout << intToHex(read_word(0x00400010)) << endl;
}

// Read a word of data from an address
uint32_t memory::read_word (uint32_t address) {
	//cerr << "SETTING MEM AT ADDRESS inside" << endl;
	uint32_t returnVal = 0;
	uint8_t* memoryLocation;
	for(int i = 3; i >= 0; i--){
		returnVal <<= 8;
		//cerr << "Get a ref" << endl;
		memoryLocation = getRefToMem(address + i);
		//cerr << "Got a ref" << endl;
		returnVal = returnVal | *memoryLocation;
	}
	return returnVal;
}

// Write a word of data to an address, mask contains 1s for bytes to be updated
void memory::write_word (uint32_t address, uint32_t data, uint32_t mask) {
   uint8_t* memoryLocation;
   uint32_t oldData = read_word(address);//gotta make this more efficient
   data = (data & mask) | (oldData & ~mask);
   for(int i = 0; i < 4; i++){
   		//cout << "Writing to " << intToHex(address + i) << endl;
   		memoryLocation = getRefToMem(address + i);
   		*memoryLocation = data;
   		data >>= 8;
   }
}

// Read a block of data of a given size, starting at an address
// Data size is a power of 2 number of words, and address is block aligned.
void memory::read_block (uint32_t address, uint32_t data[], unsigned int data_size) {
  // TODO: ...
  uint32_t readAddress;
  uint32_t mask = ~(data_size - 1);
  for(int i = 0; i < data_size; i += 4){
  	readAddress = (mask & address) + i;
  	data[i/4] = read_word(readAddress);
  }
}

// Write a block of data of a given size, starting at an address
// Data size is a power of 2 number of words, and address is block aligned.
void memory::write_block (uint32_t address, uint32_t data[], unsigned int data_size) {
  // TODO: ...
  uint32_t readAddress;
  uint32_t mask = ~(data_size - 1);
  for(int i = 0; i < data_size; i += 4){
    readAddress = (mask & address) + i;
  	write_word(readAddress, data[i/4], 0xFFFFFFFF);
  }
}

// Display on cout whether the address is present, and if so, display the data
void memory::probe_address (uint32_t address) {
  // TODO: ...
  //Memory: 00000013
  
  
  cout << "Memory: " << intToHex(read_word(address)) << endl;
  
}

// Report access stats
void memory::report_accesses () {
  // TODO: ...
}

// Display the word of data at an address
void memory::show_address (uint32_t address) {
  cout << intToHex(read_word(address)) << endl;
}

// Set the word of data at an address
void memory::set_address (uint32_t address, uint32_t data) {
	//cerr << "SETTING MEM AT ADDRESS" << endl;
	write_word(address, data, 0xFFFFFFFF);
}

//This is where the magic happens
uint8_t* memory::getRefToMem(uint32_t address){//array is 256 long
	if(memoryRef == NULL){
		memoryRef = new MemoryReference;
		//memoryRef->ref = new MemoryReference*[16];
		//cerr << "1" << endl;
	}
	MemoryReference* tempRef;
	
	//access cache check
	/*if(address & 0xFFFFFF00 == accessRefCacheAddress){
		return &accessRefCache[address & 0x000000FF];
		if(verb)
			cout << "Used ref cache" << endl;
		//return &tempRef->access[switcher];
	}	*/
	//cerr << "2" << endl;
	//int depth = 31;
	uint32_t switcher;
	int shift = 28;
	uint32_t mask = 0xF0000000;
	switcher = (address & mask) >> shift;
	shift -= 4;
	mask >>= 4;
	int temp = switcher;
	//cerr << "3" << endl;
	//cerr << "Switcher is: " << temp << endl;
	if(memoryRef->ref[temp] == NULL){
		//cerr << "Allocating new ref" << endl;
		memoryRef->ref[temp] = new MemoryReference;
	}
	tempRef = memoryRef->ref[switcher];
	
	//cerr << "4" << endl;
	while (shift > 4){
		//cout << "Mask is: " << intToHex(mask) << endl;
		switcher = (address & mask);
		switcher >>= shift;
		shift -= 4;
		mask >>= 4;
		int temp = switcher;
		//cerr << "Switcher is: " << temp << endl;
		//if(tempRef == NULL){
		//	cerr << "shit" << endl;
		//}
		//if(tempRef->ref == NULL){
		//	cerr << "Allocating new ref array" << endl;
			//tempRef->ref = new MemoryReference*[16];
		//}else{
		//	cerr << "Dont need to allocate new ref array" << endl;
		//}
		//cerr << "Test" << endl;
		/*cerr << "refs: ";
		for(int i = 0; i < 16; i++){
			if(tempRef->ref[i] == NULL){
				cerr << "N ";
			}else{
				cerr << "D ";
			}
		}
		cerr << endl;
		*/
		//cerr << "5" << endl;
		if(tempRef->ref[temp] == NULL){
			//cerr << "Allocating new ref" << endl;
			tempRef->ref[temp] = new MemoryReference;
		}
		//cerr << "Ready to loop" << endl;
		tempRef = tempRef->ref[temp];
		//cerr << "LOOP!" << endl;
		//cerr << "6" << endl;
	}
	//cerr << "7" << endl;
	if(tempRef->access == NULL){
		//cerr << "Allocating new access" << endl;
		//cerr << "8" << endl;
		tempRef->access = new uint8_t[256];
		for(int i = 0; i < 256; i++){
			//cerr << "9" << endl;
			tempRef->access[i] = 0;//initialise all mem to 0
		}
	}
	//cerr << "10" << endl;
	mask = 0x000000FF;
	switcher = address & mask;
	//shift -= 4;
	//mask >> 4;
	temp = switcher;
	//cerr << "Switcher is: " << temp << " at depth: " << depth << endl;
	//accessRefCacheAddress = address & 0xFFFFFF00;
	//accessRefCache = tempRef->access;
	
	//cerr << "11" << endl;
	return &tempRef->access[switcher];
	
	
}

// Return the accumulated number of cycles for read accesses
unsigned long int memory::get_read_cycle_count () {
  // TODO: ...
  return 0;
}

// Load a hex image file
bool memory::load_file(string file_name, uint32_t &start_address) {
  ifstream input_file(file_name);
  string input;
  unsigned long int line_count = 0;
  unsigned long int byte_count = 0;
  char record_start;
  char byte_string[3];
  char halfword_string[5];
  uint32_t record_length;
  uint32_t record_address;
  uint32_t record_type;
  uint32_t record_data;
  uint32_t record_checksum;
  bool end_of_file_record = false;
  uint32_t load_address;
  uint32_t load_data;
  uint32_t load_mask;
  uint32_t load_base_address = 0x00000000;
  start_address = 0x00000000;
  if (input_file.is_open()) {
    while (true) {
      line_count++;
      input_file >> record_start;
      if (record_start != ':') {
	cout << "Input line " << dec << line_count << " does not start with colon character" << endl;
	return false;
      }
      input_file.get(byte_string, 3);
      sscanf(byte_string, "%x", &record_length);
      input_file.get(halfword_string, 5);
      sscanf(halfword_string, "%x", &record_address);
      input_file.get(byte_string, 3);
      sscanf(byte_string, "%x", &record_type);
      switch (record_type) {
      case 0x00:  // Data record
	for (uint32_t i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  load_address = (load_base_address | record_address) + i;
	  // TODO: code to store data byte in memory.
	  //   - load_address is the byte address at which to store
	  //   - record_data is the byte of data to store
	  // ...
	  set_address(load_address, record_data);
	  


	  byte_count++;
	}
	break;
      case 0x01:  // End of file
	end_of_file_record = true;
	break;
      case 0x02:  // Extended segment address (set bits 19:4 of load base address)
	load_base_address = 0x00000000;
	for (uint32_t i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  load_base_address = (load_base_address << 8) | (record_data << 4);
	} //  - load_address is the byte address at which to store
	  //   - record_data is the byte of data to store
	break;
      case 0x03:  // Start segment address (ignored)
	for (uint32_t i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	}
	break;
      case 0x04:  // Extended linear address (set upper halfword of load base address)
	load_base_address = 0x00000000;
	for (uint32_t i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  load_base_address = (load_base_address << 8) | (record_data << 16);
	}
	break;
      case 0x05:  // Start linear address (set execution start address)
	start_address = 0x00000000;
	for (uint32_t i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  start_address = (start_address << 8) | record_data;
	}
	break;
      }
      input_file.get(byte_string, 3);
      sscanf(byte_string, "%x", &record_checksum);
      input_file.ignore();
      if (end_of_file_record)
	break;
    }
    input_file.close();
    cout << dec << byte_count << " bytes loaded, start address = "
	 << setw(8) << setfill('0') << hex << start_address << endl;
    return true;
  }
  else {
    cout << "Failed to open file" << endl;
    return false;
  }
}

string memory::intToHex(uint32_t i){
		stringstream stream;
		stream << setfill ('0') << setw(sizeof(uint32_t)*2) << hex << i;
		return stream.str();
	}	

