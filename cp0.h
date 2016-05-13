#ifndef CP0_H
#define CP0_H

#define N 10

#include "types.h"


class CPO	{
	public:
		struct Index	{
			unsigned P:1;
			unsigned nil:(31-N);
			unsigned _Index:N;
		} index; 
		
		struct Random	{
			//unsigned P;
			unsigned nil:(32-N);
			unsigned _Random:N;
		} random;
		
		struct EntryLo0	{
			unsigned nil:2;
			unsigned PFN:24;
			unsigned C:3;
			unsigned D:1;
			unsigned V:1;
			unsigned G:1;
		} entryLo0;
		
		struct EntryLo1	{
			unsigned nil:2;
			unsigned PFN:24;
			unsigned C:3;
			unsigned D:1;
			unsigned V:1;
			unsigned G:1;
		} entryLo1;
		
		struct Context	{
			unsigned nil:4;
			unsigned BadVPN2:19;
			unsigned PTEBase:9;
		} context;
		
		struct PageMask	{
			unsigned nil0:13;
			unsigned mask:16;
			unsigned nil1:3;
		} pagemask;
		
		struct Wired	{
			unsigned wired:N;
			unsigned nil:(32-N);
		} wired;
		
		struct BadVAddr	{
			unsigned adress;
		} badVAddr;
		
		struct Count {
			unsigned address;
		} count;
		
		struct EntryHI	{
			unsigned ASID:8;
			unsigned nil:5;
			unsigned VPN2:19;	
		} entryHi;
		
		struct Compare {
			unsigned compare;
		} compare;
		
		struct Status	{
			unsigned CU3:1;
			unsigned CU2:1;
			unsigned CU1:1;
			unsigned CU0:1;
			unsigned RP:1;
			unsigned FR:1;
			unsigned RE:1;
			unsigned MX:1;
			unsigned PX:1;
			unsigned BEV:1;
			unsigned TS:1;
			unsigned SR:1;
			unsigned NMI:1;
			unsigned nil:1;
			unsigned Impl:1;
			unsigned IM7:1;
			unsigned IM6:1;
			unsigned IM5:1;
			unsigned IM4:1;
			unsigned IM3:1;
			unsigned IM2:1;
			unsigned IM1:1;
			unsigned IM0:1;
			unsigned KX:1;
			unsigned SX:1;
			unsigned UX:1;
				unsigned UM:1; // KSU
				unsigned R0:1; 
			unsigned ERL:1;
			unsigned EXL:1;
			unsigned IE:1;			
		} status;
		
		struct Cause	{
			unsigned BD:1;
			unsigned nil4:1;
			unsigned CE:2;
			unsigned nil3:4;
			unsigned IV:1;
			unsigned WP:1;
			unsigned nul2:6;
			unsigned IP7:1;
			unsigned IP6:1;
			unsigned IP5:1;
			unsigned IP4:1;
			unsigned IP3:1;
			unsigned IP2:1;
			unsigned IP1:1;
			unsigned IP0:1;
			unsigned nil1:1;
			unsigned ExcCode:5;
			unsigned nil0:2;

		} cause;
		
		struct EPC	{
			unsigned address;
			
		} ePC;
//...............................................		
		struct ErrorEPC	{
			unsigned address;			
		} errorEPC;
	
	CPO()	{
		index._Index = 0;
	}
	
	private:
};

#endif //CP0_H
