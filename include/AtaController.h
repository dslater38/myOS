#ifndef ATACONTROLLER_H_INCLUDED
#define ATACONTROLLER_H_INCLUDED
#include <cstdint>
#include <memory>

struct AtaController
{
    enum class AtaDeviceType
    {
        PATAPI,
        SATAPI,
        PATA,
        SATA,
        UNKNOWN
    };

    enum class Drive
    {
        Primary,
        Secondary
    };

    virtual size_t write(Drive drive, void *buffer, uint32_t address, size_t size)const=0;
    virtual size_t read(Drive drive, void *buffer, uint32_t address, size_t size)const=0;
    virtual bool identify(Drive drv)const =0;
    virtual AtaDeviceType detect(Drive drv)const=0;
    static const AtaController &primary();
    static const AtaController &secondary();
};


#endif // ATACONTROLLER_H_INCLUDED
