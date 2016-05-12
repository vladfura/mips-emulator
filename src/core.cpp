#include "../core.h"
#include "../types.h"
#include "../memory.h"
#include "../tlb.h"
#include "../cp0.h"
unsigned Core::ASID;

TlbRecord Core::pageFaultHandle(PageFault pf, VirtAddress va)	{
	VirtAddress BadVAddr = va;
	cpo.cause.ExcCode = pf;
	cpo.context.BadVPN2 = (va >> 13) & ((1 << (31 - 13 + 1)) - 1);
	
	cpo.entryHi.VPN2 = cpo.context.BadVPN2;
	cpo.entryHi.ASID = ASID;
	
	uint64_t offset = 0;
	
	if(cpo.status.EXL == 0)	{
		if(delaySlot)	{
			cpo.ePC.address = m_pc - 4;
			cpo.cause.BD = 1;
		}
		else   {
			cpo.ePC.address = m_pc;
			cpo.cause.BD = 0;
		}
			offset = 0;	
	}
	else {
		offset = 0x180;
	}
	
	cpo.cause.CE = 0;
	cpo.status.EXL = 1;
	
	m_pc = (cpo.status.BEV ? 0xBFC00200 : 0x80000000 ) + offset;
	
	PhysAddress phys;
	phys = BadVAddr;//Обработка исключений
	
	TlbRecord temp; //Восстановление контекста
	temp.VPN = BadVAddr;
	temp.ASID = cpo.entryHi.ASID;
	temp.PFN0 = phys;
	temp.V0 = 1;
	
	m_pc = cpo.ePC.address;
	
	return temp;
}

void Core :: write1(VirtAddress va, Byte data)	{
	PhysAddress phys;
	PageFault pf;
	
	phys = m_swTlb.translateWrite(va);
	
	if(phys == WRONGADDRESS)	{
		phys = m_hwTlb.translateWrite(va, pf);
		if(phys == WRONGADDRESS)	{
			
			TlbRecord temp;
			temp = pageFaultHandle(pf,va);
			
			temp.PageMaskWrite = 0;
			temp.D0 = 1;
			
			insertTlbRecord(temp);	
		}
		m_swTlb.insertWrite(va, phys);
	}
	m_mem.write1(phys, data);
}

void Core :: write2(VirtAddress va, HalfWord data)	{
	PhysAddress phys;
	PageFault pf;
	
	phys = m_swTlb.translateWrite(va);
	
	if(phys == WRONGADDRESS)	{
		phys = m_hwTlb.translateWrite(va, pf);
		if(phys == WRONGADDRESS)	{
			
			TlbRecord temp;
			temp = pageFaultHandle(pf,va);
			
			temp.PageMaskWrite = 0;
			temp.D0 = 1;
			
			insertTlbRecord(temp);	
		}
		m_swTlb.insertWrite(va, phys);
	}
	m_mem.write2(phys, data);
}

void Core :: write4(VirtAddress va, Word data)	{
	PhysAddress phys;
	PageFault pf;
	
	phys = m_swTlb.translateWrite(va);
	
	if(phys == WRONGADDRESS)	{
		phys = m_hwTlb.translateWrite(va, pf);
		if(phys == WRONGADDRESS)	{
			
			TlbRecord temp;
			temp = pageFaultHandle(pf,va);
			
			temp.PageMaskWrite = 0;
			temp.D0 = 1;
			
			insertTlbRecord(temp);	
		}
		m_swTlb.insertWrite(va, phys);
	}
	m_mem.write4(phys, data);
}

Byte Core :: read1(VirtAddress va)	{
	PhysAddress phys;
	PageFault pf;
	
	phys = m_swTlb.translateRead(va);
	
	if(phys == WRONGADDRESS)	{
		phys = m_hwTlb.translateRead(va, pf);
		if(phys == WRONGADDRESS)	{
			
			TlbRecord temp;
			temp = pageFaultHandle(pf,va);
			
			temp.PageMaskRead = 0;
			
			insertTlbRecord(temp);	
		}
		m_swTlb.insertRead(va, phys);
	}
	return m_mem.read1(phys);
}

HalfWord Core :: read2(VirtAddress va)	{
	PhysAddress phys;
	PageFault pf;
	
	phys = m_swTlb.translateRead(va);
	
	if(phys == WRONGADDRESS)	{
		phys = m_hwTlb.translateRead(va, pf);
		if(phys == WRONGADDRESS)	{
			
			TlbRecord temp;
			temp = pageFaultHandle(pf,va);
			
			temp.PageMaskRead = 0;
			
			insertTlbRecord(temp);	
		}
		m_swTlb.insertRead(va, phys);
	}
	return m_mem.read2(phys);
}

Word Core :: read4(VirtAddress va)	{
	PhysAddress phys;
	PageFault pf;	
	phys = m_swTlb.translateRead(va);	
	if(phys == WRONGADDRESS)	{
		phys = m_hwTlb.translateRead(va, pf);
		if(phys == WRONGADDRESS)	{	
			TlbRecord temp;
			temp = pageFaultHandle(pf,va);
			temp.PageMaskRead = 0;
			phys = temp.PFN0;
			
			insertTlbRecord(temp);	

			return m_mem.read4(phys);
		}
		m_swTlb.insertRead(va, phys);
	}
	return m_mem.read4(phys);
}



void Core::insertTlbRecord(TlbRecord record)	{
	m_hwTlb.m_records[cpo.index._Index] = record;
	
	cpo.index._Index++;
	if(cpo.index._Index == 32)
		cpo.index._Index = 0;

}

bool Instruction::isBranch()	{
	if(opcode == BNE || opcode == BEQ || opcode == J)
		return true;
	else return false;
}


void Core::preparation()	{
		
	union Command
{
	uint32_t int32_val;
	uint8_t  int8_val[4];
};	
	ifstream fs("example", ios::binary | ios::in);
	Command cmd;
	VirtAddress virt;
	bool signal = false;

	
	while(!fs.eof())	{
		
		if(!signal)	{

			fs >> std::hex;
			fs >> virt;

			
			signal = true;	
			
		}
		else   {

			fs >> std::hex;
			fs >> cmd.int32_val;
			
			for(int l = 0; l < 4; l++)
				m_mem.write1(virt + l,cmd.int8_val[3 - l]);	//Берем вирт и записываем


			signal = false;
		}
		
		
	}
	
	m_lastPc = virt;
	return ;
}

Instruction Core :: decode(Word word)	{
	Instruction temp;
	uint32_t code;
	code = word & 0xFC000000;
	
	
	if(code == 0)	{

		temp.opcode = (1 << 6) | (word & 0x3F); //Т.к. code == 0,для отличия опкодов делаем 1 в начале
		word = word >> 6;
		
		temp.rd_shamt.Shamt = word & 0x1F;
		
		word = word >> 5;
		
		temp.rd_shamt.RD = word & 0x1F;
		
		word = word >> 5;
		
		temp.RT = word & 0x1F;
		
		word = word >> 5;
		
		temp.RS = word & 0x1F;		
	}
	else if(code == 0x8000000 || code == 0xC000000)	{
		temp.opcode = code >>26;
		temp.Imm = word & 0x7FFFFFF;
		
		
	}	
	else   {

		temp.opcode = code >> 26;
		
		temp.Imm = word & 0xFFFF;
		
		word = word >> 16;
		
		temp.RT = word & 0x1F;
		
		word = word >> 5;
		
		temp.RS = word & 0x1F;		

	}
	
	
	return temp;
}

BasicBlock* Core::getNextBb(VirtAddress pc)	{ 
			delaySlot = false;
			BasicBlock* bb = m_insnCache.get(pc);

			if(bb->isValid() && bb->getTag() == pc) 
				return bb;
			for(int i = 0; i < 8; i++)	{
				Word word = read4(pc); 
				Instruction	insn = decode(word);
				if(i==7 && (insn.isBranch()))
					break;
				bb -> append(insn);
				if(delaySlot)	break;
				if(insn.isBranch())
					delaySlot = true;
				if(pc == m_lastPc)
					break;
							
				pc += 4;
			}
			bb->setValid(true);
			bb->setTag(pc - 4);
			
			m_insnCache.set(pc - 4, bb);
			return bb;			
}

Word Core::getReg(Byte Reg)
{
	return m_regs[Reg];
}

void Core::setReg(unsigned Reg, Word value)
{
	if(Reg == 0)
		writeZero = value;
	else
		m_regs[Reg] = value;
}

void Core::execute()	{

	BasicBlock* bb;

	cout << "m_lastPC = " << hex << m_lastPc << endl;
	
	while((m_lastPc + 4) != m_pc)	{	
	VirtAddress temp_pc = m_pc;
		bb = getNextBb(m_pc);
		m_pc = temp_pc;

		m_nextPc = m_pc + 4 * bb -> numberOfInsn();
	
	
		cout << bb->numberOfInsn() << endl;
	
		for(int i = 0; i < bb->numberOfInsn(); i++)	{
			Executor ex = executorsArray[bb -> insn[i].opcode];
			cout << "Hello World! " << i << endl;
			
			if(!ex(this, bb -> insn[i]))	{
				//Обработка исключений!
			}
		}
		cout << "Hello World6!" << endl;
	
		m_pc = m_nextPc;	
	}
	
		
	for(int i = 0; i < 32; i++)
		cout << "Значение регистра " << i << " равно " << this -> getReg(i) << endl;
	
}

Core::Core()	{
	m_regs[0] = 0;
	
	m_pc = 0x100;
	m_lastPc = m_pc;
	executorsArray[ADDU] = executorAddu;
	executorsArray[AND] = executorAnd;
	executorsArray[OR] = executorOr;
	executorsArray[SUBU] = executorSubu;
	executorsArray[SUB] = executorSub;
	executorsArray[ADD] = executorAdd;
	executorsArray[SLT] = executorSlt;
	executorsArray[BNE] = executorBne;
	executorsArray[BEQ] = executorBeq;
	executorsArray[J] = executorJ;
	executorsArray[LB] = executorLb;
	executorsArray[SB] = executorSb;
	executorsArray[ADDI] = executorAddi;
	executorsArray[LH] = executorLh;
	executorsArray[SH] = executorSh;
	executorsArray[LW] = executorLw;
	executorsArray[SW] = executorSw;
	
	
}
