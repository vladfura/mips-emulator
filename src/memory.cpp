#include "../core.h"
#include "../types.h"
#include "../memory.h"
#include "../tlb.h"
#include "../cp0.h"

VirtAddress BasicBlock::getTag()	{

	return tag;	
}

void BasicBlock::setTag(VirtAddress addr)	{
	tag = addr;
	return;
}

void BasicBlock::setValid(bool valid)	{
	this->valid = valid;
}

BasicBlock::BasicBlock()  : i(0),valid(false)	{	
}

void BasicBlock::append(Instruction insn)	{
	this->insn[i] = insn;
	i++;
}

bool BasicBlock::isValid()	{
	return valid;
}

int BasicBlock::numberOfInsn()	{
	return i;
}

Byte	Memory::read1(PhysAddress addr)	{
	return memory[addr];
}

HalfWord Memory::read2(PhysAddress addr)	{
	HalfWord tmp;
	tmp = (memory[addr] << 8) | (memory[addr+1]);
	return tmp;
}

Word Memory::read4(PhysAddress addr)	{
	Word tmp;	
	tmp = (memory[addr] << 24) | (memory[addr+1] << 16) | (memory[addr+2] << 8) | (memory[addr+3]);	
	return tmp;
}

void Memory::write1(PhysAddress addr,Byte wd)	{
	memory[addr] = wd;	
}

void Memory::write2(PhysAddress addr,HalfWord wd)	{
	
	memory[addr] = (Byte)(wd >> 8);
	memory[addr+1] = (Byte)wd;

	
}

void Memory::write4(PhysAddress addr,Word wd)	{

	memory[addr] = (Byte)(wd >> 24);
	memory[addr+1] = (Byte)(wd >> 16);
	memory[addr+2] = (Byte)(wd >> 8);
	memory[addr+3] = (Byte)wd;
	
}

BasicBlock* Cache::get(VirtAddress addr)	{
	int id;
	id = (addr >> 2) & (8192 - 1);
	
	return &(m_bb[id]);
}

void Cache::set(VirtAddress addr,BasicBlock *bb)	{
	int id;
	id = (addr >> 2) & (8192 - 1);
	m_bb[id] = *bb;
	return;	
}

void showBits(uint8_t temp)	{
	for(int i = 0; i < 8; i++)	{
		if(temp & ((1 << 7) >> i))
			cout << "1 ";
		else
			cout << "0 ";	
	}
	cout << endl;
	
}
