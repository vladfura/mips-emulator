#include "../core.h"
#include "../types.h"
#include "../memory.h"
#include "../tlb.h"
#include "../cp0.h"

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

