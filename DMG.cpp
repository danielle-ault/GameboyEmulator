#include <iostream>
#include <sstream>
#include "DMG.h"
#include "Utils.h"

DMG::DMG(std::vector<u8> ROM)
{
	this->ROM = ROM;
	ProgramCounter = 0x100;
}

void DMG::RunCycle()
{
	for (int i = 0; i < 4; i++)
		Display.DrawNextPixel();

	Memory[REGISTER_LY] = Display.GetCurrentPixelY();
}

void DMG::RunCycles(int numCycles)
{
	for (int i = 0; i < numCycles; i++)
		RunCycle();
}

int DMG::GetTotalMemoryMapSize()
{
	//return sizeof(MemoryMap) / sizeof(u8);
	return sizeof(Memory);
}

int DMG::SetMemory(u16 address, u8 value)
{
	AddMemoryOperation(address, value, 'W');
	RunCycle();
	Memory[address] = value;
	return 0;
}

u8 DMG::GetMemory(u16 address)
{
	RunCycle();
	u8 value = Memory[address];
	AddMemoryOperation(address, value, 'R');
	return value;
}

void DMG::DisplayMemoryValue(u16 address)
{
	std::cout << "\n0x" << std::hex << address << ": " << std::hex << Memory[address];
}

u8 DMG::GetNextImmediate()
{
	RunCycle();
	ProgramCounter++;
	return ROM[ProgramCounter];
}

u16 DMG::GetNextImmediateAddress()
{
	u8 addressLow = GetNextImmediate();
	u8 addressHigh = GetNextImmediate();
	return (addressHigh << 8) | addressLow;
}

InstructionInfo& DMG::GetLastInstruction()
{
	return InstructionHistory[InstructionHistory.size() - 1];
}

MemoryOperationInfo& DMG::GetLastMemoryOperation()
{
	return MemoryOperationHistory[MemoryOperationHistory.size() - 1];
}

void DMG::AddMemoryOperation(u16 address, u8 value, char readWrite)
{
	MemoryOperationInfo info;
	info.Address = address;
	info.value = value;
	info.ReadWrite = readWrite;
	MemoryOperationHistory.push_back(info);
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

u16 DMG::GetRegisterPairValue(RegisterPair regPair)
{
	u8 low = 0, high = 0;
	switch (regPair)
	{
	case RegisterPair::BC:
		high = GetRegisterValue(Register::B);
		low = GetRegisterValue(Register::C);
		break;
	case RegisterPair::DE:
		high = GetRegisterValue(Register::D);
		low = GetRegisterValue(Register::E);
		break;
	case RegisterPair::HL:
		high = GetRegisterValue(Register::H);
		low = GetRegisterValue(Register::L);
		break;
	case RegisterPair::SP:
		high = (StackPointer & 0xFF00) >> 8;
		low = StackPointer & 0x00FF;
		break;
	}

	return (high << 8) | low;
}

void DMG::SetRegisterPairValue(RegisterPair regPair, u16 value)
{
	u8 low = value & 0xFF;
	u8 high = (value & 0xFF00) >> 8;
	SetRegisterPairValue(regPair, low, high);
}

void DMG::SetRegisterPairValue(RegisterPair regPair, u8 low, u8 high)
{
	switch (regPair)
	{
	case RegisterPair::BC:
		SetRegisterValue(Register::B, high);
		SetRegisterValue(Register::C, low);
		break;
	case RegisterPair::DE:
		SetRegisterValue(Register::D, high);
		SetRegisterValue(Register::E, low);
		break;
	case RegisterPair::HL:
		SetRegisterValue(Register::H, high);
		SetRegisterValue(Register::L, low);
		break;
	case RegisterPair::SP:
		StackPointer = (high << 8) | low;
		break;
	}
}

u16 DMG::GetRegisterPairValuePushPop(RegisterPair regPair)
{
	u8 low = 0, high = 0;
	switch (regPair)
	{
	case RegisterPair::BC:
		high = GetRegisterValue(Register::B);
		low = GetRegisterValue(Register::C);
		break;
	case RegisterPair::DE:
		high = GetRegisterValue(Register::D);
		low = GetRegisterValue(Register::E);
		break;
	case RegisterPair::HL:
		high = GetRegisterValue(Register::H);
		low = GetRegisterValue(Register::L);
		break;
	case RegisterPair::AF:
		high = GetRegisterValue(Register::A);
		low = GetFlagRegister();
		break;
	}

	return (high << 8) | low;
}

void DMG::SetRegisterPairValuePushPop(RegisterPair regPair, u16 value)
{
	u8 high = (value & 0xFF00) >> 8;
	u8 low = value & 0x00FF;

	switch (regPair)
	{
	case RegisterPair::BC:
		SetRegisterValue(Register::B, high);
		SetRegisterValue(Register::C, low);
		break;
	case RegisterPair::DE:
		SetRegisterValue(Register::D, high);
		SetRegisterValue(Register::E, low);
		break;
	case RegisterPair::HL:
		SetRegisterValue(Register::H, high);
		SetRegisterValue(Register::L, low);
		break;
	case RegisterPair::AF:
		SetRegisterValue(Register::A, high);
		SetFlagRegister(low);
		break;
	}
}

bool DMG::TestCondition(Condition condition)
{
	switch (condition)
	{
		case Condition::NZ: return GetFlag(Flag::Z) == 0;
		case Condition::Z:  return GetFlag(Flag::Z) == 1;
		case Condition::NC: return GetFlag(Flag::CY) == 0;
		case Condition::C:  return GetFlag(Flag::CY) == 1;
		default: return 0;
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

std::string DMG::GetRegisterName(u8 reg)
{
	switch ((Register)reg)
	{
		case Register::A: return "A";
		case Register::B: return "B";
		case Register::C: return "C";
		case Register::D: return "D";
		case Register::E: return "E";
		case Register::H: return "H";
		case Register::L: return "L";
		default: return "!";
	}
}

std::string DMG::GetRegisterPairName(RegisterPair regPair)
{
	switch (regPair)
	{
		case RegisterPair::AF: return "AF";
		case RegisterPair::BC: return "BC";
		case RegisterPair::DE: return "DE";
		case RegisterPair::HL: return "HL";
		default: return "!!";
	}
}

u8 DMG::GetMaskedInstruction(u8 instruction)
{
	u8 maskedRegister1 = REGISTER_MASK_1 & instruction;
	u8 maskedRegister2 = REGISTER_MASK_2 & instruction;
	u8 maskedRegisterBoth = REGISTER_MASK_BOTH & instruction;
	u8 maskedRegisterPair = REGISTER_PAIR_MASK & instruction;
	u8 maskedJumpCondition = JUMP_CONDITION_MASK & instruction;

	if (INSTRUCTION_STRINGS.find(instruction) != INSTRUCTION_STRINGS.end())
		return instruction;

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

void DMG::SetFlagRegister(u8 value)
{
	Registers.F = value;
}

u8 DMG::GetFlagRegister()
{
	u8 Z = GetFlag(Flag::Z);
	u8 N = GetFlag(Flag::Z);
	u8 H = GetFlag(Flag::Z);
	u8 CY = GetFlag(Flag::Z);
	return (Z << 7) | (N << 6) | (H << 5) | (CY << 4);
}

void DMG::ProcessNextInstruction(bool updateDisplay)
{
	RunCycle();
	CurrentInstruction = ROM[ProgramCounter];
	CurrentInstructionMasked = GetMaskedInstruction(CurrentInstruction);

	InstructionInfo instructionInfo = InstructionInfo(CurrentInstruction, CurrentInstructionMasked, ProgramCounter);
	InstructionHistory.push_back(instructionInfo);
	
	u8 instruction = CurrentInstruction;
	u8 nextByte = ROM[ProgramCounter + 1];
	u8 nextNextByte = ROM[ProgramCounter + 2];

	switch (CurrentInstructionMasked)
	{
		case LD_R_R:  LoadRegisterToRegister(instruction); break;
		case LD_R_N:  LoadImmediateToRegister(instruction); break;
		case LD_R_HL: LoadHLToRegister(instruction); break;
		case LD_HL_R: LoadRegisterToHL(instruction); break;
		case LD_HL_N: LoadImmediateToHL(); break;
		case LD_A_BC: LoadBCToA(instruction); break;
		case LD_A_DE: LoadDEToA(instruction); break;
		case LD_A_C:  LoadCLowToA(); break;
		case LD_C_A:  LoadAToCLow(); break;
		case LD_A_N:  LoadImmediateLowToA(); break;
		case LD_N_A:  LoadAToImmediateLow(); break;
		case LD_A_NN: LoadImmediateAddressToA(); break;
		case LD_NN_A: LoadAToImmediateAddress(); break;
		case LD_A_HLI: LoadHLIToA(); break;
		case LD_A_HLD: LoadHLDToA(); break;
		case LD_BC_A:
		case LD_DE_A:  LoadAToRegisterPair(instruction); break;
		case LD_HLI_A: LoadAToHLI(); break;
		case LD_HLD_A: LoadAToHLD(); break;

		case LD_DD_NN: LoadImmediateToRegisterPair(instruction); break;
		case LD_SP_HL: LoadHLToStackPointer(); break;
		case PUSH_QQ:  PushRegisterPairToStack(instruction); break;
		case POP_QQ:   PopFromStackToRegisterPair(instruction); break;
		case LDHL_SP_E: LoadStackPointerPlusOffsetToHL(); break;
		case LD_NN_SP: LoadStackPointerToImmediateAddress(); break;

		case ADD_A_R:  OperationFromRegister(Operation::ADD, instruction); break;
		case ADD_A_N:  OperationFromImmediate(Operation::ADD, instruction); break;
		case ADD_A_HL: OperationFromHL(Operation::ADD, instruction); break;
		case ADC_A_R:  OperationFromRegister(Operation::ADC, instruction); break;
		case ADC_A_N:  OperationFromImmediate(Operation::ADC, instruction); break;
		case ADC_A_HL: OperationFromHL(Operation::ADC, instruction); break;
		case SUB_R:    OperationFromRegister(Operation::SUB, instruction); break;
		case SUB_N:    OperationFromImmediate(Operation::SUB, instruction); break;
		case SUB_HL:   OperationFromHL(Operation::SUB, instruction); break;
		case SBC_A_R:  OperationFromRegister(Operation::SBC, instruction); break;
		case SBC_A_N:  OperationFromImmediate(Operation::SBC, instruction); break;
		case SBC_A_HL: OperationFromHL(Operation::SBC, instruction); break;
		case AND_R:  OperationFromRegister(Operation::AND, instruction); break;
		case AND_N:  OperationFromImmediate(Operation::AND, instruction); break;
		case AND_HL: OperationFromHL(Operation::AND, instruction); break;
		case OR_R:   OperationFromRegister(Operation::OR, instruction); break;
		case OR_N:   OperationFromImmediate(Operation::OR, instruction); break;
		case OR_HL:  OperationFromHL(Operation::OR, instruction); break;
		case XOR_R:  OperationFromRegister(Operation::XOR, instruction); break;
		case XOR_N:  OperationFromImmediate(Operation::XOR, instruction); break;
		case XOR_HL: OperationFromHL(Operation::XOR, instruction); break;
		case CP_R:   OperationFromRegister(Operation::CP, instruction); break;
		case CP_N:   OperationFromImmediate(Operation::CP, instruction); break;
		case CP_HL:  OperationFromHL(Operation::CP, instruction); break;
		case INC_R:  IncrementRegister(instruction); break;
		case INC_HL: IncrementHL(); break;
		case DEC_R:  DecrementRegister(instruction); break;
		case DEC_HL: DecrementHL(); break;

		case ADD_HL_SS: AddRegisterPairToHL(instruction); break;
		case ADD_SP_E:  AddStackPointerAndOffset(); break;
		case INC_SS:    IncrementRegisterPair(instruction); break;
		case DEC_SS:    DecrementRegisterPair(instruction); break;

		case RLCA: RotateShiftAccumulator(RotateOperation::RotateLeft); break;
		case RLA:  RotateShiftAccumulator(RotateOperation::RotateLeftCarry); break;
		case RRCA: RotateShiftAccumulator(RotateOperation::RotateRight); break;
		case RRA:  RotateShiftAccumulator(RotateOperation::RotateRightCarry); break;
		case BIT_OP_1ST_BYTE: RotateShiftInstruction(); break;

		case JP_NN:    JumpToImmediate(); break;
		case JP_CC_NN: JumpToImmediateIfTrue(instruction); break;
		case JR_E:     JumpToOffset(); break; // TODO: verify that this converts to negative correctly
		case JR_CC_E:  JumpToOffsetIfTrue(instruction); break;
		case JP_HL:    JumpToHL(); break;

		case CALL_NN:    CallImmediate(); break;
		case CALL_CC_NN: CallImmediateIfCondition(instruction); break;
		case RET:        ReturnFromSubroutine(); break;
		case RETI:       ReturnFromInterrupt(); break;
		case RET_CC:     ReturnIfCondition(instruction); break;
		case RST_T:      RestartCallToAddress(instruction); break;

		default:
			ProgramCounter++;
			//DisplayInstructionString(CurrentInstructionMasked);
			//DisplayRegisters();
	}

	//DisplayInstructionInfo();
}

void DMG::RotateShiftInstruction()
{
	u8 instruction = GetNextImmediate();
	
	switch (instruction)
	{
		case RLC_R:  RotateShiftRegister(RotateOperation::RotateLeft, instruction); break;
		case RL_R:   RotateShiftRegister(RotateOperation::RotateLeftCarry, instruction); break;
		case RRC_R:  RotateShiftRegister(RotateOperation::RotateRight, instruction); break;
		case RR_R:   RotateShiftRegister(RotateOperation::RotateRightCarry, instruction); break;
		case SLA_R:  RotateShiftRegister(RotateOperation::ShiftLeftZero, instruction); break;
		case SRA_R:  RotateShiftRegister(RotateOperation::ShiftRight, instruction); break;
		case SRL_R:  RotateShiftRegister(RotateOperation::ShiftRightZero, instruction); break;
		case SWAP_R: RotateShiftRegister(RotateOperation::Swap, instruction); break;

		case RLC_HL:  RotateShiftHL(RotateOperation::RotateLeft); break;
		case RL_HL:   RotateShiftHL(RotateOperation::RotateLeftCarry); break;
		case RRC_HL:  RotateShiftHL(RotateOperation::RotateRight); break;
		case RR_HL:   RotateShiftHL(RotateOperation::RotateRightCarry); break;
		case SLA_HL:  RotateShiftHL(RotateOperation::ShiftLeftZero); break;
		case SRA_HL:  RotateShiftHL(RotateOperation::ShiftRight); break;
		case SRL_HL:  RotateShiftHL(RotateOperation::ShiftRightZero); break;
		case SWAP_HL: RotateShiftHL(RotateOperation::Swap); break;

		case BIT_B_R:  GetBitFromRegister(instruction); break;
		case BIT_B_HL: GetBitFromHL(instruction); break;
		case SET_B_R:  SetBitInRegister(instruction); break;
		case SET_B_HL: SetBitInHL(instruction); break;
		case RES_B_R:  ResetBitInRegister(instruction); break;
		case RES_B_HL: ResetBitInHL(instruction); break;
	}
}

void DMG::DisplayInstructionHistory(short consoleWidth, short consoleHeight)
{
	int topPadding = 3;
	Utils::DrawHorizontalLineOnConsole(0, 0, CONSOLE_INSTRUCTION_TOTAL_WIDTH - 1);
	Utils::DrawHorizontalLineOnConsole(0, topPadding - 1, CONSOLE_INSTRUCTION_TOTAL_WIDTH - 1);
	Utils::DrawVerticalLineOnConsole(CONSOLE_INSTRUCTION_WIDTH, 1, topPadding - 2);
	Utils::DrawVerticalLineOnConsole(CONSOLE_INSTRUCTION_WIDTH, topPadding, consoleHeight - topPadding);

	if (InstructionHistory.size() > 0)
		DisplayInstructionInfoString(0, 1, GetLastInstruction());

	for (int i = 0; i < consoleHeight - topPadding; i++)
	{
		int index = InstructionHistory.size() - 2 - i;
		if (index >= 0)
			DisplayInstructionInfoString(0, i + topPadding, InstructionHistory[index]);
	}
}

void DMG::DisplayInstructionInfoString(int x, int y, InstructionInfo instructionInfo)
{
	u8 instruction = instructionInfo.Instruction;
	u8 maskedInstruction = GetMaskedInstruction(instruction);

	std::string instructionString;

	if (INSTRUCTION_STRINGS.find(maskedInstruction) != INSTRUCTION_STRINGS.end())
	{
		instructionString = INSTRUCTION_STRINGS.at(maskedInstruction);
	}

	Utils::gotoxy(x, y);
	std::cout << Utils::GetHexString(instructionInfo.Address) << ": ";
	std::cout << Utils::GetBinary(instructionInfo.Instruction) << " 0x";
	std::cout << Utils::GetHexString(instructionInfo.Instruction) << " ";
	std::cout << instructionString;

	Utils::gotoxy(x + CONSOLE_INSTRUCTION_WIDTH + 2, y);
	std::cout << instructionInfo.ExtraInfo;
}

void DMG::DisplayMemoryOperationHistory(short consoleWidth, short consoleHeight)
{
	int topPadding = 3;
	int startX = consoleWidth - MEMORY_OPERATION_WIDTH;
	
	Utils::DrawHorizontalLineOnConsole(startX, 0, MEMORY_OPERATION_WIDTH);
	Utils::DrawHorizontalLineOnConsole(startX, topPadding - 1, MEMORY_OPERATION_WIDTH);
	Utils::DrawVerticalLineOnConsole(startX, 1, topPadding - 2);
	Utils::DrawVerticalLineOnConsole(startX, topPadding, consoleHeight - topPadding);

	if (MemoryOperationHistory.size() > 0)
		DisplayMemoryOperationString(startX + 2, 1, GetLastMemoryOperation());

	for (int i = 0; i < consoleHeight - topPadding; i++)
	{
		int index = MemoryOperationHistory.size() - 2 - i;
		if (index >= 0)
			DisplayMemoryOperationString(startX + 2, i + topPadding, MemoryOperationHistory[index]);
	}
}

void DMG::DisplayMemoryOperationString(int x, int y, MemoryOperationInfo info)
{
	Utils::gotoxy(x, y);
	std::cout << info.ReadWrite << " - ";
	std::cout << Utils::GetHexString(info.Address, true) << ": ";
	std::cout << Utils::GetHexString(info.value);
}

void DMG::DisplayStateInfo()
{
	u8 consoleWidth = Utils::GetConsoleWidth();
	u8 consoleHeight = Utils::GetConsoleHeight();

	Utils::ClearConsole();
	DisplayInstructionHistory(consoleWidth, consoleHeight);
	DisplayMemoryOperationHistory(consoleWidth, consoleHeight);
	Utils::DrawVerticalLineOnConsole(CONSOLE_INSTRUCTION_TOTAL_WIDTH, 0, Utils::GetConsoleHeight());
	DisplayAllRegisters(CONSOLE_INSTRUCTION_TOTAL_WIDTH + 2, 0);
}

void DMG::DisplayValueGeneric(std::string name, u16 value, short x, short y)
{
	Utils::gotoxy(x, y);
	std::cout << name << ": " << std::hex << value;
}

void DMG::DisplayRegister(Register reg, short x, short y)
{
	Utils::gotoxy(x, y);
	std::cout << GetRegisterName((u8)reg);
	std::cout << ": " << std::hex << (u16)GetRegisterValue(reg) << " ";
	//Utils::gotoxy(0, 0);
}

void DMG::DisplayFlags(int x, int y)
{
	Utils::gotoxy(x, y);
	std::cout << "Z | N | H | CY";
	Utils::gotoxy(x, y + 1);
	std::cout << GetFlag(Flag::Z);
	std::cout << " | " << GetFlag(Flag::N);
	std::cout << " | " << GetFlag(Flag::H);
	std::cout << " |  " << GetFlag(Flag::CY);
}

void DMG::DisplayAllRegisters(short x, short y)
{
	DisplayRegister(Register::A, x, y);
	DisplayRegister(Register::B, x, y + 1);
	DisplayRegister(Register::C, x, y + 2);
	DisplayRegister(Register::D, x, y + 3);
	DisplayRegister(Register::E, x, y + 4);
	DisplayRegister(Register::H, x, y + 5);
	DisplayRegister(Register::L, x, y + 6);
	
	DisplayValueGeneric("PC", ProgramCounter, x + 7, y);
	DisplayValueGeneric("SP", StackPointer, x + 7, y + 1);

	DisplayFlags(x, y + 8);
}

std::string DMG::DisplayTransferString(std::string to, std::string from)
{
	return to + "<-" + from;
}

std::string DMG::DisplayTransferString(u8 to, u8 from)
{
	std::stringstream stream;
	stream << Utils::GetHexString(to) << "<-" << Utils::GetHexString(from);
	return stream.str();
}

std::string DMG::DisplayTransferString(std::string to, u8 from)
{
	std::stringstream stream;
	stream << to << "<-" << Utils::GetHexString(from);
	return stream.str();
}

std::string DMG::DisplayTransferString(u8 to, std::string from)
{
	std::stringstream stream;
	stream << Utils::GetHexString(to) << "<-" << from;
	return stream.str();
}

std::string DMG::DisplayTransferString(std::string to, u16 from)
{
	std::stringstream stream;
	stream << to << "<-(" << Utils::GetHexString(from) << ")";
	return stream.str();
}

std::string DMG::DisplayTransferString(u16 to, std::string from)
{
	std::stringstream stream;
	stream << "(" << Utils::GetHexString(to) << ")<-" << from;
	return stream.str();
}

std::string DMG::DisplayTransferString(u16 to, u8 from)
{
	std::stringstream stream;
	stream << "(" << Utils::GetHexString(to) << ")<-" << Utils::GetHexString(from);
	return stream.str();
}

std::string DMG::DisplayTransferString(u8 to, u16 from)
{
	std::stringstream stream;
	stream << Utils::GetHexString(to) << "<-(" << Utils::GetHexString(from) << ")";
	return stream.str();
}

//
// Instruction execution
//

void DMG::LoadRegisterToRegister(u8 instruction)
{
	u8 registerToLoad = instruction & 0b0111;
	u8 registerToLoadTo = (instruction & 0b111000) >> 3;
	u8 valueToLoad = GetRegisterValue((Register)registerToLoad); // TODO: do I need to cast to Register here, or pass a u8?
	SetRegisterValue((Register)registerToLoadTo, valueToLoad);

	//RunCycle();
	ProgramCounter++;

	std::string transferString = DisplayTransferString(GetRegisterName(registerToLoadTo), GetRegisterName(registerToLoad));
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadImmediateToRegister(u8 instruction)
{
	u8 registerToLoadTo = (instruction & 0b111000) >> 3;
	u8 immediateValue = GetNextImmediate();
	SetRegisterValue((Register)registerToLoadTo, immediateValue);
	
	ProgramCounter++;
	
	std::string transferString = DisplayTransferString(GetRegisterName(registerToLoadTo), immediateValue);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadHLToRegister(u8 instruction)
{
	u8 registerToLoadTo = (instruction & 0b111000) >> 3;
	u8 addressLow = GetRegisterValue(Register::L);
	u8 addressHigh = GetRegisterValue(Register::H);
	u16 address = (addressHigh << 8) + addressLow;
	//RunCycle();
	SetRegisterValue((Register)registerToLoadTo, GetMemory(address));

	//RunCycle();
	ProgramCounter++;
	
	std::string transferString = DisplayTransferString(GetRegisterName(registerToLoadTo), address);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadRegisterToHL(u8 instruction)
{
	u8 registerToLoad = (instruction & 0b00000111);
	u8 registerValue = GetRegisterValue((Register)registerToLoad);
	u8 addressLow = GetRegisterValue(Register::L);
	u8 addressHigh = GetRegisterValue(Register::H);
	u16 address = (addressHigh << 8) + addressLow;
	SetMemory(address, registerValue);

	ProgramCounter++;
	
	std::string transferString = DisplayTransferString(address, GetRegisterName(registerToLoad));
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadImmediateToHL()
{
	u16 address = GetRegisterPairValue(RegisterPair::HL);
	u8 immediateValue = GetNextImmediate();
	SetMemory(address, immediateValue);

	//RunCycle();
	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(address, immediateValue);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadBCToA(u8 instruction)
{
	u16 address = GetRegisterPairValue(RegisterPair::BC);
	//RunCycle();
	SetRegisterValue(Register::A, GetMemory(address));

	//RunCycle();
	ProgramCounter++;

	std::string transferString = DisplayTransferString("A", "(BC)");
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadDEToA(u8 instruction)
{
	u16 address = GetRegisterPairValue(RegisterPair::DE);
	//RunCycle();
	SetRegisterValue(Register::A, GetMemory(address));

	//RunCycle();
	ProgramCounter++;

	std::string transferString = DisplayTransferString("A", "(DE)");
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadCLowToA()
{
	u8 addressLow = GetRegisterValue(Register::C);
	u8 addressHigh = 0xFF;
	u16 address = (addressHigh << 8) + addressLow;
	//RunCycle();
	SetRegisterValue(Register::A, GetMemory(address));

	//RunCycle();
	ProgramCounter++;

	std::string transferString = DisplayTransferString("A", "(C)");
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadAToCLow()
{
	u8 addressLow = GetRegisterValue(Register::C);
	u8 addressHigh = 0xFF;
	u16 address = (addressHigh << 8) + addressLow;
	//RunCycle();
	SetMemory(address, GetRegisterValue(Register::A));

	//RunCycle();
	ProgramCounter++;

	std::string transferString = DisplayTransferString("(C)", "A");
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadImmediateLowToA()
{
	u8 addressHigh = 0xFF;
	u16 address = (addressHigh << 8) + GetNextImmediate();
	SetRegisterValue(Register::A, GetMemory(address));

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString("A", address);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadAToImmediateLow()
{
	u8 addressHigh = 0xFF;
	u16 address = ((u16)addressHigh << 8) + GetNextImmediate();
	SetMemory(address, GetRegisterValue(Register::A));

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(address, "A");
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadImmediateAddressToA()
{
	u16 address = GetNextImmediateAddress();
	u8 memoryAtAddress = GetMemory(address);
	SetRegisterValue(Register::A, memoryAtAddress);

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString("A", memoryAtAddress);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadAToImmediateAddress()
{
	u16 address = GetNextImmediateAddress();
	SetMemory(address, GetRegisterValue(Register::A));

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(address, Registers.A);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadHLIToA()
{
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	SetRegisterValue(Register::A, GetMemory(HL));
	SetRegisterPairValue(RegisterPair::HL, HL + 1);

	ProgramCounter += 1;
}

void DMG::LoadHLDToA()
{
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	SetRegisterValue(Register::A, GetMemory(HL));
	SetRegisterPairValue(RegisterPair::HL, HL - 1);

	ProgramCounter += 1;
}

void DMG::LoadAToRegisterPair(u8 instruction)
{
	RegisterPair regPair = (RegisterPair)((instruction & 0b00110000) >> 4);
	u16 address = GetRegisterPairValue(regPair);
	SetMemory(address, GetRegisterValue(Register::A));

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(address, "A");
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadAToHLI()
{
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	SetMemory(HL, GetRegisterValue(Register::A));
	SetRegisterPairValue(RegisterPair::HL, HL + 1);

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(HL, Registers.A);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadAToHLD()
{
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	SetMemory(HL, GetRegisterValue(Register::A));
	SetRegisterPairValue(RegisterPair::HL, HL - 1);

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(HL, Registers.A);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

//
// 16-bit transfer instructions
//

void DMG::LoadImmediateToRegisterPair(u8 instruction)
{
	RegisterPair regPair = (RegisterPair)((instruction & 0b00'110'000) >> 4);
	u16 immediateValue = GetNextImmediateAddress();
	SetRegisterPairValue(regPair, immediateValue);

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(GetRegisterPairName(regPair), Utils::GetHexString(immediateValue));
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadHLToStackPointer()
{
	StackPointer = GetRegisterPairValue(RegisterPair::HL);
	RunCycle();
	
	ProgramCounter += 1;

	std::string transferString = DisplayTransferString("SP", Utils::GetHexString(StackPointer));
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::PushRegisterPairToStack(u8 instruction)
{
	RegisterPair regPair = (RegisterPair)((instruction & 0b00'110'000) >> 4);
	u16 value = GetRegisterPairValuePushPop(regPair);
	RunCycle();
	u8 low = 0xFF & value;
	u8 high = (0xFF00 & value) >> 8;
	SetMemory(StackPointer - 1, high);
	SetMemory(StackPointer - 2, low);
	StackPointer = StackPointer - 2;

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString("SP", GetRegisterPairName(regPair));
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::PopFromStackToRegisterPair(u8 instruction)
{
	RegisterPair regPair = (RegisterPair)((instruction & 0b00'110'000) >> 4);
	u8 low = GetMemory(StackPointer);
	u8 high = GetMemory(StackPointer + 1);
	u16 value = (high << 8) | low;
	SetRegisterPairValue(regPair, value);

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(GetRegisterPairName(regPair), value);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadStackPointerPlusOffsetToHL()
{
	i8 offset = GetNextImmediate();
	
	//TODO: this is undocumented in what I have, hopefully this works
	RunCycle();

	SetRegisterPairValue(RegisterPair::HL, Add(StackPointer, offset));

	SetFlag(Flag::Z, 0);

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString("HL", Utils::GetHexString((u16)(StackPointer + offset)));
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::LoadStackPointerToImmediateAddress()
{
	u16 address = GetNextImmediateAddress();
	u8 low = StackPointer & 0x00FF;
	u8 high = (StackPointer & 0xFF00) >> 8;
	SetMemory(address, low);
	SetMemory(address + 1, high);

	ProgramCounter += 1;

	std::string transferString = DisplayTransferString(address, Utils::GetHexString(StackPointer));
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = transferString;
}

void DMG::OperationFromRegister(Operation operation, u8 instruction)
{
	Register registerToLoad = (Register)(instruction & 0b00'000'111);
	u8 result = PerformOperation(
		operation, 
		GetRegisterValue(Register::A), 
		GetRegisterValue(registerToLoad)
	);
	SetRegisterValue(Register::A, result);

	ProgramCounter++;
}

void DMG::OperationFromImmediate(Operation operation, u8 instruction)
{
	u8 result = PerformOperation(operation, GetRegisterValue(Register::A), GetNextImmediate());
	SetRegisterValue(Register::A, result);

	ProgramCounter++;
}

void DMG::OperationFromHL(Operation operation, u8 instruction)
{
	u16 address = GetRegisterPairValue(RegisterPair::HL);
	u8 memoryValue = GetMemory(address);
	u8 result = PerformOperation(operation, GetRegisterValue(Register::A), memoryValue);
	if (operation != Operation::CP)
		SetRegisterValue(Register::A, result);

	ProgramCounter++;

//	DisplayInstructionString(instruction);
	//DisplayRegisters();
}

u8 DMG::PerformOperation(Operation operation, u8 left, u8 right)
{
	switch (operation)
	{
		case Operation::ADD: return Add(left, right);
		case Operation::ADC: return AddWithCarry(left, right);
		case Operation::SUB: return Subtract(left, right);
		case Operation::SBC: return SubtractWithCarry(left, right);
		case Operation::AND: return And(left, right);
		case Operation::OR:  return Or(left, right);
		case Operation::XOR: return Xor(left, right);
		case Operation::CP:  return Subtract(left, right);
			/*SetFlag(Flag::Z, Compare(left, right));
			return GetFlag(Flag::Z);*/
	}
}

u8 DMG::Add(u8 left, u8 right, bool setCarryFlag, bool setZeroFlag)
{
	SetFlag(Flag::N, 0);

	u8 halfAdd = (0x0F & left) + (0x0F & right);
	bool halfCarry = (0b01'0000 & halfAdd) != 0;
	SetFlag(Flag::H, halfCarry);
	
	u16 result = left + right;
	if (setCarryFlag)
	{
		bool carry = (0b1'0000'0000 & result) != 0;
		SetFlag(Flag::CY, carry);
	}

	if (setZeroFlag)
		SetFlag(Flag::Z, (u8)result == 0);

	return (u8)result;
}

u8 DMG::AddWithCarry(u8 left, u8 right)
{
	SetFlag(Flag::N, 0);

	u8 halfAdd = (0x0F & left) + (0x0F & right) + GetFlag(Flag::CY);
	bool halfCarry = (0b01'0000 & halfAdd) != 0;
	SetFlag(Flag::H, halfCarry);

	u16 result = left + right + GetFlag(Flag::CY);
	bool carry = (0b1'0000'0000 & result) != 0;
	SetFlag(Flag::CY, carry);

	SetFlag(Flag::Z, (u8)result == 0);

	return (u8)result;
}

u8 DMG::Subtract(u8 left, u8 right, bool setCarryFlag)
{
	SetFlag(Flag::N, 1);
	
	u8 halfSubtract = (0x0F & left) - (0x0f & right);
	bool halfCarry = (0b01'0000 & halfSubtract) != 0;
	SetFlag(Flag::H, halfCarry);

	u16 result = left - right;
	if (setCarryFlag)
	{
		bool carry = (0b1'0000'0000 & result) != 0;
		SetFlag(Flag::CY, carry);
	}

	SetFlag(Flag::Z, (u8)result == 0);

	return (u8)result;
}

u8 DMG::SubtractWithCarry(u8 left, u8 right)
{
	SetFlag(Flag::N, 1);

	u8 halfSubtract = (0x0F & left) - (0x0f & right) - GetFlag(Flag::CY);
	bool halfCarry = (0b01'0000 & halfSubtract) != 0;
	SetFlag(Flag::H, halfCarry);

	u16 result = left - right - GetFlag(Flag::CY);
	bool carry = (0b1'0000'0000 & result) != 0;
	SetFlag(Flag::CY, carry);

	SetFlag(Flag::Z, (u8)result == 0);
	
	return (u8)result;
}

u8 DMG::And(u8 left, u8 right)
{
	SetFlag(Flag::CY, 0);
	SetFlag(Flag::H, 1);
	SetFlag(Flag::N, 0);
	
	auto result = left & right;
	
	if (result == 0)
		SetFlag(Flag::Z, 1);
	else
		SetFlag(Flag::Z, 0);

	return result;
}

u8 DMG::Or(u8 left, u8 right)
{
	SetFlag(Flag::CY, 0);
	SetFlag(Flag::H, 0);
	SetFlag(Flag::N, 0);
	
	u8 result = left | right;

	if (result == 0)
		SetFlag(Flag::Z, 1);
	else
		SetFlag(Flag::Z, 0);

	return result;
}

u8 DMG::Xor(u8 left, u8 right)
{
	SetFlag(Flag::CY, 0);
	SetFlag(Flag::H, 0);
	SetFlag(Flag::N, 0);
	
	u8 result = left ^ right;

	if (result == 0)
		SetFlag(Flag::Z, 1);
	else
		SetFlag(Flag::Z, 0);

	return result;
}

u8 DMG::Compare(u8 left, u8 right)
{
	// TODO: implement carry flags here (may not need to, not using this function)
	u8 result = (left == right) ? 1 : 0;
	
	return result;
}

void DMG::IncrementRegister(u8 instruction)
{
	Register registerToLoad = (Register)((instruction & 0b00'111'000) >> 3);
	u8 result = Add(GetRegisterValue(registerToLoad), 1, false);
	SetRegisterValue(registerToLoad, result);

	ProgramCounter++;
}

void DMG::IncrementHL()
{
	u16 address = GetRegisterPairValue(RegisterPair::HL);
	u8 valueAtAddress = GetMemory(address);
	SetMemory(address, Add(valueAtAddress, 1, false));

	ProgramCounter++;
}

void DMG::DecrementRegister(u8 instruction)
{
	Register registerToLoad = (Register)((instruction & 0b00'111'000) >> 3);
	u8 result = Subtract(GetRegisterValue(registerToLoad), 1, false);
	SetRegisterValue(registerToLoad, result);

	ProgramCounter++;
}

void DMG::DecrementHL()
{
	u16 address = GetRegisterPairValue(RegisterPair::HL);
	u8 valueAtAddress = GetMemory(address);
	SetMemory(address, Subtract(valueAtAddress, 1, false));

	ProgramCounter++;
}

u16 DMG::Add16(u16 left, u16 right, bool setZeroFlag)
{
	SetFlag(Flag::N, 0);

	u8 halfAdd = (0x0FFF & left) + (0x0FFF & right);
	bool halfCarry = (0b01'0000'0000'0000 & halfAdd) != 0;
	SetFlag(Flag::H, halfCarry);

	u32 result = left + right;
	bool carry = (0x1'00'00 & result) != 0;
	SetFlag(Flag::CY, carry);

	if (setZeroFlag)
		SetFlag(Flag::Z, (u16)result == 0);

	return (u16)result;
}

u16 DMG::Add16(u16 left, i8 right, bool setZeroFlag)
{
	SetFlag(Flag::N, 0);

	u8 halfAdd = (0x0FFF & left) + right;
	bool halfCarry = (0b01'0000'0000'0000 & halfAdd) != 0;
	SetFlag(Flag::H, halfCarry);

	u32 result = left + right;
	bool carry = (0x1'00'00 & result) != 0;
	SetFlag(Flag::CY, carry);

	if (setZeroFlag)
		SetFlag(Flag::Z, (u16)result == 0);

	return (u16)result;
}

void DMG::AddRegisterPairToHL(u8 instruction)
{
	RegisterPair regPair = (RegisterPair)((instruction & 0b00'110'000) >> 4);
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	RunCycle(); // TODO: this timing isn't documented in docs I have
	u16 result = Add16(HL, GetRegisterPairValue(regPair), false);
	SetRegisterPairValue(RegisterPair::HL, result);

	ProgramCounter++;
}

void DMG::AddStackPointerAndOffset()
{
	i8 offset = GetNextImmediate();
	RunCycles(2); // TODO: timing isnt documented in docs I have
	StackPointer = Add16(StackPointer, offset);
	SetFlag(Flag::Z, 0);

	ProgramCounter++;
}

void DMG::IncrementRegisterPair(u8 instruction)
{
	RegisterPair regPair = (RegisterPair)((instruction & 0b00'110'000) >> 4);
	RunCycle(); // TODO: this timing isn't documented in docs I have
	SetRegisterPairValue(regPair, GetRegisterPairValue(regPair) + 1);

	ProgramCounter++;
}

void DMG::DecrementRegisterPair(u8 instruction)
{
	RegisterPair regPair = (RegisterPair)((instruction & 0b00'110'000) >> 4);
	RunCycle(); // TODO: this timing isn't documented in docs I have
	SetRegisterPairValue(regPair, GetRegisterPairValue(regPair) - 1);

	ProgramCounter++;
}

void DMG::RotateShiftAccumulator(RotateOperation op)
{
	u8 result = PerformRotateShiftOperation(op, Registers.A);
	SetRegisterValue(Register::A, result);
	SetFlag(Flag::H, 0);
	SetFlag(Flag::N, 0);
	SetFlag(Flag::Z, 0);

	ProgramCounter++;
}

void DMG::RotateShiftRegister(RotateOperation op, u8 instruction)
{
	Register reg = (Register)(instruction & 0b00'000'111);
	u8 result = PerformRotateShiftOperation(op, GetRegisterValue(reg));
	RunCycle();
	SetRegisterValue(reg, result);
	SetFlag(Flag::H, 0);
	SetFlag(Flag::N, 0);
	SetFlag(Flag::Z, result == 0);

	ProgramCounter++;
}

void DMG::RotateShiftHL(RotateOperation op)
{
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	u8 result = PerformRotateShiftOperation(op, GetMemory(HL));
	RunCycle();
	SetMemory(HL, result);
	SetFlag(Flag::H, 0);
	SetFlag(Flag::N, 0);
	SetFlag(Flag::Z, result == 0);

	ProgramCounter++;
}

u8 DMG::PerformRotateShiftOperation(RotateOperation op, u8 byte)
{
	switch (op)
	{
		case RotateOperation::RotateLeft:       return RotateLeft(byte);
		case RotateOperation::RotateLeftCarry:  return RotateLeftCarry(byte);
		case RotateOperation::RotateRight:      return RotateRight(byte);
		case RotateOperation::RotateRightCarry: return RotateRightCarry(byte);
		case RotateOperation::ShiftLeftZero:    return ShiftLeftZero(byte);
		case RotateOperation::ShiftRight:       return ShiftRight(byte);
		case RotateOperation::ShiftRightZero:   return ShiftRightZero(byte);
		case RotateOperation::Swap:             return Swap(byte);
	}
}

u8 DMG::RotateLeft(u8 byte)
{
	bool bit7 = (0b1000'0000 & byte) >> 7;
	SetFlag(Flag::CY, bit7);
	u8 result = (byte << 1) | bit7;

	return result;
}

u8 DMG::RotateLeftCarry(u8 byte)
{
	bool bit7 = (0b1000'0000 & byte) >> 7;
	bool carry = GetFlag(Flag::CY);
	SetFlag(Flag::CY, bit7);
	u8 result = (byte << 1) | carry;

	return result;
}

u8 DMG::RotateRight(u8 byte)
{
	bool bit0 = 0b1 & byte;
	SetFlag(Flag::CY, bit0);
	u8 result = (byte >> 1) | (bit0 << 7);

	return result;
}

u8 DMG::RotateRightCarry(u8 byte)
{
	bool bit0 = 0b1 & byte;
	bool carry = GetFlag(Flag::CY);
	SetFlag(Flag::CY, bit0);
	u8 result = (byte >> 1) | (carry << 7);

	return result;
}

u8 DMG::ShiftLeftZero(u8 byte)
{
	bool bit7 = (0b1000'0000 & byte) >> 7;
	SetFlag(Flag::CY, bit7);
	u8 result = (byte << 1) & (0b1111'1110);

	return result;
}

u8 DMG::ShiftRight(u8 byte)
{
	bool bit0 = 0b1 & byte;
	SetFlag(Flag::CY, bit0);
	u8 result = (0b1000'0000 & byte) | (byte >> 1);

	return result;
}

u8 DMG::ShiftRightZero(u8 byte)
{
	bool bit0 = 0b1 & byte;
	SetFlag(Flag::CY, bit0);
	u8 result = (byte >> 1) & 0b0111'1111;

	return result;
}

u8 DMG::Swap(u8 byte)
{
	u8 high = byte & 0xF0;
	u8 low = byte & 0x0F;
	u8 result = (high >> 4) | (low << 4);

	SetFlag(Flag::CY, 0);

	return result;
}

void DMG::GetBitFromRegister(u8 instruction)
{
	u8 bit = (0b00'111'000 & instruction) >> 3;
	Register reg = (Register)(0b00'000'111 & instruction);
	u8 regValue = GetRegisterValue(reg);
	u8 bitValue = ((0b1 << bit) & regValue) >> bit;

	RunCycle();

	SetFlag(Flag::H, 1);
	SetFlag(Flag::N, 0);
	SetFlag(Flag::Z, bitValue);
}

void DMG::GetBitFromHL(u8 instruction)
{
	u8 bit = (0b00'111'000 & instruction) >> 3;
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	u8 memoryValue = GetMemory(HL);
	u8 bitValue = ((0b1 << bit) & memoryValue) >> bit;

	RunCycle();

	SetFlag(Flag::H, 1);
	SetFlag(Flag::N, 0);
	SetFlag(Flag::Z, bitValue);
}

void DMG::SetBitInRegister(u8 instruction)
{
	u8 bit = (0b00'111'000 & instruction) >> 3;
	Register reg = (Register)(0b00'000'111 & instruction);
	u8 regValue = GetRegisterValue(reg);
	u8 newRegValue = (0b1 << bit) | regValue;
	SetRegisterValue(reg, newRegValue);

	RunCycle();
}

void DMG::SetBitInHL(u8 instruction)
{
	u8 bit = (0b00'111'000 & instruction) >> 3;
	Register reg = (Register)(0b00'000'111 & instruction);
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	u8 memoryValue = GetMemory(HL);
	u8 newMemoryValue = (0b1 << bit) | memoryValue;
	SetRegisterValue(reg, newMemoryValue);

	RunCycle();
}

void DMG::ResetBitInRegister(u8 instruction)
{
	u8 bit = (0b00'111'000 & instruction) >> 3;
	Register reg = (Register)(0b00'000'111 & instruction);
	u8 regValue = GetRegisterValue(reg);
	u8 newRegValue = ~(0b1 << bit) & regValue;
	SetRegisterValue(reg, newRegValue);

	RunCycle();
}

void DMG::ResetBitInHL(u8 instruction)
{
	u8 bit = (0b00'111'000 & instruction) >> 3;
	Register reg = (Register)(0b00'000'111 & instruction);
	u16 HL = GetRegisterPairValue(RegisterPair::HL);
	u8 memoryValue = GetMemory(HL);
	u8 newMemoryValue = ~(0b1 << bit) & memoryValue;
	SetRegisterValue(reg, newMemoryValue);

	RunCycle();
}

void DMG::JumpToImmediate()
{
	u16 address = GetNextImmediateAddress();
	RunCycle();
	ProgramCounter = address;
}

void DMG::JumpToImmediateIfTrue(u8 instruction)
{
	u16 address = GetNextImmediateAddress();
	RunCycle();

	Condition condition = (Condition)((instruction & 0b00'011'000) >> 3);
	if (TestCondition(condition))
	{
		ProgramCounter = address;
	}
	else
	{
		ProgramCounter += 1;
	}

	std::string infoString = "cc=" + Utils::GetBinary((long)condition, false);
	InstructionHistory[InstructionHistory.size() - 1].ExtraInfo = infoString;
}

void DMG::JumpToOffset()
{
	i8 offset = GetNextImmediate();
	RunCycle();
	ProgramCounter += offset + 2;
}

void DMG::JumpToOffsetIfTrue(u8 instruction)
{
	i8 offset = GetNextImmediate();
	RunCycle();

	Condition condition = (Condition)((instruction & 0b00'011'000) >> 3);
	if (TestCondition(condition))
	{
		ProgramCounter += offset + 2;
	}
	else
	{
		ProgramCounter += 1;
	}
}

void DMG::JumpToHL()
{
	ProgramCounter = GetRegisterPairValue(RegisterPair::HL);
}

void DMG::CallImmediate()
{
	SetMemory(StackPointer - 1, (ProgramCounter & 0xFF00) >> 8);
	SetMemory(StackPointer - 2, ProgramCounter & 0x00FF);
	ProgramCounter = GetNextImmediateAddress();
	RunCycle(); // timing not documented, but it seems that setting StackPointer takes a cycle?
	StackPointer = StackPointer - 2;
}

void DMG::CallImmediateIfCondition(u8 instruction)
{
	Condition condition = (Condition)((instruction & 0b00'011'000) >> 3);

	if (TestCondition(condition))
		CallImmediate();
	else
		GetNextImmediateAddress();
}

void DMG::ReturnFromSubroutine()
{
	u8 low = GetMemory(StackPointer);
	u8 high = GetMemory(StackPointer + 1);
	ProgramCounter = (high << 8) | low;
	RunCycle();
	StackPointer = StackPointer + 2;
}

void DMG::ReturnFromInterrupt()
{
	u8 low = GetMemory(StackPointer);
	u8 high = GetMemory(StackPointer + 1);
	ProgramCounter = (high << 8) | low;
	RunCycle();
	StackPointer = StackPointer + 2;
}

void DMG::ReturnIfCondition(u8 instruction)
{
	Condition condition = (Condition)((instruction & 0b00'011'000) >> 3);

	if (TestCondition(condition))
		ReturnFromSubroutine();
	else
	{
		RunCycle();
		ProgramCounter++;
	}
}

void DMG::RestartCallToAddress(u8 instruction)
{
	u8 address = ((0b00'111'000 & instruction) >> 3) * 8;

	SetMemory(StackPointer - 1, (ProgramCounter & 0xFF00) >> 8);
	SetMemory(StackPointer - 2, ProgramCounter & 0x00FF);
	RunCycle();
	StackPointer = StackPointer - 2;
	
	ProgramCounter = 0x00FF & address;
}

// apparently documentation for this one is incomplete, and I did 
// run into some code for it here: https://forums.nesdev.org/viewtopic.php?p=196282&sid=2ac4ba1ab86155b864f63750791ff59a#p196282
void DMG::DecimalAdjustAccumulator()
{
	if (!GetFlag(Flag::N)) // after addition
	{
		if (GetFlag(Flag::CY) || Registers.A > 0x99)
		{
			Registers.A += 0x60;
			SetFlag(Flag::CY, 1);
		}
		if (GetFlag(Flag::H) || (Registers.A & 0x0F) > 0x09)
		{
			Registers.A += 0x6;
		}
	}
	else // after subtraction
	{
		if (GetFlag(Flag::CY)) Registers.A -= 0x60;
		if (GetFlag(Flag::H)) Registers.A -= 0x6;
	}

	SetFlag(Flag::Z, (Registers.A == 0));
	SetFlag(Flag::H, 0);

	ProgramCounter++;
}

void DMG::ComplementAccumulator()
{
	SetRegisterValue(Register::A, ~Registers.A);
	SetFlag(Flag::H, 1);
	SetFlag(Flag::N, 1);

	ProgramCounter++;
}

void DMG::FlipCarryFlag()
{
	SetFlag(Flag::CY, !GetFlag(Flag::CY));
	SetFlag(Flag::H, 0);
	SetFlag(Flag::N, 0);

	ProgramCounter++;
}

void DMG::SetCarryFlag()
{
	SetFlag(Flag::CY, 1);
	SetFlag(Flag::H, 0);
	SetFlag(Flag::N, 0);

	ProgramCounter++;
}

void DMG::DisableInterrupts()
{
	// TODO: implement?

	ProgramCounter++;
}

void DMG::EnableInterrupts()
{
	// TODO: implement?

	ProgramCounter++;
}

void DMG::HaltSystemClock()
{
	// TODO: implement

	ProgramCounter++;
}

void DMG::StopSystemAndMainClocks()
{
	// TODO: implement

	ProgramCounter++;
}
