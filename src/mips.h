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


class Core;
struct Instruction;
class TLB;
class Cache;
class BasicBlock;
class Memory;
class CPO;
class TlbRecord;
class SwTlbRecord;
class SwTlb;

typedef uint32_t PhysAddress;
typedef uint32_t VirtAddress;
typedef uint32_t Word;
typedef uint16_t HalfWord;
typedef uint8_t Byte;
typedef bool (*Executor)(Core*,Instruction);
typedef unsigned PageFault;


void showBits(uint8_t temp);

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

class SwTlbRecord	{
	public:
		VirtAddress virt;
		PhysAddress phys;
	private:
};

class SwTlb	{
	public:
		bool lookupWrite(VirtAddress va, PhysAddress &pa);
		bool lookupRead(VirtAddress va, PhysAddress &pa);
		
		void insertWrite(VirtAddress va, PhysAddress pa);
		void insertRead(VirtAddress va, PhysAddress pa);
		
		void invalidateAll();
		void invalidateWrite(VirtAddress va, int n);
		void invalidateRead(VirtAddress va, int n);
		
		PhysAddress translateWrite(VirtAddress va);
		PhysAddress translateRead(VirtAddress va);
		
		uint64_t m_valid_write;
		uint64_t m_valid_read;
		SwTlbRecord m_records[64];
};


class TlbRecord		{
	public:
		unsigned PageMaskWrite;
		unsigned PageMaskRead;
		unsigned VPN;
		unsigned G;
		unsigned ASID;
		unsigned PFN0;
		unsigned C0;
		unsigned D0;
		unsigned V0;
	
		unsigned PFN1;
		unsigned C1;
		unsigned D1;
		unsigned V1;
	
	
	private:	
};

class TLB	{
	
	public:
		PhysAddress translateWrite(VirtAddress va,PageFault &pf);
		PhysAddress translateRead(VirtAddress va,PageFault &pf);
		TlbRecord	m_records[32];
		
	
};

class Cache		{
	private:
		BasicBlock m_bb[8192];
	public:
		BasicBlock* get(VirtAddress addr);
		void set(VirtAddress addr,BasicBlock *bb);
		
};

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

class CPO	{
	public:
		struct Index	{
			unsigned P;
			//unsigned nil;
			unsigned _Index;
		} index; 
		
		struct Random	{
			unsigned P;
			unsigned nil;
			unsigned _Random;
		} random;
		
		struct EntryLo0	{
			unsigned PFN;
			unsigned D;
			unsigned V;
			unsigned G;
		} entryLo0;
		
		struct EntryLo1	{
			unsigned PFN;
			unsigned D;
			unsigned V;
			unsigned G;
		} entryLo1;
		
		struct Context	{
			unsigned BadVPN2;
		} context;
		
		struct PageMask	{
			unsigned mask;
		} pagemask;
		
		struct Wired	{
			unsigned wired;
		} wired;
		
		struct BadVAddr	{
			unsigned adress;
		} badVAddr;
		
		struct EntryHI	{
			unsigned VPN2;
			unsigned ASID;	
		} entryHi;
		
		struct Status	{
			unsigned BEV;
			unsigned TS;
			unsigned SR;
			unsigned NMI;
			unsigned ERL;
			unsigned EXL;
			unsigned IE;			
		} status;
		
		struct Cause	{
			unsigned BD;
			unsigned CE;
			unsigned ExcCode;

		} cause;
		
		struct EPC	{
			unsigned address;
			
		} ePC;
		
		struct ErrorEPC	{
			unsigned address;			
		} errorEPC;
	
	CPO()	{
		index._Index = 0;
	}
	
	private:
};


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




bool Instruction::isBranch()	{
	if(opcode == BNE || opcode == BEQ || opcode == J)
		return true;
	else return false;
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


PhysAddress SwTlb :: translateWrite(VirtAddress va)	{
	PhysAddress phys;
	if(lookupWrite(va, phys))
		return phys;
	else	
		return WRONGADDRESS;
}

PhysAddress SwTlb :: translateRead(VirtAddress va)	{
	PhysAddress phys;
	if(lookupRead(va, phys))
		return phys;
	else	
		return WRONGADDRESS;
}

PhysAddress TLB :: translateWrite(VirtAddress va,PageFault &pf)	{
	for(int i = 0; i < 32; i++)	{
		if(m_records[i].VPN == ((~m_records[i].PageMaskWrite) & (va & ~0x1FFF)))	{
			VirtAddress s = va & 0x1000;
			if((m_records[i].G == 0	&& m_records[i].ASID == Core::ASID) || m_records[i].G == 1)	{
				if(s == 0)	{	
					if(m_records[i].V0 == 1 && m_records[i].D0 == 1)
								return m_records[i].PFN0;
				}	else	if(m_records[i].V1 == 1 && m_records[i].D1 == 1)	{	
						return m_records[i].PFN1;	
				}
			}
		}
	}
	
	pf = TLBS;
	return WRONGADDRESS;
	
}


PhysAddress TLB :: translateRead(VirtAddress va,PageFault &pf)	{
		for(int i = 0; i < 32; i++)	{
			if(m_records[i].VPN == ((~m_records[i].PageMaskRead) & (va & ~0x1FFF)))	{
				VirtAddress s = va & 0x1000;
				if((m_records[i].G == 0	&& m_records[i].ASID == Core::ASID) || m_records[i].G == 1)	{
					if(s == 0)	{
						if(m_records[i].V0 == 1)	{
							return m_records[i].PFN0;
						}	else   if(m_records[i].V1 == 1)	{	
							return m_records[i].PFN1;	
						}
					}
						
				}
			}
		}
		
		pf = TLBL;
		return WRONGADDRESS;
	
}


bool SwTlb:: lookupWrite(VirtAddress va, PhysAddress &pa)	{
	size_t num = (va >> 12) & (0x3F);
	pa = m_records[num].phys;
	return m_valid_write & (1ul << num) && (m_records[num].virt == (va & 0xFFFFF000) );
}

bool SwTlb:: lookupRead(VirtAddress va, PhysAddress &pa)	{
	size_t num = (va >> 12) & (0x3F);
	pa = m_records[num].phys;
	return m_valid_read & (1ul << num) && (m_records[num].virt == (va & 0xFFFFF000) );
}


void SwTlb:: insertWrite(VirtAddress va, PhysAddress pa)	{
	size_t num = (va >> 12) & 0x3F;
	m_valid_write = m_valid_write | (1ul << num);
	m_records[num].virt = (va & 0xFFFFF000);
	m_records[num].phys = pa & 0xFFFFF000;
	return ;
}

void SwTlb:: insertRead(VirtAddress va, PhysAddress pa)	{
	size_t num = (va >> 12) & 0x3F;
	m_valid_read = m_valid_read | (1ul << num);
	m_records[num].virt = (va & 0xFFFFF000);
	m_records[num].phys = pa & 0xFFFFF000;
	return ;
}


void SwTlb:: invalidateAll() {
	m_valid_write = m_valid_read = 0;
}

void SwTlb :: invalidateWrite(VirtAddress va, int n)	{
	size_t num = (va >> 12) & 0x3F;
	
	if( n > 16 )	{
		m_valid_write = 0;
		return;
	}
	
	uint64_t mask;
	mask = (1ul << n) - 1;
	m_valid_write = m_valid_write & (~(mask << num));
	return; 	
}

void SwTlb :: invalidateRead(VirtAddress va, int n)	{
	size_t num = (va >> 12) & 0x3F;
	
	if( n > 16 )	{
		m_valid_read = 0;
		return;
	}
	
	uint64_t mask;
	mask = (1ul << n) - 1;
	m_valid_read = m_valid_read & (~(mask << num));
	return; 	
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

void showBits(uint8_t temp)	{
	for(int i = 0; i < 8; i++)	{
		if(temp & ((1 << 7) >> i))
			cout << "1 ";
		else
			cout << "0 ";	
	}
	cout << endl;
	
}
