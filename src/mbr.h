#ifndef MBR_H_INCLUDED_
#define MBR_H_INCLUDED_

#include <cstdint>

struct CHS
{
    uint8_t head;
    uint8_t sector;
    uint16_t cylinder;
};

struct MBRPartitionEntry
{
    bool active()const noexcept { return ((record[0] & 0x80u) == 0x80u); }
    CHS first()const noexcept {
        CHS chs{};
        chs.head = record[1];
        chs.sector = (record[2] & 0x3Fu);
        chs.cylinder = (((static_cast<uint16_t>(record[2]) & 0x00C0u) << 2) | record[3]);
        return chs;
    }

    uint8_t type()const noexcept {
        return record[4];
    }

    CHS last()const noexcept {
        CHS chs{};
        chs.head = record[5];
        chs.sector = (record[6] & 0x3Fu);
        chs.cylinder = (((static_cast<uint16_t>(record[6]) & 0x00C0u) << 2) | record[7]);
        return chs;
    }

    uint32_t  firstLBA()const noexcept {
        return *reinterpret_cast<const uint32_t *>(&record[8]);
    }

    uint32_t sectorCount()const noexcept {
        return *reinterpret_cast<const uint32_t *>(&record[12]);
    }

private:
    uint8_t record[16];
};

struct GenericMBR
{
    uint8_t             bootstrap_code[446];
    MBRPartitionEntry   partitions[4];
    uint8_t             boot_signature[2];
};

#endif // MBR_H_INCLUDED_
