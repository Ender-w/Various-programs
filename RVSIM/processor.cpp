/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2017

   Class members for processor

**************************************************************** */
#include <iostream>

#include "processor.h"
#include <sstream>
#include <iomanip>

using namespace std;

  // Consructor
	processor::processor(memory_accessible* icache, 
		memory_accessible* dcache, 
		bool verbose){
		instructionCount = 0;
		instrucCache = icache;
		dataCache = dcache;
		breakPoint = -1;
		pc = 0;
		pcSet = false;
		verb = verbose;
		//float test = 0.83333;
		//cout << setprecision(2) << test << endl;
		//cerr << "HELP IM STUCK IN A CPU CONSTRUCTOR" << endl;
	}

  // Display PC value
	void processor::show_pc(){
		cout << intToHex(pc) << endl;
	}

  // Set PC to new value
	void processor::set_pc(uint32_t new_pc){
		pc = new_pc;
	}

  // Display register value
	void processor::show_reg(unsigned int reg_num){
		cout << intToHex(registers[reg_num]) << endl;
	}

  // Set register to new value
	void processor::set_reg(unsigned int reg_num, uint32_t new_value){
		if(reg_num == 0){
			return;
		}else{
			registers[reg_num] = new_value;
			return;
		}
	}

  // Execute a number of instructions
	void processor::execute(unsigned int num, bool breakpoint_check){
	//cerr << "HELP IM STUCK IN A COMMAND" << endl;
		if(pc%4 != 0){
			//if(verb){
				cout << "Unaligned pc: " << intToHex(pc) << endl;
			//}
			return;
		}
		bool validCommand;
		for(int i = 0; i < num; i++){
			if(breakpoint_check && pc == breakPoint){
				//if(verb){
					cout << "Breakpoint reached at " << intToHex(pc) << endl;
				//}
				return;
			}
			validCommand = true;
			uint32_t currentInstruction;
			currentInstruction = instrucCache->read_word(pc);
			
			/*-----Now the real game begins-----*/
			
			//cout << intToHex(0xFF000000 | 0x00000FF0) << endl;
			//if(verb)
			//	cout << pc << "   ";
			
			uint32_t mask = 0x0000007F;//opcode mask
			uint32_t opcode = currentInstruction & mask;
			switch(opcode){
				//R format
				case 0x33:
				case 0x3B:
					validCommand = rType(currentInstruction);
					break;
				//I format
				case 0x03:
				case 0x0F:
				case 0x13:
				case 0x1B:
				case 0x67:
				case 0x73:
					validCommand = iType(currentInstruction);
					break;
				//S format
				case 0x23:
					validCommand = sType(currentInstruction);
					break;
				//SB format
				case 0x63:
					validCommand = sbType(currentInstruction);
					break;
				//U format
				case 0x17:
				case 0x37:
					validCommand = uType(currentInstruction);
					break;
				//UJ format
				case 0x6F:
					validCommand = ujType(currentInstruction);
					break;
				default:
					validCommand = false;
					break;
			}
						
			/*-----Now the real game finishes-----*/
			
			if(!validCommand){
				//if(verb){
					cout << "Illegal instruction: " << intToHex(currentInstruction) << endl;
				//}
			}
			instructionCount++;
			if(!pcSet){
				pc += 4;
			}
			pcSet = false;
			registers[0] = 0;//force x0 to 0
		}
		
	}

	bool processor::rType(uint32_t currentInstruction){
		uint32_t funct7 = (currentInstruction & 0xFE000000) >> 25;
		uint32_t rs2 = (currentInstruction & 0x01F00000) >> 20;
		uint32_t rs1 = (currentInstruction & 0x000F8000) >> 15;
		uint32_t funct3 = (currentInstruction & 0x00007000) >> 12;
		uint32_t rd = (currentInstruction & 0x00000F80) >> 7;
		uint32_t opcode = currentInstruction & 0x0000007F;
		//cout << "Op: " << opcode << " funct3: " << funct3 << " funct7: " << funct7 << endl;
		switch(opcode){
		case 0x33:
			switch(funct3){
			case 0x0:
				switch(funct7){
				case 0x0://add- 
					registers[rd] = registers[rs1] + registers[rs2];
					if(verb)
						cout << "add instruction" << endl;
					break;
				case 0x20://sub-
					registers[rd] = registers[rs1] - registers[rs2];
					if(verb)
						cout << "sub instruction" << endl;
					break;
				default:
					return false;
				}
				break;
			case 0x1:
				switch(funct7){
				case 0://sll-
					registers[rd] = registers[rs1] << registers[rs2];
					break;
				default:
					return false;
				}
				break;
			case 0x2:
				switch(funct7){
				case 0://slt-
					registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;
					break;
				default:
					return false;
				}
				break;
			case 0x3:
				switch(funct7){
				case 0://sltu-
					registers[rd] = ((uint32_t)registers[rs1] < (uint32_t)registers[rs2]) ? 1 : 0;
					break;
				default:
					return false;
				}
				break;
			case 0x4:
				switch(funct7){
				case 0://xor-
					registers[rd] = registers[rs1] ^ registers[rs2];
					break;
				default:
					return false;
				}
				break;
			case 0x5:
				switch(funct7){
				case 0://srl-
					registers[rd] = (uint32_t)registers[rs1] >> (uint32_t)registers[rs2];
					break;
				case 0x20://sra-
					registers[rd] = registers[rs1] >> registers[rs2];
					break;
				default:
					return false;
				}
				break;
			case 0x6:
				switch(funct7){
				case 0://or-
					registers[rd] = registers[rs1] | registers[rs2];
					break;
				default:
					return false;
				}
				break;
			case 0x7:
				switch(funct7){
				case 0://and-
					registers[rd] = registers[rs1] & registers[rs2];
					break;
				default:
					return false;
				}
				break;
			default:
				return false;
			}
			break;
		case 0x3B:
			switch(funct3){
			case 0x0:
				switch(funct7){
				case 0://addw Unimplemented
					
					break;
				case 0x20://subw Unimplemented
					
					break;
				default:
					return false;
				}
				break;
			case 0x1:
				switch(funct7){
				case 0://sllw Unimplemented
					
					break;
				default:
					return false;
				}
				break;
			case 0x5:
				switch(funct7){
				case 0://srlw Unimplemented
					
					break;
				case 0x20://sraw Unimplemented
					
					break;
				default:
					return false;
				}
				break;
			}
			break;
		}
		return true; 
		
	}
	bool processor::iType(uint32_t currentInstruction){
	
	uint32_t temp5;
	
		int32_t imm11_0 = ((int32_t)currentInstruction) >> 20;
		uint32_t rs1 = (currentInstruction & 0x000F8000) >> 15;
		uint32_t funct3 = (currentInstruction & 0x00007000) >> 12;
		uint32_t rd = (currentInstruction & 0x00000F80) >> 7;
		uint32_t opcode = currentInstruction & 0x0000007F;
		uint32_t funct7 = (currentInstruction & 0xFE000000) >> 25;
		int32_t shamt = (currentInstruction & 0x01F00000) >> 20;//bit of extra parsing just for these guys
		switch(opcode){
			case 0x03:
				switch(funct3){
				case 0://lb-
					//if((registers[rs1] + imm11_0) % 2 != 0)
					//	imm11_0--;
					registers[rd] = dataCache->read_word(registers[rs1] + imm11_0) & 0x000000ff;
					registers[rd] <<= 24;//turns out these need to be sign extended
					registers[rd] >>= 24;
					if(verb)
						cout << "lb instruction, read byte: " << intToHex(registers[rd]) << " at address " << intToHex(registers[rs1] + imm11_0) << " Imm: " << imm11_0 << endl;
					break;
				case 1://lh-
					//if((registers[rs1] + imm11_0) % 2 != 0)
						//imm11_0--;
					registers[rd] = dataCache->read_word(registers[rs1] + imm11_0) & 0x0000ffff;
					registers[rd] <<= 16;//turns out these need to be sign extended
					registers[rd] >>= 16;
					break;
				case 2://lw-
					//if((registers[rs1] + imm11_0) % 2 != 0)
					//	imm11_0--;
					registers[rd] = dataCache->read_word(registers[rs1] + imm11_0);
					if(verb)
						cout << "lw instruction, read byte: " << intToHex(registers[rd]) << " at address " << intToHex(registers[rs1] + imm11_0) << " Imm: " << imm11_0 << endl;
					break;
				case 3://ld
					
					break;
				case 4://lbu-

					//if((registers[rs1] + imm11_0) % 2 != 0){
					//	imm11_0--;
					
					//	}
					registers[rd] = (uint32_t)dataCache->read_word(registers[rs1] + imm11_0) & 0x000000ff;
					if(verb)
						cout << "lbu instruction, read byte: " << intToHex(registers[rd]) << " at address " << intToHex(registers[rs1] + imm11_0) << " Imm: " << imm11_0 << endl;
					break;
				case 5://lhu-
					//if((registers[rs1] + imm11_0) % 2 != 0)
					//	imm11_0--;
					registers[rd] = (uint32_t)dataCache->read_word(registers[rs1] + imm11_0) & 0x0000ffff;
					break;
				case 6://lwu
					
					break;
				default:
					return false;
				}
				return true;
			case 0x0F:
				switch(funct3){
				case 0://fence-
					//unimplemented
					break;
				case 1://fence.i-
					//unimplemented
					break;
				default:
					return false;
				}
				return true;
			case 0x13:
				switch(funct3){
				case 0x0://addi-
					registers[rd] = registers[rs1] + imm11_0;
					if(verb)
						cout << "addi instruction to rs" << rd << " + " << registers[rs1] << endl;
					break;
				case 1://slli-has funct7
					registers[rd] = registers[rs1] << imm11_0;
					break;
				case 2://slti-
					registers[rd] = (registers[rs1] < imm11_0)? 1 : 0;
					break;
				case 3://sltiu-
					registers[rd] = ((uint32_t)registers[rs1] < (uint32_t)imm11_0)? 1 : 0;
					break;
				case 4://xori-
					registers[rd] = registers[rs1] ^ imm11_0;
					break;
				case 5://srli-/srai-has funct7
					
					//shamt >>= 27;//sign extention - not needed
					switch(funct7){
					case 0://srli
						registers[rd] = (uint32_t)registers[rs1] >> shamt;
						break;
					case 0x20://srai
						registers[rd] = registers[rs1] >> shamt;
						break;
					default:
						return false;
					}
					break;
				case 6://ori-
					registers[rd] = registers[rs1] | imm11_0;
					break;
				case 7://andi-
					registers[rd] = registers[rs1] & imm11_0;
					break;
				default:
					return false;
				}
				return true;
			case 0x1B://why do these i types have a funct7 TODO - Seems like they arent part of the req instructions
				switch(funct3){
				case 0://addiw
					
					break;
				case 1://slliw
					
					break;
				case 5://srliw/sraiw
					
					break;
				default:
					return false;
				}
				return true;
			case 0x67:
				if(funct3 == 0){//jalr-
					//cout << "Imm: " << imm11_0 << " Rs1: " << rs1 << endl;
					if(rd != 0)
						registers[rd] = pc + 4;
					while((registers[rs1] + imm11_0) % 2 != 0){
						imm11_0--;
					}
					//if(verb)
					//	cout << "jalr instruction - Imm: " << imm11_0 << " rs1: " << rs1 << " R[rs1]: " << registers[rs1] << endl;
					pc = registers[rs1] + imm11_0;
					pcSet = true;
					if(verb)
						cout << "jalr instruction" << endl;
					return true;
				}
				return	false;
			case 0x73:
				switch(funct3){
				case 0://ecall-/ebreak-
					if(imm11_0 == 0){
						cout << "ecall: unimplemented instruction" << endl;
					}else{
						cout << "ebreak: unimplemented instruction" << endl;
					}
					break;
				case 1://CSRRW-
					cout << "csrrw: unimplemented instruction" << endl;
					break;
				case 2://CSRRS-
					cout << "csrrs: unimplemented instruction" << endl;
					break;
				case 3://CSRRC-
					cout << "csrrc: unimplemented instruction" << endl;
					break;
				case 5://CSRRWI-
					cout << "csrrwi: unimplemented instruction" << endl;
					break;
				case 6://CSRRSI-
					cout << "csrrsi: unimplemented instruction" << endl;
					break;
				case 7://CSRRCI-
					cout << "csrrci: unimplemented instruction" << endl;
					break;
				default:
					return false;
				}
				return true;
		}
	}
	bool processor::sType(uint32_t currentInstruction){
		uint32_t imm11_5 = (currentInstruction & 0xFE000000) >> 25;
		uint32_t rs2 = (currentInstruction & 0x01F00000) >> 20;
		uint32_t rs1 = (currentInstruction & 0x000F8000) >> 15;
		uint32_t funct3 = (currentInstruction & 0x00007000) >> 12;
		uint32_t imm4_0 = (currentInstruction & 0x00000F80) >> 7;
		uint32_t opcode = currentInstruction & 0x0000007F;
		int32_t immJoined = (imm11_5 << 5) | imm4_0;
		immJoined <<= 20;//gotta sign extend
		immJoined >>= 20;
		
		switch(funct3){
		case 0://sb-
			dataCache->write_word(registers[rs1] + immJoined, registers[rs2], 0x000000ff);
			if(verb)
				cout << "sb instruction imm: " << immJoined << " address: " << registers[rs1] << endl;
			break;
		case 1://sh-
			dataCache->write_word(registers[rs1] + immJoined, registers[rs2], 0x0000ffff);
			break;
		case 2://sw-
			dataCache->write_word(registers[rs1] + immJoined, registers[rs2], 0xffffffff);
			if(verb)
				cout << "sw instruction imm: " << immJoined << " address: " << registers[rs1] << endl;
			break;
		case 3://sd
			
			break;
		default:
			return false;
		}
		return true;
		
	}
	bool processor::sbType(uint32_t currentInstruction){
		uint32_t imm12_10_5 = (currentInstruction & 0xFE000000) >> 25;
		uint32_t rs2 = (currentInstruction & 0x01F00000) >> 20;
		uint32_t rs1 = (currentInstruction & 0x000F8000) >> 15;
		uint32_t funct3 = (currentInstruction & 0x00007000) >> 12;
		uint32_t imm4_1_11 = (currentInstruction & 0x00000F80) >> 7;
		uint32_t opcode = currentInstruction & 0x0000007F;
		uint32_t immJoined = ((imm12_10_5 << 5) & 0x7E0) | ((imm12_10_5 << 6) & 0x1000) | (imm4_1_11 & 0x1E) | ((imm4_1_11 << 11) & 0x800);
		int32_t immSigned = (int32_t)(immJoined << 19) >> 19;
		//cout << immSigned << endl;
		switch(funct3){
			case 0://beq-
				if(registers[rs1] == registers[rs2]){
					pc = pc + immSigned;
					pcSet = true;
				}
				break;
			case 1://bne-
				if(registers[rs1] != registers[rs2]){
					pc = pc + immSigned;
					pcSet = true;
				}
				if(verb){
					cout << "bne instruction, condition: " << ((pcSet)? "Passed" : "Failed") << " New pc is: " << pc << endl;
					cout << "Condition was: " << registers[rs1] << " != " << registers[rs2] << endl;
				}
						
				break;
			case 4://blt-
				if(registers[rs1] < registers[rs2]){
					pc = pc + immSigned;
					pcSet = true;
				}
				break;
			case 5://bge-
				if(registers[rs1] >= registers[rs2]){
					pc = pc + immSigned;
					pcSet = true;
				}
				break;
			case 6://bltu- - WAT TODO TODO TODO
				if((uint32_t)registers[rs1] < (uint32_t)registers[rs2]){
					pc = pc + immSigned;
					pcSet = true;
				}
				break;
			case 7://bgeu- WAT
				if((uint32_t)registers[rs1] >= (uint32_t)registers[rs2]){
					pc = pc + immSigned;
					pcSet = true;
				}
				break;
			default:
				return false;
		}
		return true;
	}
	bool processor::uType(uint32_t currentInstruction){
		int32_t imm31_12 = (int32_t)currentInstruction & 0xFFFFF000;
		uint32_t rd = (currentInstruction & 0x00000F80) >> 7;
		uint32_t opcode = currentInstruction & 0x0000007F;
		
		switch(opcode){
			case 0x37://lui-
				registers[rd] = imm31_12;
				if(verb)
						cout << "lui instruction" << endl;
				return true;
			case 0x17://auipc-
				registers[rd] = pc + imm31_12;
				return true;
		}
		
	}
	bool processor::ujType(uint32_t currentInstruction){
		uint32_t imm = ((int32_t)currentInstruction & 0xFFFFF000) >> 11;
		uint32_t rd = (currentInstruction & 0x00000F80) >> 7;
		uint32_t opcode = currentInstruction & 0x0000007F;
		//uint32_t immJoined = ((immLongThing 
		//dafaq is this long jumbled shit
		uint32_t immJoined = (imm & 0x100000) | ((imm & 0xFFC00) >> 9) | ((imm & 0x200) << 2) | ((imm & 0x1FE) << 11);
		int32_t immSigned = (int32_t)(immJoined << 11) >> 11;
		//jal-
		if(verb)
			cout << "jal instruction, imm val: " << (int)immSigned << " rd: " << rd << " pc: " << pc << endl;
		registers[rd] = pc + 4;
		pc = pc + immSigned;
		pcSet = true;
		if(verb)
			cout << "pc: " << pc << endl;
		return true;
	}

  // Clear breakpoint
	void processor::clear_breakpoint(){
		breakPoint = -1;
		//cout << breakPoint << endl;
	}

  // Set breakpoint at an address
	void processor::set_breakpoint(uint32_t address){
		breakPoint = address;
	}

  // Probe the instruction cache interface for an address
	void processor::probe_instruction (uint32_t address){
		instrucCache->probe_address(address);
	}

  // Probe the data cache interface for an address
	void processor::probe_data (uint32_t address){
		dataCache->probe_address(address);
	}

	uint64_t processor::get_instruction_count(){
	
		return instructionCount;
	}

	uint64_t processor::get_cycle_count(){
	
		return 0;
	}
	
	string processor::intToHex(uint32_t i){
		stringstream stream;
		stream << setfill ('0') << setw(sizeof(uint32_t)*2) << hex << i;
		return stream.str();
	}	
	


