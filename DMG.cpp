#include <iostream>
#include "DMG.h"

DMG::DMG(std::vector<u8> ROM)
{
	this->ROM = ROM;
}

int DMG::GetTotalMemoryMapSize()
{
	//return sizeof(MemoryMap) / sizeof(u8);
	return sizeof(Memory);
}

int DMG::SetMemory(u16 address, u8 value)
{
	Memory[address] = value;
	return 0;
}

int DMG::GetMemoryValue(u16 address)
{
	return Memory[address];
}

u8 DMG::GetRegisterValue(Register reg)
{
	switch (reg)
	{
		case Register::A: return Registers.A;
		case Register::B: return Registers.B;
		case Register::C: return Registers.C;
		case Register::D: return Registers.D;
		case Register::E: return Registers.E;
		case Register::H: return Registers.H;
		case Register::L: return Registers.L;
		default: return 0;
	}

}

void DMG::SetRegisterValue(Register reg, u8 value)
{
	switch (reg)
	{
		case Register::A: Registers.A = value; break;
		case Register::B: Registers.B = value; break;
		case Register::C: Registers.C = value; break;
		case Register::D: Registers.D = value; break;
		case Register::E: Registers.E = value; break;
		case Register::H: Registers.H = value; break;
		case Register::L: Registers.L = value; break;
	}
}

void DMG::DisplayRegister(Register reg, std::string name, bool newLine)
{
	std::cout << name << ": " << std::hex << (unsigned int)GetRegisterValue(reg);
	
	if (newLine)
		std::cout << "\n";
}

void DMG::DisplayRegisters()
{
	std::cout << "\n";
	DisplayRegister(Register::A, "A");
	DisplayRegister(Register::B, "B");
	DisplayRegister(Register::C, "C");
	DisplayRegister(Register::D, "D");
	DisplayRegister(Register::E, "E");
	DisplayRegister(Register::H, "H");
	DisplayRegister(Register::L, "L");
}

char DMG::GetRegisterName(u8 reg)
{
	switch ((Register)reg)
	{
		case Register::A: return 'A';
		case Register::B: return 'B';
		case Register::C: return 'C';
		case Register::D: return 'D';
		case Register::E: return 'E';
		case Register::H: return 'H';
		case Register::L: return 'L';
		default: return '!';
	}
}

u8 DMG::GetMaskedInstruction(u8 instruction)
{
	u8 maskedRegister1 = REGISTER_MASK_1 & instruction;
	u8 maskedRegister2 = REGISTER_MASK_2 & instruction;
	u8 maskedRegisterBoth = REGISTER_MASK_BOTH & instruction;
	u8 maskedRegisterPair = REGISTER_PAIR_MASK & instruction;
	u8 maskedJumpCondition = JUMP_CONDITION_MASK & instruction;

	switch (maskedRegister1)
	{
		case LD_R_N: 
		case LD_R_HL:
		case INC_R:
		case DEC_R:
		case RST_T:
			return maskedRegister1;
	}

	switch (maskedRegister2)
	{
		case LD_HL_R:
		case ADD_A_R:
		case ADC_A_R:
		case SUB_R:
		case SBC_A_R:
		case AND_R:
		case OR_R:
		case XOR_R:
		case CP_R:
			return maskedRegister2;
	}

	switch (maskedRegisterBoth)
	{
		case LD_R_R:
			return maskedRegisterBoth;
	}

	switch (maskedRegisterPair)
	{
		case LD_DD_NN:
		case PUSH_QQ:
		case POP_QQ:
		case ADD_HL_SS:
		case INC_SS:
		case DEC_SS:
			return maskedRegisterPair;
	}

	switch (maskedJumpCondition)
	{
		case JP_CC_NN:
		case JR_CC_E:
		case CALL_CC_NN:
			return maskedJumpCondition;
	}
	
	return instruction;
}

void DMG::SetFlag(Flag flag, bool value)
{
	u8 boolExpanded = 0; // TODO: bad name, come up with better
	boolExpanded = ~boolExpanded; // make it all 1's, like 11111111;

	if (value == true)
	{
		boolExpanded &= (int)flag;
		Registers.F |= boolExpanded;
	}
	else if (value == false)
	{
		boolExpanded &= ~(int)flag;
		Registers.F &= boolExpanded;
	}
}

bool DMG::GetFlag(Flag flag)
{
	u8 result = (int)flag & Registers.F;
	switch (result)
	{
		case (int)Flag::Z: return result >> 7;
		case (int)Flag::N: return result >> 6;
		case (int)Flag::H: return result  >> 5;
		case (int)Flag::CY: return result >> 4;
	}
	return false;
}

void DMG::ProcessInstruction(unsigned char instruction, int* programCounter, std::vector<u8>* ROM_param)
{
	// TODO make these masks consts outside of this function

	/*u8 maskedRegister1 = REGISTER_MASK_1 & instruction;
	u8 maskedRegister2 = REGISTER_MASK_2 & instruction;
	u8 maskedRegister3 = REGISTER_MASK_BOTH & instruction;
	u8 maskedRegisterPair = REGISTER_PAIR_MASK & instruction;
	u8 maskedJumpCondition = JUMP_CONDITION_MASK & instruction;*/
	
	ProgramCounter = *programCounter;

	u8 maskedInstruction = GetMaskedInstruction(instruction);
	u8 nextByte = ROM[ProgramCounter + 1];
	u8 nextNextByte = ROM[ProgramCounter + 2];

	switch (maskedInstruction)
	{
		case LD_R_R: 
			LoadRegisterToRegister(instruction);
			break;
		case LD_R_N:
			LoadImmediateToRegister(instruction, nextByte);
			break;

		default:
			ProgramCounter++;
			DisplayInstructionString(maskedInstruction);
	}

	*programCounter = ProgramCounter;

	return;
	
	if (maskedInstruction == LD_R_R)
	{
		
	}
	else if (maskedInstruction == LD_R_N)
	{
		
	}
	else if (maskedInstruction == LD_R_HL)
	{
		u8 registerToLoadTo = (instruction & 0b111000) >> 3;
		u8 addressLow = GetRegisterValue(Register::L);
		u8 addressHigh = GetRegisterValue(Register::H);
		u16 address = (addressHigh << 8) + addressLow;
		SetRegisterValue((Register)registerToLoadTo, Memory[address]);
		(*programCounter)++;
		DisplayInstructionString(maskedInstruction);
		std::cout << " " << GetRegisterName(registerToLoadTo) << "<-0x" << std::hex << address;
		std::cout << "\n0x" << std::hex << address << ": " << Memory[address];
		DisplayRegisters();
	}
	else if (maskedInstruction == LD_HL_R)
	{
		u8 registerToLoad = (instruction & 0b00000111);
		u8 registerValue = GetRegisterValue((Register)registerToLoad);
		u8 addressLow = GetRegisterValue(Register::L);
		u8 addressHigh = GetRegisterValue(Register::H);
		u16 address = (addressHigh << 8) + addressLow;
		Memory[address] = registerValue;
		(*programCounter)++;
		DisplayInstructionString(maskedInstruction);
		std::cout << " 0x" << std::hex << address << "<-" << GetRegisterName(registerToLoad);
		std::cout << "\n0x" << std::hex << address << ": " << Memory[address];
		DisplayRegisters();
	}
	else if (maskedInstruction == LD_HL_N)
	{
		(*programCounter)++;
		DisplayInstructionString(maskedInstruction);
	}

	else if (maskedInstruction == JP_NN)
	{
		u8 addressLow = (*ROM_param)[++(*programCounter)];
		u8 addressHigh = (*ROM_param)[++(*programCounter)];
		u16 address = (addressHigh << 8) + addressLow;
		*programCounter = address;
		DisplayInstructionString(maskedInstruction);
	}
	else if (maskedInstruction == JP_CC_NN)
	{
		(*programCounter)++;
		DisplayInstructionString(maskedInstruction);
	}
	else if (maskedInstruction == JR_E)
	{
		int8_t e = (*ROM_param)[*programCounter + 1] + 2;
		*programCounter += e;
		DisplayInstructionString(maskedInstruction);
		std::cout << (int)e;
	}
	else if (maskedInstruction == JR_CC_E)
	{
		(*programCounter)++;
		DisplayInstructionString(maskedInstruction);
	}
	else if (maskedInstruction == JP_HL)
	{
		(*programCounter)++;
		DisplayInstructionString(maskedInstruction);
	}
	else // eventually when all instructions are implemented, this else statement should throw an error
	{
		(*programCounter)++;
		DisplayInstructionString(maskedInstruction);
	}
}

void DMG::DisplayInstructionString(u8 instruction)
{
	// TODO: These declarations of the masked instruction are repeated here and in DMG::ProcessInstruction. Figure out a better way to do this.
	u8 maskedRegister1 = REGISTER_MASK_1 & instruction;
	u8 maskedRegister2 = REGISTER_MASK_2 & instruction;
	u8 maskedRegister3 = REGISTER_MASK_BOTH & instruction;
	u8 maskedRegisterPair = REGISTER_PAIR_MASK & instruction;
	u8 maskedJumpCondition = JUMP_CONDITION_MASK & instruction;

	std::string instructionString;

	if (INSTRUCTION_STRINGS.find(maskedRegister1) != INSTRUCTION_STRINGS.end())
	{
		instructionString = INSTRUCTION_STRINGS.at(maskedRegister1);
	}
	else if (INSTRUCTION_STRINGS.find(maskedRegister2) != INSTRUCTION_STRINGS.end())
	{
		instructionString = INSTRUCTION_STRINGS.at(maskedRegister2);
	}
	else if (INSTRUCTION_STRINGS.find(maskedRegister3) != INSTRUCTION_STRINGS.end())
	{
		instructionString = INSTRUCTION_STRINGS.at(maskedRegister3);
	}
	else if (INSTRUCTION_STRINGS.find(maskedRegisterPair) != INSTRUCTION_STRINGS.end())
	{
		instructionString = INSTRUCTION_STRINGS.at(maskedRegisterPair);
	}
	else if (INSTRUCTION_STRINGS.find(maskedJumpCondition) != INSTRUCTION_STRINGS.end())
	{
		instructionString = INSTRUCTION_STRINGS.at(maskedJumpCondition);
	}
	else if (INSTRUCTION_STRINGS.find(instruction) != INSTRUCTION_STRINGS.end())
	{
		instructionString = INSTRUCTION_STRINGS.at(instruction);
	}

	std::cout << instructionString;
}

void DMG::LoadRegisterToRegister(u8 instruction)
{
	u8 registerToLoad = instruction & 0b0111;
	u8 registerToLoadTo = (instruction & 0b111000) >> 3;
	u8 valueToLoad = GetRegisterValue((Register)registerToLoad); // TODO: do I need to cast to Register here, or pass a u8?
	SetRegisterValue((Register)registerToLoadTo, valueToLoad);
	ProgramCounter++;
	DisplayInstructionString(LD_R_R);
	std::cout << " " << GetRegisterName(registerToLoadTo) << "<-" << GetRegisterName(registerToLoad);
	DisplayRegisters();
}

void DMG::LoadImmediateToRegister(u8 instruction, u8 immediateValue)
{
	u8 registerToLoadTo = (instruction & 0b111000) >> 3;
	SetRegisterValue((Register)registerToLoadTo, immediateValue);
	ProgramCounter = ProgramCounter + 2;
	DisplayInstructionString(instruction);
	std::cout << " " << GetRegisterName(registerToLoadTo) << "<-";
	std::cout << std::hex << (((u16)immediateValue) & 0x00FF);
	DisplayRegisters();
}
