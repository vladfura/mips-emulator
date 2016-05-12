#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <stdint.h>
#include <fstream>
using namespace std;

#define ADDU 97
#define ADDI 8
#define SUBU 99
#define OR 101
#define AND 100
#define ADD 96
#define SUB 98
#define SLT 106
#define BNE 5
#define BEQ 4
#define J 2
#define LB 32
#define SB 40
#define LH 33
#define SH 41
#define LW 35
#define SW 43

#define WRONGADDRESS 0xBFC00200

#define TLBL 2
#define TLBS 3

typedef uint32_t PhysAddress;
typedef uint32_t VirtAddress;
typedef uint32_t Word;
typedef uint16_t HalfWord;
typedef uint8_t Byte;
typedef unsigned PageFault;

struct Instruction
{
	public:
		bool isBranch();	
	struct RD_Shamt
		{
			uint8_t RD;
			uint8_t Shamt;
		}; 
	uint16_t opcode;
	uint8_t RS, RT;
	union 
	{
		uint32_t Imm;
		RD_Shamt rd_shamt;
	};
	private:
};

#endif // TYPES_H

