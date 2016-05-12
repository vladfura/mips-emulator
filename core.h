#ifndef CORE_H
#define CORE_H

#include "types.h"
#include "memory.h"
#include "tlb.h"
#include "executor.h"
#include "cp0.h"

#include <iostream>

class Core
{
private:
	
	Word m_regs[32];
	Word writeZero;
	VirtAddress m_pc;
	Cache m_insnCache;
	Executor executorsArray[127];
	CPO cpo;
	bool delaySlot;

public:
	Memory m_mem; 
	VirtAddress m_nextPc,m_lastPc;
	Core();
	Word getReg (Byte Reg);
	void setReg (unsigned Reg, Word value);
	Instruction decode(Word word);
	BasicBlock* getNextBb(VirtAddress pc);
	void execute();
	void preparation();
	void write1(VirtAddress va, Byte data);
	void write2(VirtAddress va, HalfWord data);
	void write4(VirtAddress va, Word data);
	Byte read1(VirtAddress va);
	HalfWord read2(VirtAddress va);
	Word read4(VirtAddress va);
	
	TlbRecord pageFaultHandle(PageFault pf, VirtAddress va);
	static unsigned ASID;
	SwTlb m_swTlb;
	TLB m_hwTlb;
	void insertTlbRecord(TlbRecord record);
	
};

#endif //CORE_H
