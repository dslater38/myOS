#ifndef MEMCMP_H_INCLUDED
#define MEMCMP_H_INCLUDED

#include "common.h"

namespace {

    template<std::size_t SIZE>
    int memcmp_fixed_(const void *a1, const void *a2)
    {
        auto const s1 = reinterpret_cast<const unsigned char*>(a1);
        auto const s2 = reinterpret_cast<const unsigned char*>(a2);
        auto const diff = *s1 - *s2;
        return diff ? diff : memcmp_fixed_<SIZE-1>(s1+1, s2+1);
    }

    template<>
    int memcmp_fixed_<0>(const void *s1, const void *s2){
        return 0;
    }

    inline int fast_memcmp(const void *a1, const void *a2, size_t const size){
        switch(size){
            case  0: return 0;

            case  1: return memcmp_fixed_< 1>(a1, a2);
            case  2: return memcmp_fixed_< 2>(a1, a2);
            case  3: return memcmp_fixed_< 3>(a1, a2);
            case  4: return memcmp_fixed_< 4>(a1, a2);
            case  5: return memcmp_fixed_< 5>(a1, a2);
            case  6: return memcmp_fixed_< 6>(a1, a2);
            case  7: return memcmp_fixed_< 7>(a1, a2);
            case  8: return memcmp_fixed_< 8>(a1, a2);
            case  9: return memcmp_fixed_< 9>(a1, a2);
            case 10: return memcmp_fixed_<10>(a1, a2);
            case 21: return memcmp_fixed_<21>(a1, a2);
            case 22: return memcmp_fixed_<22>(a1, a2);
            case 23: return memcmp_fixed_<23>(a1, a2);
            case 24: return memcmp_fixed_<24>(a1, a2);
            case 25: return memcmp_fixed_<25>(a1, a2);
            case 26: return memcmp_fixed_<26>(a1, a2);
            case 27: return memcmp_fixed_<27>(a1, a2);
            case 28: return memcmp_fixed_<28>(a1, a2);
            case 29: return memcmp_fixed_<29>(a1, a2);
            case 30: return memcmp_fixed_<30>(a1, a2);
            case 31: return memcmp_fixed_<31>(a1, a2);
            default: 
                return memcmp_fixed_<31>(a1, a2) + fast_memcmp(reinterpret_cast<const uint8_t *>(a1)+31, reinterpret_cast<const uint8_t *>(a2)+31, (size-31));
        }
    }
}

inline int memcmp(const void *a, const void *b, size_t size)
{
    return fast_memcmp(a,b,size);
}

#endif // MEMCMP_H_INCLUDED

