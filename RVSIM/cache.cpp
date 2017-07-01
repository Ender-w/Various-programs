/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2017

   Class members for cache

**************************************************************** */

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <sstream>
#include <climits>
#include <bitset>

#include "cache.h"

using namespace std;

// Constructors

cacheBlock::cacheBlock(){
	//data = new uint32_t[dataSize];
	//tag = 0;
	//valid = false;
	//dirty = false;
	//age = 0;
	//cout << "test" << endl;
	
	//all moved to the cache constructor
}

cache::cache(string name,
	     memory_accessible* backing_store,
	     unsigned long int block_size,
	     unsigned long int size,
	     unsigned long int associativity,
	     write_strategy_t write_strategy,
	     bool verbose,
	     bool cycle_reporting,
	     unsigned long int address_cycles,
	     unsigned long int data_cycles) {
  // TODO: ...
  	backing = backing_store;
  	strategy = write_strategy;
  	cacheName = name;
	accessCount = 0;
 	missCount = 0;
 	dirtyReplacedCount = 0;
  	
    int numOfBlocks = size / block_size;
    int numOfSets = numOfBlocks/associativity;
    
    
    for(int i = 0; i < numOfSets; i++){
    	setAges.push_back(0);
    	vector<cacheBlock> tempSet;
    	for(int x = 0; x < associativity; x++){
    		cacheBlock tempBlock;
    		for(int z = 0; z < block_size/4; z++){
    			tempBlock.data.push_back(0);
    		}
    		tempBlock.tag = 0;
    		tempBlock.valid = false;
    		tempBlock.dirty = false;
    		tempBlock.age = 0;
			tempSet.push_back(tempBlock);
    	}
    	cacheRef.push_back(tempSet);
    }
    
    
    offsetMask = block_size - 1;
    setMask = numOfSets - 1 << (int)log2(block_size);
    tagMask = ~(setMask + offsetMask);
    
}

// Read a word of data from an address
uint32_t cache::read_word (uint32_t address) {

  uint32_t offset = address & offsetMask;
  uint32_t set = (address & setMask) >> (int)log2(offsetMask + 1);
  uint32_t tag = address & tagMask;
  
  accessCount++;
  
  int emptyIndex = -1;
  
  for(int i = 0; i < cacheRef[set].size(); i++){
  	if(cacheRef[set][i].valid){
  		if(cacheRef[set][i].tag == tag){//hit
  			setAges[set]++;
  			cacheRef[set][i].age = setAges[set];
  			return cacheRef[set][i].data[offset/4];
  		}
  	}
  }
  //missed
  missCount++;
  
  uint32_t* dataBlock = new uint32_t[(offsetMask + 1)/4];
  uint32_t blockAddress = address & ~offsetMask;
  backing->read_block(blockAddress, dataBlock, offsetMask + 1);
  write_block(blockAddress, dataBlock, offsetMask + 1);
  delete dataBlock;
  
  
  for(int i = 0; i < cacheRef[set].size(); i++){
  	if(cacheRef[set][i].valid){
  		if(cacheRef[set][i].tag == tag){
  			setAges[set]++;
  			cacheRef[set][i].age = setAges[set];
			
  			return cacheRef[set][i].data[offset/4];
  		}
  	}
  }
  
  cerr << "Should never get here" << endl;
}

// Write a word of data to an address, mask contains 1s for bytes to be updated
void cache::write_word (uint32_t address, uint32_t data, uint32_t mask) {
  // TODO: ...
  uint32_t offset = address & offsetMask;
  uint32_t set = (address & setMask) >> (int)log2(offsetMask + 1);
  uint32_t tag = address & tagMask;
  accessCount++;
  
  
  if(offset % 4 != 0){//bloody unaligned address
  	if(offset + 4 > offsetMask - 1){//unaligned over block
  		uint32_t upperAddress = address+2;
  		address = address -2;
  		uint32_t upperMask = mask >> 16;
  		uint32_t upperData = data >> 16;
  		mask = mask << 16;
  		data = data << 16;
  		write_word(upperAddress, upperData, upperMask);
    	}
  }
  
  for(int i = 0; i < cacheRef[set].size(); i++){
  	if(cacheRef[set][i].valid){
  		if(cacheRef[set][i].tag == tag){//hit

  			setAges[set]++;
  			cacheRef[set][i].age = setAges[set];//Update access history for LRU
  			
  			if(strategy == WRITE_THROUGH){
  				uint32_t oldData = cacheRef[set][i].data[offset/4];
  				uint32_t newdata = (data & mask) | (oldData & ~mask);
  				cacheRef[set][i].data[offset/4] = newdata;
  			
  				backing->write_word(address, data, mask);
  			
  			}else{//write_back
  				uint32_t oldData = cacheRef[set][i].data[offset/4];
  				uint32_t newdata = (data & mask) | (oldData & ~mask);
  				cacheRef[set][i].data[offset/4] = newdata;
  				cacheRef[set][i].dirty = true;
  				//cout << "MADE THING DIRTY" <<
  			}
  			return;
  		}
  	}
  }
  missCount++;
  //entry is not in cache ( miss )
  if(strategy == WRITE_THROUGH){
  	backing->write_word(address, data, mask);
  }else{//write_back
  	uint32_t* dataBlock = new uint32_t[(offsetMask+1)/4];
  	uint32_t blockAddress = address & ~offsetMask;
  	backing->read_block(blockAddress, dataBlock, offsetMask+1);
  	write_block(blockAddress, dataBlock, offsetMask+1);
  	delete dataBlock;
  	
  	for(int i = 0; i < cacheRef[set].size(); i++){
  		if(cacheRef[set][i].valid && cacheRef[set][i].tag == tag){
  			setAges[set]++;
  			cacheRef[set][i].age = setAges[set];
  			
  			uint32_t oldData = cacheRef[set][i].data[offset/4];
  			uint32_t newdata = (data & mask) | (oldData & ~mask);
  			cacheRef[set][i].data[offset/4] = newdata;
  			cacheRef[set][i].dirty = true;
  		}
  	}
  }

}

// Read a block of data of a given size, starting at an address
// Data size is a power of 2 number of words, and address is block aligned.
void cache::read_block (uint32_t address, uint32_t data[], unsigned int data_size) {
  // TODO: ...
  //cout << "read line below" << endl;
  accessCount++;
  bool hit;
  uint32_t workingAddress;
  for(int z = 0; z < data_size/4; z++){
  	workingAddress = address + z*4;
  	uint32_t offset = workingAddress & offsetMask;
    uint32_t set = (workingAddress & setMask) >> (int)log2(offsetMask + 1);
    uint32_t tag = workingAddress & tagMask;
    //cout << "Set: " << set << " offset: " << offset << endl;
    hit = false;
	for(int i = 0; i < cacheRef[set].size(); i++){
	  	if(cacheRef[set][i].valid){
	  		if(cacheRef[set][i].tag == tag){//hit
	  			setAges[set]++;
	  			cacheRef[set][i].age = setAges[set];
	  			
	  			data[z] = cacheRef[set][i].data[offset/4];
	  			
	  			//cout << "hit: " << intToHex(data[z]) << endl;
	  			hit = true;
	  			break;
	  		}
	  	}
	}
	//miss on address
	if(!hit){
		missCount++;
		uint32_t* dataBlock = new uint32_t[(offsetMask + 1)/4];
		uint32_t blockAddress = address & ~offsetMask;
		backing->read_block(blockAddress, dataBlock, offsetMask + 1);
		write_block(blockAddress, dataBlock, offsetMask + 1);
		delete dataBlock;
		  
		for(int i = 0; i < cacheRef[set].size(); i++){
		  	if(cacheRef[set][i].valid){
		  		if(cacheRef[set][i].tag == tag){//hit
		  			setAges[set]++;
		  			cacheRef[set][i].age = setAges[set];
		  			
		  			data[z] = cacheRef[set][i].data[offset/4];
		  			
		  			//cout << intToHex(data[z]) << endl;
		  			break;
		  		}
		  	}
		}
	}
  }
  //for(int i = 0; i < data_size/4; i++){
  ///	cout << data[i] << " ";
  //}
  //cout << endl;
}

// Write a block of data of a given size, starting at an address
// Data size is a power of 2 number of words, and address is block aligned.
void cache::write_block (uint32_t address, uint32_t data[], int unsigned data_size) {
  // TODO: ...
  uint32_t offset = address & offsetMask;
  uint32_t set = (address & setMask) >> (int)log2(offsetMask + 1);
  uint32_t tag = address & tagMask;
  int minAge = INT_MAX;
  int minAgeIndex = -1;
  int emptySlot = -1;
  for(int i = 0; i < cacheRef[set].size(); i++){//Find spot to put block
	if(cacheRef[set][i].valid){
	  	if(cacheRef[set][i].age < minAge){
	  		minAgeIndex = i;
	  		minAge = cacheRef[set][i].age;
	  	}
  	}else{//Found an empty slot, dont bother with the other stuff
  		emptySlot = i;
  		break;
  	}
  }
  if(emptySlot != -1){
  	//cout << "Creating new entry at set:" << set << " entry:" << emptySlot << endl;
  	cacheRef[set][emptySlot].valid = true;
  	cacheRef[set][emptySlot].dirty = false;
  	cacheRef[set][emptySlot].tag = tag;
  	//delete cacheRef[set][emptySlot].data;
  	for(int i = 0; i < data_size/4; i++){
  		cacheRef[set][emptySlot].data[i] = data[i];
  	}
  	//cacheRef[set][emptySlot].data = data;
  	setAges[set]++;
  	cacheRef[set][emptySlot].age = setAges[set];
  }else if(cacheRef[set].size() == 1){//direct mapped cache
  	if(cacheRef[set][0].dirty){//write through the data
  		dirtyReplacedCount++;
  		uint32_t* oldDataBlock = new uint32_t[(offsetMask+1)/4];
  		for(int i = 0; i < (offsetMask+1)/4; i++){
  			oldDataBlock[i] = cacheRef[set][0].data[i];
  		}
  		uint32_t oldSet = set << (int)log2(offsetMask + 1);
 	 	uint32_t oldTag = cacheRef[set][0].tag;
  		uint32_t oldBlockAddress = 0;
  		oldBlockAddress = oldBlockAddress + oldSet + oldTag;
  		backing->write_block(oldBlockAddress, oldDataBlock, offsetMask +1);
  		delete oldDataBlock;
  	}
  	cacheRef[set][0].valid = true;
  	cacheRef[set][0].dirty = false;
  	cacheRef[set][0].tag = tag;
  	for(int i = 0; i < data_size/4; i++){
  		cacheRef[set][0].data[i] = data[i];
  	}
  	
  }else{//use LRU
  	if(cacheRef[set][minAgeIndex].dirty){//write through the data
  		dirtyReplacedCount++;
  		uint32_t* oldDataBlock = new uint32_t[(offsetMask+1)/4];
  		for(int i = 0; i < (offsetMask+1)/4; i++){
  			oldDataBlock[i] = cacheRef[set][minAgeIndex].data[i];
  		}
  		uint32_t oldSet = set << (int)log2(offsetMask + 1);
 	 	uint32_t oldTag = cacheRef[set][minAgeIndex].tag;
  		uint32_t oldBlockAddress = 0;
  		oldBlockAddress = oldBlockAddress + oldSet + oldTag;
  		backing->write_block(oldBlockAddress, oldDataBlock, offsetMask +1);
  		delete oldDataBlock;
  	}
  	cacheRef[set][minAgeIndex].valid = true;
  	cacheRef[set][minAgeIndex].dirty = false;
  	cacheRef[set][minAgeIndex].tag = tag;
  	for(int i = 0; i < data_size/4; i++){
  		cacheRef[set][minAgeIndex].data[i] = data[i];
  	}
  	setAges[set]++;
  	cacheRef[set][emptySlot].age = setAges[set];
  }
  
}

// Display on cout whether the address is present, and if so, display the data
void cache::probe_address (uint32_t address) {
  // TODO: ...
  //cerr << "HELP IM GETTING PROBED" << endl;
  uint32_t offset = address & offsetMask;
  uint32_t set = (address & setMask) >> (int)log2(offsetMask + 1);
  uint32_t tag = address & tagMask;
 // cerr << "HELP IM STUCK IN A COMPUTER" << endl;
  int entryIndex = -1;
  bool hit = false;
  bool dirty = false;
  for(int i = 0; i < cacheRef[set].size(); i++){
  	if(cacheRef[set][i].valid){
  		if(cacheRef[set][i].tag == tag){
  			hit = true;
  			entryIndex = i;
  			dirty = cacheRef[set][i].dirty;
  			break;
  		}
  	}
  }
  
  cout << cacheName <<  ": ";
  if(hit){

  	cout << "hit, set = "<< hex << set << ", entry = " << dec << entryIndex;
  	if(dirty){
  		cout << ", dirty";
  	}
  	cout << ":" << endl;
  	cout << "   ";
  	for(int i = 0; i < (offsetMask+1)/4; i++){
  		if(i % 8 == 0 && i != 0){
  			cout << endl;
  		}
  		cout << " " << intToHex(cacheRef[set][entryIndex].data[i]);
  	}
  }else{
  	cout << "miss, set = " << hex << set;
  }
  cout << endl;
  backing->probe_address(address);
}

// Return the accumulated number of cycles for read accesses
unsigned long int cache::get_read_cycle_count () {
  // TODO: ...
}

// Report access stats
void cache::report_accesses () {
  // TODO: ...
  
  cout << cacheName << " access count: " << accessCount << endl;
  if(accessCount > 0){
  	//cout << missCount << endl;
  	cout << fixed;
  	float missRate = ((float)missCount/(float)accessCount);
 	 cout << setprecision(6) << cacheName << " miss rate: " << missRate << endl;
 	 if(strategy == WRITE_BACK){
 	 	//cout << "Dirty Count: " << dirtyReplacedCount << endl;
 	 	cout << setprecision(6) << cacheName << " dirty on replacement rate: " << (float)((float)dirtyReplacedCount/(float)missCount);
 	 }
  }
  
}

string cache::intToHex(uint32_t i){
		stringstream stream;
		stream << setfill ('0') << setw(sizeof(uint32_t)*2) << hex << i;
		return stream.str();
	}	


