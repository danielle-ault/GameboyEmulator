#pragma once
#include <variant>
#include <vector>
#include <stdint.h>
#include <string>
#include <map>

typedef uint8_t u8;
typedef uint16_t u16;

enum class Register
{
	A = 0b111,
	B = 0b000,
	C = 0b001,
	D = 0b010,
	E = 0b011,
	H = 0b100,
	L = 0b101,
};

enum class Flag
{
	Z  = 0b1000'0000,
	N  = 0b0100'0000,
	H  = 0b0010'0000,
	CY = 0b0001'0000,
};

enum class Operation
{
	ADD,
	ADC,
	SUB,
	SBC,
	AND,
	OR,
	XOR,
	CP
};

struct Registers_t
{
	u8 A;
	u8 F;
	u8 B;
	u8 C;
	u8 D;
	u8 E;
	u8 H;
	u8 L;
};

const int REG_A = 0b111;
const int REG_B = 0b000;
const int REG_C = 0b001;
const int REG_D = 0b101;
const int REG_E = 0b011;
const int REG_H = 0b100;
const int REG_L = 0b101;

const int LD_IMMEDIATE = 0b00;
const int LD_CONTENTS  = 0b01;

const int LD_OPCODE = 0b110; // TODO: not sure what to name this yet, so defaulting to opcode even though that's not technically correct I think.

//typedef unsigned int uint24_t;
//typedef std::variant<u8, u16, uint24_t> instruction;

struct MemoryMap
{
	u8 interrupt[0x100];
	u8 ROMData[0x50];
	u8 userProgram[0x7EB0];
	u8 characterData[0x1800];
	u8 BGDisplayData1[0x400];
	u8 BGDisplayData2[0x400];
	u8 externalExpansionWRAM[0x2000];
	u8 unitWRAM[0x2000];
	u8 unused[0x1E00]; //don't use
	u8 OAM[0xA0];
	u8 unused2[0x60]; //don't use
	u8 registers[0x80];
	u8 workingAndStackRAM[0x7f];
};

class DMG
{
public:
	//MemoryMap Memory;
	u8 Memory[0xFFFF] = { 0 };
	Registers_t Registers = { 0 };
	size_t ProgramCounter = 0;
	std::vector<u8> ROM;

	DMG(std::vector<u8> ROM);

	int GetTotalMemoryMapSize();
	int SetMemory(u16 address, u8 value);
	int GetMemoryValue(u16 address);
	void DisplayMemoryValue(u16 address);

	u8 GetRegisterValue(Register reg);
	void SetRegisterValue(Register reg, u8 value);
	void DisplayRegister(Register reg, std::string name, bool newLine = true);
	void DisplayRegisters();
	std::string GetRegisterName(u8 reg);

	u8 GetMaskedInstruction(u8 instruction);

	void SetFlag(Flag flag, bool value);
	bool GetFlag(Flag flag);

	void ProcessInstruction(unsigned char instruction, int* programCounter, std::vector<u8>* ROM);

	void DisplayInstructionString(u8 instruction);
	void DisplayTransferString(std::string to, std::string from);
	void DisplayTransferString(u8 to, u8 from);
	void DisplayTransferString(std::string to, u8 from);
	void DisplayTransferString(u8 to, std::string from);
	void DisplayTransferString(std::string to, u16 from);
	void DisplayTransferString(u16 to, std::string from);
	void DisplayTransferString(u16 to, u8 from);
	void DisplayTransferString(u8 to, u16 from);

	////
	//// Instruction execution
	////
	
	//
	// 8-bit transfer instructions
	//
	
	// LD r,r'
	void LoadRegisterToRegister(u8 instruction);
	
	// LD r,n
	void LoadImmediateToRegister(u8 instruction, u8 immediateValue);
	
	// LD r,(HL) 
	void LoadHLToRegister(u8 instruction);
	
	// LD (HL),r
	void LoadRegisterToHL(u8 instruction);
	
	// LD (HL),n
	void LoadImmediateToHL(u8 immediateValue);
	
	// LD A,(BC)
	void LoadBCToA(u8 instruction);

	// LD A,(DE)
	void LoadDEToA(u8 instruction);
	
	// LD A,(C)
	void LoadCLowToA();
	
	// LD (C),A
	void LoadAToCLow();
	
	// LD A,(n)
	void LoadImmediateLowToA(u8 immediateValue);
	
	// LD (n),A
	void LoadAToImmediateLow(u8 immediateValue);
	
	// LD A,(nn)
	void LoadImmediateAddressToA(u8 immediateLow, u8 immediateHigh);
	
	// LD (nn),A
	void LoadAToImmediateAddress(u8 immediateLow, u8 immediateHigh);

	// LD A,(HLI)
	void LoadHLIToA();

	// LD A, (HLD)
	void LoadHLDToA();

	// LD (BC),A  and  LD (DE),A
	void LoadAToRegisterPair(u8 instruction);

	// LD (HLI),A
	void LoadAToHLI();

	// LD (HLD),A
	void LoadAToHLD();
	
	//
	// 16-bit transfer instructions
	//
	
	// LD dd,nn
	void LoadImmediateToRegisterPair(u8 instruction, u8 immediateLow, u8 immediateHigh);

	//
	// 8-bit arithmetic and logical operation instructions
	//

	void OperationFromRegister(Operation operation, u8 instruction);
	void OperationFromImmediate(Operation operation, u8 instruction, u8 immediate);
	void OperationFromHL(Operation operation, u8 instruction);

	u8 PerformOperation(Operation operation, u8 left, u8 right);

	u8 Add(u8 left, u8 right);
	u8 AddWithCarry(u8 left, u8 right);
	u8 Subtract(u8 left, u8 right);
	u8 SubtractWithCarry(u8 left, u8 right);
	u8 And(u8 left, u8 right);
	u8 Or(u8 left, u8 right);
	u8 Xor(u8 left, u8 right);
	u8 Compare(u8 left, u8 right);

	// INC r
	void IncrementRegister(u8 instruction);
	
	// INC (HL)
	void IncrementHL();

	// DEC r
	void DecrementRegister(u8 instruction);

	// DEC (HL)
	void DecrementHL();
	

	//
	// 16-bit arithmetic operation instructions
	//

	//
	// Rotate shift instructions
	//

	//
	// Bit operations
	//

	//
	// Jump instructions
	//

	//
	// Call/return instructions
	//

	//
	// Gen-purpose arithmetic/CPU control instructions
	//


	// Instruction bit masks
	const u8 REGISTER_MASK_1     = 0b11'000'111;
	const u8 REGISTER_MASK_2     = 0b11'111'000;
	const u8 REGISTER_MASK_BOTH  = 0b11'000'000;
	const u8 REGISTER_PAIR_MASK  = 0b11'001'111;
	const u8 JUMP_CONDITION_MASK = 0b11'100'111;

	// 8-bit transfer/input-output instructions
	static const u8 LD_R_R   = 0b01'000'000;
	static const u8 LD_R_N   = 0b00'000'110;
	static const u8 LD_R_HL  = 0b01'000'110;
	static const u8 LD_HL_R  = 0b01'110'000;
	static const u8 LD_HL_N  = 0b00'110'110;
	static const u8 LD_A_BC  = 0b00'001'010;
	static const u8 LD_A_DE  = 0b00'011'010;
	static const u8 LD_A_C   = 0b11'110'010;
	static const u8 LD_C_A   = 0b11'100'010;
	static const u8 LD_A_N   = 0b11'110'000;
	static const u8 LD_N_A   = 0b11'100'000;
	static const u8 LD_A_NN  = 0b11'111'010;
	static const u8 LD_NN_A  = 0b11'101'010;
	static const u8 LD_A_HLI = 0b00'101'010;
	static const u8 LD_A_HLD = 0b00'111'010;
	static const u8 LD_BC_A  = 0b00'000'010;
	static const u8 LD_DE_A  = 0b00'010'010;
	static const u8 LD_HLI_A = 0b00'100'010;
	static const u8 LD_HLD_A = 0b00'110'010;

	// 16-bit transfer instructions
	static const u8 LD_DD_NN  = 0b00'000'001;
	static const u8 LD_SP_HL  = 0b11'111'001;
	static const u8 PUSH_QQ   = 0b11'000'101;
	static const u8 POP_QQ    = 0b11'000'001;
	static const u8 LDHL_SP_E = 0b11'111'000;
	static const u8 LD_NN_SP  = 0b00'001'000;

	// 8-bit arithmetic and logical operation instructions
	static const u8 ADD_A_R  = 0b10'000'000;
	static const u8 ADD_A_N  = 0b11'000'110;
	static const u8 ADD_A_HL = 0b10'000'110;
	static const u8 ADC_A_R  = 0b10'001'000;
	static const u8 ADC_A_N  = 0b11'001'110;
	static const u8 ADC_A_HL = 0b10'001'110;
	static const u8 SUB_R    = 0b10'010'000;
	static const u8 SUB_N    = 0b11'010'110;
	static const u8 SUB_HL   = 0b10'010'110;
	static const u8 SBC_A_R  = 0b10'011'000;
	static const u8 SBC_A_N  = 0b11'011'110;
	static const u8 SBC_A_HL = 0b10'011'110;
	static const u8 AND_R    = 0b10'100'000;
	static const u8 AND_N    = 0b11'100'110;
	static const u8 AND_HL   = 0b10'100'110;
	static const u8 OR_R     = 0b10'110'000;
	static const u8 OR_N     = 0b11'110'110;
	static const u8 OR_HL    = 0b10'110'110;
	static const u8 XOR_R    = 0b10'101'000;
	static const u8 XOR_N    = 0b11'101'110;
	static const u8 XOR_HL   = 0b10'101'110;
	static const u8 CP_R     = 0b10'111'000;
	static const u8 CP_N     = 0b11'111'110;
	static const u8 CP_HL    = 0b10'111'110;
	static const u8 INC_R    = 0b00'000'100;
	static const u8 INC_HL   = 0b00'110'100;
	static const u8 DEC_R    = 0b00'000'101;
	static const u8 DEC_HL   = 0b00'110'101;

	// 16-bit arithmetic operation instructions
	static const u8 ADD_HL_SS = 0b00'001'001;
	static const u8 ADD_SP_E  = 0b11'101'000;
	static const u8 INC_SS    = 0b00'000'011;
	static const u8 DEC_SS    = 0b00'001'011;

	// Rotate shift instructions
	static const u8 RLCA = 0b00'000'111;
	static const u8 RLA  = 0b00'010'111;
	static const u8 RRCA = 0b00'001'111;
	static const u8 RRA  = 0b00'011'111;

	static const u8 BIT_OP_1ST_BYTE = 0b11'001'011; // Also used for Bit operations
	static const u8 RLC_R   = 0b00'000'000;
	static const u8 RLC_HL  = 0b00'000'110;
	static const u8 RL_R    = 0b00'010'000;
	static const u8 RL_HL   = 0b00'010'110;
	static const u8 RRC_R   = 0b00'001'000;
	static const u8 RRC_HL  = 0b00'001'110;
	static const u8 RR_R    = 0b00'011'000;
	static const u8 RR_HL   = 0b00'011'110;
	static const u8 SLA_R   = 0b00'100'000;
	static const u8 SLA_HL  = 0b00'100'110;
	static const u8 SRA_R   = 0b00'101'000;
	static const u8 SRA_HL  = 0b00'101'110;
	static const u8 SRL_R   = 0b00'111'000;
	static const u8 SRL_HL  = 0b00'111'110;
	static const u8 SWAP_R  = 0b00'110'000;
	static const u8 SWAP_HL = 0b00'110'110;

	// Bit operations
	static const u8 BIT_B_R  = 0b01'000'000;
	static const u8 BIT_B_HL = 0b01'000'110;
	static const u8 SET_B_R  = 0b11'000'000;
	static const u8 SET_B_HL = 0b11'000'110;
	static const u8 RES_B_R  = 0b10'000'000;
	static const u8 RES_B_HL = 0b10'000'110;

	// Jump instructions
	static const u8 JP_NN    = 0b11'000'011;
	static const u8 JP_CC_NN = 0b11'000'010;
	static const u8 JR_E     = 0b00'011'000;
	static const u8 JR_CC_E  = 0b00'100'000;
	static const u8 JP_HL    = 0b11'101'001;

	// Call/return instructions
	static const u8 CALL_NN    = 0b11'001'101;
	static const u8 CALL_CC_NN = 0b11'000'100;
	static const u8 RET        = 0b11'001'001;
	static const u8 RETI       = 0b11'011'001;
	static const u8 RET_CC     = 0b11'000'000;
	static const u8 RST_T      = 0b11'000'111;

	// Gen-purpose arithmetic/CPU control instructions
	static const u8 DAA  = 0b00'100'111;
	static const u8 CPL  = 0b00'101'111;
	static const u8 NOP  = 0b00'000'000;
	static const u8 CCF  = 0b00'111'111;
	static const u8 SCF  = 0b00'110'111;
	static const u8 DI   = 0b11'110'011;
	static const u8 EI   = 0b11'111'011;
	static const u8 HALT = 0b01'110'110;
	static const u8 STOP = 0b00'010'000;

	const std::map<u8, std::string> INSTRUCTION_STRINGS = {
		{LD_R_R,   "LD r,r'"},
		{LD_R_N,   "LD r,n"},
		{LD_R_HL,  "LD r,(HL)"},
		{LD_HL_R,  "LD (HL),r"},
		{LD_HL_N,  "LD (HL),n"},
		{LD_A_BC,  "LD A,(BC)"},
		{LD_A_DE,  "LD A,(DE)"},
		{LD_A_C,   "LD A,(C)"},
		{LD_C_A,   "LD (C),A"},
		{LD_A_N,   "LD A,(n)"},
		{LD_N_A,   "LD (n),A"},
		{LD_A_NN,  "LD A,(nn)"},
		{LD_NN_A,  "LD (nn),A"},
		{LD_A_HLI, "LD A,(HLI)"},
		{LD_A_HLD, "LD A,(HLD)"},
		{LD_BC_A , "LD (BC),A"},
		{LD_DE_A , "LD (DE),A"},
		{LD_HLI_A, "LD (HLI),A"},
		{LD_HLD_A, "LD (HLD),A"},

		{LD_DD_NN , "LD dd,nn"},
		{LD_SP_HL , "LD SP,HL"},
		{PUSH_QQ  , "PUSH qq"},
		{POP_QQ   , "POP qq"},
		{LDHL_SP_E, "LDHL SP,e"},
		{LD_NN_SP , "LD (nn),SP"},

		{ADD_A_R , "ADD A,r"},
		{ADD_A_N , "ADD A,n"},
		{ADD_A_HL, "ADD A,(HL)"},
		{ADC_A_R , "ADC A,r"},
		{ADC_A_N , "ADC A,n"},
		{ADC_A_HL, "ADC A,(HL)"},
		{SUB_R   , "SUB r"},
		{SUB_N   , "SUB n"},
		{SUB_HL  , "SUB (HL)"},
		{SBC_A_R , "SBC A,r"},
		{SBC_A_N , "SBC A,n"},
		{SBC_A_HL, "SBC A,(HL)"},
		{AND_R   , "AND r"},
		{AND_N   , "AND n"},
		{AND_HL  , "AND (HL)"},
		{OR_R    , "OR r"},
		{OR_N    , "OR n"},
		{OR_HL   , "OR (HL)"},
		{XOR_R   , "XOR r"},
		{XOR_N   , "XOR n"},
		{XOR_HL  , "XOR (HL)"},
		{CP_R    , "CP r"},
		{CP_N    , "CP n"},
		{CP_HL   , "CP (HL)"},
		{INC_R   , "INC r"},
		{INC_HL  , "INC (HL)"},
		{DEC_R   , "DEC r"},
		{DEC_HL  , "DEC (HL)"},

		{ADD_HL_SS, "ADD HL,ss"},
		{ADD_SP_E , "ADD SP,e"},
		{INC_SS   , "INC ss"},
		{DEC_SS   , "DEC ss"},

		{RLCA   , "RLCA"},
		{RLA    , "RLA"},
		{RRCA   , "RRCA"},
		{RRA    , "RRA"},
		/*{RLC_R  , "RLC r"}, // Commented out strings all use the same opcode first byte, so they need to be handled separately
		{RLC_HL , "RLC (HL)"},
		{RL_R   , "RL r"},
		{RL_HL  , "RL (HL)"},
		{RRC_R  , "RRC r"},
		{RRC_HL , "RRC (HL)"},
		{RR_R   , "RR r"},
		{RR_HL  , "RR (HL)"},
		{SLA_R  , "SLA r"},
		{SLA_HL , "SLA (HL)"},
		{SRA_R  , "SRA r"},
		{SRA_HL , "SRA (HL)"},
		{SRL_R  , "SRL r"},
		{SRL_HL , "SRL (HL)"},
		{SWAP_R , "SWAP r"},
		{SWAP_HL, "SWAP (HL)"},

		{BIT_B_R , "BIT b,r"},
		{BIT_B_HL, "BIT b,(HL)"},
		{SET_B_R , "SET b,r"},
		{SET_B_HL, "SET b,(HL)"},
		{RES_B_R , "RES b,r"},
		{RES_B_HL, "RES b,(HL)"},*/

		{JP_NN,    "JP nn"},
		{JP_CC_NN, "JP cc,nn"},
		{JR_E,     "JR e"},
		{JR_CC_E,  "JR cc,e"},
		{JP_HL,    "JP (HL)"},

		{CALL_NN   , "CALL nn"},
		{CALL_CC_NN, "CALL cc,nn"},
		{RET       , "RET"},
		{RETI      , "RETI"},
		{RET_CC    , "RET cc"},
		{RST_T     , "RST t"},

		{DAA , "DAA"},
		{CPL , "CPL"},
		{NOP , "NOP"},
		{CCF , "CCF"},
		{SCF , "SCF"},
		{DI  , "DI"},
		{EI  , "EI"},
		{HALT, "HALT"},
		{STOP, "STOP"},
	};

	void TestForClashingMaskedInstruction()
	{
		u8 var = 0;
		switch (var)
		{
		case LD_R_R:
		case LD_R_N:
		case LD_R_HL:
		case LD_HL_R:
		case LD_HL_N:
		case LD_A_BC:
		case LD_A_DE:
		case LD_A_C:
		case LD_C_A:
		case LD_A_N:
		case LD_N_A:
		case LD_A_NN:
		case LD_NN_A:
		case LD_A_HLI:
		case LD_A_HLD:
		case LD_BC_A:
		case LD_DE_A:
		case LD_HLI_A:
		case LD_HLD_A:
		case LD_DD_NN:
		case LD_SP_HL:
		case PUSH_QQ:
		case POP_QQ:
		case LDHL_SP_E:
		case LD_NN_SP:
		case ADD_A_R:
		case ADD_A_N:
		case ADD_A_HL:
		case ADC_A_R:
		case ADC_A_N:
		case ADC_A_HL:
		case SUB_R:
		case SUB_N:
		case SUB_HL:
		case SBC_A_R:
		case SBC_A_N:
		case SBC_A_HL:
		case AND_R:
		case AND_N:
		case AND_HL:
		case OR_R:
		case OR_N:
		case OR_HL:
		case XOR_R:
		case XOR_N:
		case XOR_HL:
		case CP_R:
		case CP_N:
		case CP_HL:
		case INC_R:
		case INC_HL:
		case DEC_R:
		case DEC_HL:
		case ADD_HL_SS:
		case ADD_SP_E:
		case INC_SS:
		case DEC_SS:
		case RLCA:
		case RLA:
		case RRCA:
		case RRA:
		case BIT_OP_1ST_BYTE:
		case JP_NN:
		case JP_CC_NN:
		case JR_E:
		case JR_CC_E:
		case JP_HL:
		case CALL_NN:
		case CALL_CC_NN:
		case RET:
		case RETI:
		case RET_CC:
		case RST_T:
		case DAA:
		case CPL:
		case NOP:
		case CCF:
		case SCF:
		case DI:
		case EI:
		case HALT:
			return;
		};
	}
};



