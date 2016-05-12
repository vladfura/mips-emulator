#ifndef TLB_H
#define TLB_H

#include "types.h"

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

#endif //TLB_H
