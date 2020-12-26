#include "cpu.h"
#include "common.h"

/* Vendor-strings. */
#define CPUID_VENDOR_OLDAMD       "AMDisbetter!" /* early engineering samples of AMD K5 processor */
#define CPUID_VENDOR_AMD          "AuthenticAMD"
#define CPUID_VENDOR_INTEL        "GenuineIntel"
#define CPUID_VENDOR_VIA          "CentaurHauls"
#define CPUID_VENDOR_OLDTRANSMETA "TransmetaCPU"
#define CPUID_VENDOR_TRANSMETA    "GenuineTMx86"
#define CPUID_VENDOR_CYRIX        "CyrixInstead"
#define CPUID_VENDOR_CENTAUR      "CentaurHauls"
#define CPUID_VENDOR_NEXGEN       "NexGenDriven"
#define CPUID_VENDOR_UMC          "UMC UMC UMC "
#define CPUID_VENDOR_SIS          "SiS SiS SiS "
#define CPUID_VENDOR_NSC          "Geode by NSC"
#define CPUID_VENDOR_RISE         "RiseRiseRise"
#define CPUID_VENDOR_VORTEX       "Vortex86 SoC"
// #define CPUID_VENDOR_VIA          "VIA VIA VIA "
 
/*Vendor-strings from Virtual Machines.*/
#define CPUID_VENDOR_VMWARE       "VMwareVMware"
#define CPUID_VENDOR_XENHVM       "XenVMMXenVMM"
#define CPUID_VENDOR_MICROSOFT_HV "Microsoft Hv"
#define CPUID_VENDOR_PARALLELS    " lrpepyh vr"


extern "C"
{
	uint32_t Cpuid(CpuidRegs *pRegs);
}

Cpu::Cpu()
{
	getLevel0Info();
	getLevel1Info();
	getLevel2Info();
}

void Cpu::getLevel0Info()
{
	CpuidRegs regs {};
	regs.eax = 0;
	maxRequestLevel = Cpuid(&regs);
	uint32_t *ptr = reinterpret_cast<uint32_t *>(vendorId);
	ptr[0] = regs.ebx;
	ptr[1] = regs.ecx;
	ptr[2] = regs.edx;
}

void Cpu::getLevel1Info()
{
	CpuidRegs regs {};
	regs.eax = 1;
	modelFamilyStepping = Cpuid(&regs);
	brandCflushLocapApic = regs.ebx;
	cpuFeatures[0] = regs.ecx;
	cpuFeatures[1] = regs.edx;
}

void Cpu::getLevel2Info()
{
	regsTlbCachePrefetch.eax = 2;
	regsTlbCachePrefetch.eax = Cpuid(&regsTlbCachePrefetch);
}

const char *Cpu::VendorId()const
{
	return vendorId;
}

uint32_t Cpu::model()const
{
	constexpr uint32_t MODEL_MASK = 0x000000F0;
	return ((modelFamilyStepping & MODEL_MASK) >> 4);
}

uint32_t Cpu::family()const
{
	constexpr uint32_t FAMILY_MASK = 0x00000F00;
	return ((modelFamilyStepping & FAMILY_MASK) >> 8);
}

uint32_t Cpu::stepping()const
{
	constexpr uint32_t STEPPING_MASK = 0x0000000F;
	return (modelFamilyStepping & STEPPING_MASK);
}

uint32_t Cpu::processorType()const
{
	constexpr uint32_t TYPE_MASK = 0x00003000;
	return ((modelFamilyStepping & TYPE_MASK)>>12);
}

uint32_t Cpu::extendedModel()const
{
	constexpr uint32_t EXTENDED_MASK = 0x000F0000;
	return ((modelFamilyStepping & EXTENDED_MASK)>>16);
}

uint32_t Cpu::extendedFamily()const
{
	constexpr uint32_t EXTENDED_FAMILY_MASK = 0x0FF00000;
	return ((modelFamilyStepping & EXTENDED_FAMILY_MASK)>>20);
}

bool Cpu::SSE3()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_SSE3) != 0);
}

bool Cpu::PCLMULQDQ()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_PCLMUL) != 0);
}

bool Cpu::DTES64()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_DTES64) != 0);
}

bool Cpu::MONITOR()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_MONITOR) != 0);
}

bool Cpu::DS_CPL()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_DS_CPL) != 0);
}

bool Cpu::VMX()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_VMX) != 0);
}

bool Cpu::SMX()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_SMX) != 0);
}

bool Cpu::EIST()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_EST) != 0);
}

bool Cpu::TM2()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_TM2) != 0);
}

bool Cpu::SSSE3()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_SSSE3) != 0);
}

bool Cpu::CNXT_ID()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_CID) != 0);
}

bool Cpu::SDBG()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_SDBG) != 0);
}

bool Cpu::FMA()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_FMA) != 0);
}

bool Cpu::FCMPXCHG16BMA()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_CX16) != 0);
}

bool Cpu::TPRUPDATE()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_ETPRD) != 0);
}

bool Cpu::PDCM()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_PDCM) != 0);
}

bool Cpu::Reserved16()const
{
	return ((cpuFeatures[0] & (1<<16)) != 0);
}

bool Cpu::PCID()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_PCIDE) != 0);
}

bool Cpu::DCA()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_DCA) != 0);
}

bool Cpu::SSE41()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_SSE4_1) != 0);
}

bool Cpu::SSE42()const
{

	return ((cpuFeatures[0] & CPUID_FEAT_ECX_SSE4_2) != 0);
}

bool Cpu::X2APIC()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_x2APIC) != 0);
}

bool Cpu::MOVBE()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_MOVBE) != 0);
}

bool Cpu::POPCNT()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_POPCNT) != 0);
}

bool Cpu::TCSDEADLINE()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_TCSD) != 0);
}

bool Cpu::AESNI()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_AES) != 0);
}

bool Cpu::XSAVE()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_XSAVE) != 0);
}

bool Cpu::OSXSAVE()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_OSXSAVE) != 0);
}

bool Cpu::AVX()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_AVX) != 0);
}

bool Cpu::F16C()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_F16C) != 0);
}

bool Cpu::RDRAND()const
{
	return ((cpuFeatures[0] & CPUID_FEAT_ECX_RDRAND) != 0);
}

bool Cpu::Unused31()const
{
	return ((cpuFeatures[0] & (1<<31)) != 0);
}

bool Cpu::FPU()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_FPU) != 0);
}

bool Cpu::VME()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_VME) != 0);
}

bool Cpu::DE()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_DE) != 0);
}

bool Cpu::PSE()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_PSE) != 0);
}

bool Cpu::TSC()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_TSC) != 0);
}

bool Cpu::MSR()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_MSR) != 0);
}

bool Cpu::PAE()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_PAE) != 0);
}

bool Cpu::MCE()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_MCE) != 0);
}

bool Cpu::CX8()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_CX8) != 0);
}

bool Cpu::APIC()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_APIC) != 0);
}

bool Cpu::Reserved10()const
{
	return ((cpuFeatures[1] & (1<<10)) != 0);
}

bool Cpu::SEP()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_SEP) != 0);
}

bool Cpu::MTRR()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_MTRR) != 0);
}

bool Cpu::PGE()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_PGE) != 0);
}

bool Cpu::MCA()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_MCA) != 0);
}

bool Cpu::CMOV()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_CMOV) != 0);
}

bool Cpu::PAT()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_PAT) != 0);
}

bool Cpu::PSE36()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_PSE36) != 0);
}

bool Cpu::PSN()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_PSN) != 0);
}

bool Cpu::CLFSH()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_CLF) != 0);
}

bool Cpu::Reserved20()const
{
	return ((cpuFeatures[1] & (1<<20)) != 0);
}

bool Cpu::DS()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_DTES) != 0);
}

bool Cpu::ACPI()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_ACPI) != 0);
}

bool Cpu::MMX()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_MMX) != 0);
}

bool Cpu::FXSR()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_FXSR) != 0);
}

bool Cpu::SSE()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_SSE) != 0);
}

bool Cpu::SSE2()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_SSE2) != 0);
}

bool Cpu::SS()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_SS) != 0);
}

bool Cpu::HTT()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_HTT) != 0);
}

bool Cpu::TM()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_TM1) != 0);
}

bool Cpu::IA64()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_IA64) != 0);
}

bool Cpu::PBE()const
{
	return ((cpuFeatures[1] & CPUID_FEAT_EDX_PBE) != 0);
}

