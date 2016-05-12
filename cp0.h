#ifndef CP0_H
#define CP0_H

#include "types.h"

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

#endif //CP0_H
