#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <vector>
#include "types.h"

void showBits(uint8_t temp);

class Memory	{
	public:
	Byte		read1(PhysAddress addr);
	HalfWord	read2(PhysAddress addr);
	Word		read4(PhysAddress addr);
	void write1(PhysAddress addr,Byte wd);
	void write2(PhysAddress addr,HalfWord wd);
	void write4(PhysAddress addr, Word wd);
	private:
		Byte memory[262144];
};

class BasicBlock		{
	private:

		int i;
		bool valid;
		VirtAddress tag;
	public:
		Instruction insn[8];
		BasicBlock();
		void append(Instruction insn);
		bool isValid();
		void setValid(bool valid);
		VirtAddress getTag();
		void setTag(VirtAddress addr);
		int numberOfInsn();
};



class Cache		{
	private:
		BasicBlock m_bb[8192];
	public:
		BasicBlock* get(VirtAddress addr);
		void set(VirtAddress addr,BasicBlock *bb);
		
};


#endif // MEMORY_H
