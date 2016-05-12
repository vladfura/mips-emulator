#include "../core.h"
#include "../types.h"
#include "../memory.h"
#include "../tlb.h"
#include "../cp0.h"
bool executorAddi (Core* core, Instruction insn)
{
	cout << "Привет Addi" << endl;
	
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word im = insn.Imm;
	
	rt = rs + im;
	
	core->setReg(insn.RT, rt);
	
	return true;
}

bool executorAddu (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word rd = rs + rt;
	core->setReg(insn.rd_shamt.RD, rd);
	return true;
}

bool executorAdd (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word rd = rs + rt;
	
	cout << "Привет Add!" << endl;
	if( (insn.RS  & (1 << 31)) == (insn.RT & (1 << 31)) )
		if( ~(insn.RS & (1 << 31)) == (insn.rd_shamt.RD & (1 << 31)) )
			return false;
	
	core->setReg(insn.rd_shamt.RD, rd);

	cout << "Значение 6-го регистра равно " <<core -> getReg(6) << endl;
	
	return true;
}

bool executorSubu (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word rd = rs - rt;
	core->setReg(insn.rd_shamt.RD, rd);
	return true;
}

bool executorSub (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word rd = rs - rt;
	
	cout << "Привет SUB" << endl;
	
	if( (insn.RS  & (1 << 31)) != (insn.RT & (1 << 31)) )
		if( ~(insn.RS & (1 << 31)) == (insn.rd_shamt.RD & (1 << 31)) )
			return false;
	
	core->setReg(insn.rd_shamt.RD, rd);
	return true;
}

bool executorSlt (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word rd = rs < rt;
	
	core->setReg(insn.rd_shamt.RD, rd);
	return true;
}

bool executorBne (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word im = insn.Imm;
	
	cout << "Привет BNE " << endl;
	
	if(rs != rt)
		core -> m_nextPc = core -> m_nextPc + 4 * im;
	
	return true;
}

bool executorBeq (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word im = insn.Imm;
	
	cout << "Привет BEQ" << endl;
	
	if(rs == rt)
		core -> m_nextPc = core -> m_nextPc + 4 * im;
	
	return true;
}

bool executorJ (Core* core, Instruction insn)
{
	Word im = insn.Imm;
	
	cout << "Привет J " << endl;
	core -> m_nextPc = im;
	
	return true;
}

bool executorLb (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Byte rt = insn.RT;
	Word im = insn.Imm;
		
	core -> setReg(rt , core -> m_mem.read1(rs + im));

	cout << "Привет Lb" << endl;

	
	return true;
}

bool executorLh (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	HalfWord rt = insn.RT;
	Word im = insn.Imm;
	
	core -> setReg(rt , core -> m_mem.read2(rs + im));

	cout << "Привет Lh" << endl;
	
	return true;
}

bool executorLw (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = insn.RT;
	Word im = insn.Imm;
	
	core -> setReg(rt , core -> m_mem.read4(rs + im));

	cout << "Привет Lw" << endl;
	
	return true;
}

bool executorSb (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word im = insn.Imm;
	
	cout << "Привет Sb" << endl;

	core -> m_mem.write1(rs + im, rt);

	return true;
}

bool executorSh (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word im = insn.Imm;
	
	cout << "Привет Sh" << endl;
	
	core -> m_mem.write2(rs + im, rt);	
	
	return true;
}

bool executorSw (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word im = insn.Imm;
	
	cout << "Привет Sw" << endl;
	
	core -> m_mem.write4(rs + im, rt);	
	
	return true;
}


bool executorOr (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word rd = rs | rt;
	core->setReg(insn.rd_shamt.RD, rd);
	return true;
}

bool executorAnd (Core* core, Instruction insn)
{
	Word rs = core->getReg(insn.RS);
	Word rt = core->getReg(insn.RT);
	Word rd = rs & rt;
	
	cout << "Привет And" << endl;
	
	core->setReg(insn.rd_shamt.RD, rd);

	
	cout << "Пока and" << endl;
	return true;
}

