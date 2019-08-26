#include "common.h"
#include "cpuinfo.h"


enum 
{
    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_ETPRD        = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_PCIDE        = 1 << 17, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_x2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_AES          = 1 << 25, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
 
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLF          = 1 << 19, 
    CPUID_FEAT_EDX_DTES         = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM1          = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
};

inline
uint32_t test(uint32_t reg, const uint32_t test)
{
    return ((reg & test) ? 1 : 0);
}

const char *
getBrandString( uint32_t index)
{
    switch(index)
    {
        case 0x00:
            return "This processor does not support the brand identification feature";
        case 0x01:
            return "Intel(R) Celeron(R) processor";
        case 0x02:
            return "Intel(R) Pentium(R) III processor";
        case 0x03:
            return "Intel(R) Pentium(R) III XeonTM processor";
        case 0x04:
            return "Intel(R) Pentium(R) III processor";
        case 0x06:
            return "Mobile Intel(R) Pentium(R) III processor-M";
        case 0x07:
            return "Mobile Intel(R) Celeron(R) processor";
        case 0x08:
            return "Intel(R) Pentium(R) 4 processor";
        case 0x09:
            return "Intel(R) Pentium(R) 4 processor";
        case 0x0A:
            return "Intel(R) Celeron(R) processor";
        case 0x0B:
            return "Intel(R) Xeon(TM) processor";
        case 0x0C:
            return "Intel(R) Xeon(TM) processor MP";
        case 0x0E:
            return "Mobile Intel(R) Pentium(R) 4 processor-M";
        case 0x0F:
            return "Mobile Intel(R) Celeron(R) processor";
        case 0x11:
            return "Mobile Genuine Intel(R) processor";
        case 0x12:
            return "Intel(R) Celeron(R) M processor";
        case 0x13:
            return "Mobile Intel(R) Celeron(R) processor";
        case 0x14:
            return "Intel(R) Celeron(R) processor";
        case 0x15:
            return "Mobile Genuine Intel(R) processor";
        case 0x16:
            return "Intel(R) Pentium(R) M processor";
        case 0x17:
            return "Mobile Intel(R) Celeron(R) processor";
        default:
            break;
    }
    return nullptr;
}

void getCpuInfo(CpuInfo &info)
{
    memset(&info, '\0', sizeof(CpuInfo));
    CpuidInfo cpuInfo{0, 0, 0, 0};
    cpuid(&cpuInfo, 0);
    *(uint32_t*)info.vendorId = cpuInfo.ebx;
    *((uint32_t*)info.vendorId+1) = cpuInfo.edx;
    *((uint32_t*)info.vendorId+2) = cpuInfo.ecx;

    cpuInfo.eax = cpuInfo.ebx = cpuInfo.ecx = cpuInfo.edx = 0;

    cpuid(&cpuInfo, 1);
    // EAX
    info.stepping = cpuInfo.eax & 0x0000000F;
    info.model = (cpuInfo.eax & 0x000000F0)>>4;
    info.family = (cpuInfo.eax & 0x00000F00)>>8;
    info.processor_type = (cpuInfo.eax & 0x00003000)>>12;
    info.extendedModelId = (cpuInfo.eax & 0x000F0000) >> 16;
    info.extendedFamilyId = (cpuInfo.eax & 0x0FF00000) >> 20;
    // EBX
    info.brand_index = (cpuInfo.ebx & 0x000000FF);
    info.clflush_line_size = (cpuInfo.ebx & 0x0000FF00)>>8;
    info.localApicId = (cpuInfo.ebx & 0xFF000000)>>24;

    //ECX
    info.sse3 = test(cpuInfo.ecx, CPUID_FEAT_ECX_SSE3);
    info.pclmul = test(cpuInfo.ecx, CPUID_FEAT_ECX_PCLMUL);
    info.dtes64 = test(cpuInfo.ecx, CPUID_FEAT_ECX_DTES64);
    info.monitor = test(cpuInfo.ecx, CPUID_FEAT_ECX_MONITOR);
    info.ds_cpl = test(cpuInfo.ecx, CPUID_FEAT_ECX_DS_CPL);
    info.vmx    = test(cpuInfo.ecx, CPUID_FEAT_ECX_VMX);
    info.smx    = test(cpuInfo.ecx, CPUID_FEAT_ECX_SMX);
    info.est    = test(cpuInfo.ecx, CPUID_FEAT_ECX_EST);
    info.tm2    = test(cpuInfo.ecx, CPUID_FEAT_ECX_TM2);
    info.ssse3  = test(cpuInfo.ecx, CPUID_FEAT_ECX_SSSE3);
    info.cid    = test(cpuInfo.ecx, CPUID_FEAT_ECX_CID);
    info.fma    = test(cpuInfo.ecx, CPUID_FEAT_ECX_FMA);
    info.cx16    = test(cpuInfo.ecx, CPUID_FEAT_ECX_CX16);
    info.etprd    = test(cpuInfo.ecx, CPUID_FEAT_ECX_ETPRD);
    info.pdcm    = test(cpuInfo.ecx, CPUID_FEAT_ECX_PDCM);
    info.pcide    = test(cpuInfo.ecx, CPUID_FEAT_ECX_PCIDE);
    info.dca    = test(cpuInfo.ecx, CPUID_FEAT_ECX_DCA);
    info.sse4_1    = test(cpuInfo.ecx, CPUID_FEAT_ECX_SSE4_1);
    info.sse4_2    = test(cpuInfo.ecx, CPUID_FEAT_ECX_SSE4_2);
    info.x2apic    = test(cpuInfo.ecx, CPUID_FEAT_ECX_x2APIC);
    info.movbe    = test(cpuInfo.ecx, CPUID_FEAT_ECX_MOVBE);
    info.popcnt    = test(cpuInfo.ecx, CPUID_FEAT_ECX_POPCNT);
    info.aes    = test(cpuInfo.ecx, CPUID_FEAT_ECX_AES);
    info.xsave    = test(cpuInfo.ecx, CPUID_FEAT_ECX_XSAVE);
    info.osxsave    = test(cpuInfo.ecx, CPUID_FEAT_ECX_OSXSAVE);
    info.avx    = test(cpuInfo.ecx, CPUID_FEAT_ECX_AVX);
    // EDX

    info.fpu    = test(cpuInfo.edx, CPUID_FEAT_EDX_FPU);
    info.vme    = test(cpuInfo.edx, CPUID_FEAT_EDX_VME);
    info.de    = test(cpuInfo.edx, CPUID_FEAT_EDX_DE);
    info.pse    = test(cpuInfo.edx, CPUID_FEAT_EDX_PSE);
    info.tsc    = test(cpuInfo.edx, CPUID_FEAT_EDX_TSC);
    info.msr    = test(cpuInfo.edx, CPUID_FEAT_EDX_MSR);
    info.pae    = test(cpuInfo.edx, CPUID_FEAT_EDX_PAE);
    info.mce    = test(cpuInfo.edx, CPUID_FEAT_EDX_MCE);
    info.cx8    = test(cpuInfo.edx, CPUID_FEAT_EDX_CX8);
    info.apic    = test(cpuInfo.edx, CPUID_FEAT_EDX_APIC);
    info.sep    = test(cpuInfo.edx, CPUID_FEAT_EDX_SEP);
    info.pge    = test(cpuInfo.edx, CPUID_FEAT_EDX_PGE);
    info.mca    = test(cpuInfo.edx, CPUID_FEAT_EDX_MCA);
    info.cmov    = test(cpuInfo.edx, CPUID_FEAT_EDX_CMOV);
    info.pat    = test(cpuInfo.edx, CPUID_FEAT_EDX_PAT);
    info.pse36    = test(cpuInfo.edx, CPUID_FEAT_EDX_PSE36);
    info.psn    = test(cpuInfo.edx, CPUID_FEAT_EDX_PSN);
    info.clf    = test(cpuInfo.edx, CPUID_FEAT_EDX_CLF);
    info.dtes    = test(cpuInfo.edx, CPUID_FEAT_EDX_DTES);
    info.acpi    = test(cpuInfo.edx, CPUID_FEAT_EDX_ACPI);
    info.mmx    = test(cpuInfo.edx, CPUID_FEAT_EDX_MMX);
    info.fxsr    = test(cpuInfo.edx, CPUID_FEAT_EDX_FXSR);
    info.sse    = test(cpuInfo.edx, CPUID_FEAT_EDX_SSE);
    info.sse2    = test(cpuInfo.edx, CPUID_FEAT_EDX_SSE2);
    info.ss    = test(cpuInfo.edx, CPUID_FEAT_EDX_SS);
    info.htt    = test(cpuInfo.edx, CPUID_FEAT_EDX_HTT);
    info.tm1    = test(cpuInfo.edx, CPUID_FEAT_EDX_TM1);
    info.pbe    = test(cpuInfo.edx, CPUID_FEAT_EDX_PBE);

    cpuInfo.eax = cpuInfo.ebx = cpuInfo.ecx = cpuInfo.edx = 0;

    cpuid(&cpuInfo, 2);
    


}