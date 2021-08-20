#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED
#pragma once
#include "common.h"

struct CpuidRegs
{
	uint32_t eax{0};
	uint32_t ebx{0};
	uint32_t ecx{0};
	uint32_t edx{0};
};

class Cpu
{
public:
	enum Features {
		CPUID_FEAT_ECX_SSE3    = 1 << 0, 
		CPUID_FEAT_ECX_PCLMUL  = 1 << 1,
		CPUID_FEAT_ECX_DTES64  = 1 << 2,
		CPUID_FEAT_ECX_MONITOR = 1 << 3,  
		CPUID_FEAT_ECX_DS_CPL  = 1 << 4,  
		CPUID_FEAT_ECX_VMX     = 1 << 5,  
		CPUID_FEAT_ECX_SMX     = 1 << 6,  
		CPUID_FEAT_ECX_EST     = 1 << 7,  
		CPUID_FEAT_ECX_TM2     = 1 << 8,  
		CPUID_FEAT_ECX_SSSE3   = 1 << 9,  
		CPUID_FEAT_ECX_CID     = 1 << 10,
		CPUID_FEAT_ECX_SDBG    = 1 << 11,
		CPUID_FEAT_ECX_FMA     = 1 << 12,
		CPUID_FEAT_ECX_CX16    = 1 << 13, 
		CPUID_FEAT_ECX_ETPRD   = 1 << 14, 
		CPUID_FEAT_ECX_PDCM    = 1 << 15, 
		CPUID_FEAT_ECX_PCIDE   = 1 << 17, 
		CPUID_FEAT_ECX_DCA     = 1 << 18, 
		CPUID_FEAT_ECX_SSE4_1  = 1 << 19, 
		CPUID_FEAT_ECX_SSE4_2  = 1 << 20, 
		CPUID_FEAT_ECX_x2APIC  = 1 << 21, 
		CPUID_FEAT_ECX_MOVBE   = 1 << 22, 
		CPUID_FEAT_ECX_POPCNT  = 1 << 23, 
		CPUID_FEAT_ECX_TCSD    = 1 << 24, 
		CPUID_FEAT_ECX_AES     = 1 << 25, 
		CPUID_FEAT_ECX_XSAVE   = 1 << 26, 
		CPUID_FEAT_ECX_OSXSAVE = 1 << 27, 
		CPUID_FEAT_ECX_AVX     = 1 << 28,
		CPUID_FEAT_ECX_F16C    = 1 << 29,
		CPUID_FEAT_ECX_RDRAND  = 1 << 30,
 
		CPUID_FEAT_EDX_FPU     = 1 << 0,  
		CPUID_FEAT_EDX_VME     = 1 << 1,  
		CPUID_FEAT_EDX_DE      = 1 << 2,  
		CPUID_FEAT_EDX_PSE     = 1 << 3,  
		CPUID_FEAT_EDX_TSC     = 1 << 4,  
		CPUID_FEAT_EDX_MSR     = 1 << 5,  
		CPUID_FEAT_EDX_PAE     = 1 << 6,  
		CPUID_FEAT_EDX_MCE     = 1 << 7,  
		CPUID_FEAT_EDX_CX8     = 1 << 8,  
		CPUID_FEAT_EDX_APIC    = 1 << 9,  
		CPUID_FEAT_EDX_SEP     = 1 << 11, 
		CPUID_FEAT_EDX_MTRR    = 1 << 12, 
		CPUID_FEAT_EDX_PGE     = 1 << 13, 
		CPUID_FEAT_EDX_MCA     = 1 << 14, 
		CPUID_FEAT_EDX_CMOV    = 1 << 15, 
		CPUID_FEAT_EDX_PAT     = 1 << 16, 
		CPUID_FEAT_EDX_PSE36   = 1 << 17, 
		CPUID_FEAT_EDX_PSN     = 1 << 18, 
		CPUID_FEAT_EDX_CLF     = 1 << 19, 
		CPUID_FEAT_EDX_DTES    = 1 << 21, 
		CPUID_FEAT_EDX_ACPI    = 1 << 22, 
		CPUID_FEAT_EDX_MMX     = 1 << 23, 
		CPUID_FEAT_EDX_FXSR    = 1 << 24, 
		CPUID_FEAT_EDX_SSE     = 1 << 25, 
		CPUID_FEAT_EDX_SSE2    = 1 << 26, 
		CPUID_FEAT_EDX_SS      = 1 << 27, 
		CPUID_FEAT_EDX_HTT     = 1 << 28, 
		CPUID_FEAT_EDX_TM1     = 1 << 29, 
		CPUID_FEAT_EDX_IA64    = 1 << 30,
		CPUID_FEAT_EDX_PBE     = 1 << 31
	};
public:
	Cpu();
	const char *VendorId()const;
	uint32_t    MaxRequestLevel()const { return maxRequestLevel; }
	uint32_t model()const;
	uint32_t family()const;
	uint32_t stepping()const;
	uint32_t processorType()const;
	uint32_t extendedModel()const;
	uint32_t extendedFamily()const;
	
	bool SSE3()const;
	bool PCLMULQDQ()const;
	bool DTES64()const;
	bool MONITOR()const;
	bool DS_CPL()const;
	bool VMX()const;
	bool SMX()const;
	bool EIST()const;
	bool TM2()const;
	bool SSSE3()const;
	bool CNXT_ID()const;
	bool SDBG()const;
	bool FMA()const;
	bool FCMPXCHG16BMA()const;
	bool TPRUPDATE()const;
	bool PDCM()const;
	bool Reserved16()const;
	bool PCID()const;
	bool DCA()const;
	bool SSE41()const;
	bool SSE42()const;
	bool X2APIC()const;
	bool MOVBE()const;
	bool POPCNT()const;
	bool TCSDEADLINE()const;
	bool AESNI()const;
	bool XSAVE()const;
	bool OSXSAVE()const;
	bool AVX()const;
	bool F16C()const;
	bool RDRAND()const;
	bool Unused31()const;
	
	
	bool FPU()const;
	bool VME()const;
	bool DE()const;
	bool PSE()const;
	bool TSC()const;
	bool MSR()const;
	bool PAE()const;
	bool MCE()const;
	bool CX8()const;
	bool APIC()const;
	bool Reserved10()const;
	bool SEP()const;
	bool MTRR()const;
	bool PGE()const;
	bool MCA()const;
	bool CMOV()const;
	bool PAT()const;
	bool PSE36()const;
	bool PSN()const;
	bool CLFSH()const;
	bool Reserved20()const;
	bool DS()const;
	bool ACPI()const;
	bool MMX()const;
	bool FXSR()const;
	bool SSE()const;
	bool SSE2()const;
	bool SS()const;
	bool HTT()const;
	bool TM()const;
	bool IA64()const;
	bool PBE()const;
	
private:
	void		getLevel0Info();
	void		getLevel1Info();
	void		getLevel2Info();
private:

	uint32_t    maxRequestLevel {0};
	uint32_t	modelFamilyStepping {0};
	uint32_t	brandCflushLocapApic{0};
	uint32_t	cpuFeatures[2] {0, 0};
	CpuidRegs	regsTlbCachePrefetch {};
	char        vendorId[13] = {'\0'};
};


#endif // CPU_H_INCLUDED

