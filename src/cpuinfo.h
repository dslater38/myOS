#ifndef CPUINFO_H_INCLUDED__
#define CPUINFO_H_INCLUDED__
#include <cstdint>

struct CpuidInfo
{
	uint32_t	eax{0};
	uint32_t	ebx{0};
	uint32_t	ecx{0};
	uint32_t	edx{0};
};

extern "C"
{
    void cpuid(CpuidInfo *info, uint32_t req);
}

struct CpuInfo
{
    // EAX == 0 info returned in eax,ebx,ecx,edx
    char vendorId[13];
    uint32_t    stepping : 4;
    uint32_t    model : 4;
    uint32_t    family : 4;
    // EAX == 1 info returned in eax,ebx,ecx,edx
    // EAX
    uint32_t    processor_type : 2;
    uint32_t    reserved : 2;
    uint32_t    extendedModelId : 4;
    uint32_t    extendedFamilyId : 8;
    // EBX
    uint32_t    reserved1 : 4;
    uint8_t     brand_index;
    uint8_t     clflush_line_size;
    uint8_t     localApicId;
    // ECX
    uint32_t    sse3 : 1;   
    uint32_t    pclmul : 1;
    uint32_t    dtes64 : 1;
    uint32_t    monitor : 1;
    uint32_t    ds_cpl : 1;
    uint32_t    vmx : 1;
    uint32_t    smx : 1;
    uint32_t    est : 1;
    uint32_t    tm2 : 1;
    uint32_t    ssse3 : 1;
    uint32_t    cid : 1;
    uint32_t    reserved2 : 1;
    uint32_t    fma : 1;
    uint32_t    cx16 : 1;
    uint32_t    etprd : 1;
    uint32_t    pdcm : 1;
    uint32_t    reserved3 : 1;
    uint32_t    pcide : 1;
    uint32_t    dca : 1;
    uint32_t    sse4_1 : 1;
    uint32_t    sse4_2 : 1;
    uint32_t    x2apic : 1;
    uint32_t    movbe : 1;
    uint32_t    popcnt : 1;
    uint32_t    reserved4 : 1;
    uint32_t    aes : 1;
    uint32_t    xsave : 1;
    uint32_t    osxsave : 1;
    uint32_t    avx : 1;
    // EDX
    uint32_t    fpu : 1;
    uint32_t    vme : 1;
    uint32_t    de : 1;
    uint32_t    pse : 1;
    uint32_t    tsc : 1;
    uint32_t    msr : 1;
    uint32_t    pae : 1;
    uint32_t    mce : 1;
    uint32_t    cx8 : 1;
    uint32_t    apic : 1;
    uint32_t    reserved5 : 1;
    uint32_t    sep : 1;
    uint32_t    mtrr : 1;
    uint32_t    pge : 1;
    uint32_t    mca : 1;
    uint32_t    cmov : 1;
    uint32_t    pat : 1;
    uint32_t    pse36 : 1;
    uint32_t    psn : 1;
    uint32_t    clf : 1;
    uint32_t    reserved6 : 1;
    uint32_t    dtes : 1;
    uint32_t    acpi : 1;
    uint32_t    mmx : 1;
    uint32_t    fxsr : 1;
    uint32_t    sse : 1;
    uint32_t    sse2 : 1;
    uint32_t    ss : 1;
    uint32_t    htt : 1;
    uint32_t    tm1 : 1;
    uint32_t    ia64 : 1;
    uint32_t    pbe : 1;

};

void getCpuInfo(CpuInfo &info);
const char * getBrandString( uint32_t index);

#endif // CPUINFO_H_INCLUDED__
