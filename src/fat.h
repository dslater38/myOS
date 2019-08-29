#ifndef FAT_H_INCLUDED
#define FAT_H_INCLUDED

#include <cstdint>
#include "vfs.h"

struct BootBlock
{
    uint8_t bootstrap[3];
    uint8_t description[8];
    uint8_t bytes_per_block[2];
    uint8_t blocks_per_alloc;
    uint8_t reserved_blocks[2];
                                    // 16 bytes
    uint8_t num_FAT_tables;
    uint8_t num_root_dir_entries[2];
    uint8_t total_num_blocks[2];
    uint8_t media_descriptor;
    uint8_t num_blocks_fat1[2];
                                    // 8 bytes
    uint8_t num_blocks_per_track[2];
    uint8_t num_heads[2];
                                    // 4 bytes
    uint8_t num_hidden_blocks[4];
                                    // 8 bytes
    uint8_t total_blocks[4];
                                    // 4 bytes
    uint8_t phys_drive_num[2];
    uint8_t extended_boot_record;
    uint8_t volume_serial[4];
    uint8_t volume_label[11];
    uint8_t fs_id[8];
    uint8_t bootstrap2[0x1c0];
    uint8_t block_sig[2];

    uint16_t BytesPerBlock()const noexcept{
        return *reinterpret_cast<const uint16_t *>(bytes_per_block);
    }

    uint16_t ReservedBlocks()const noexcept{
        return *reinterpret_cast<const uint16_t *>(reserved_blocks);
    }

    uint16_t NumRootDirEntries()const noexcept{
        return *reinterpret_cast<const uint16_t *>(num_root_dir_entries);
    }

    uint16_t TotalNumBlocks()const noexcept{
        return *reinterpret_cast<const uint16_t *>(total_num_blocks);
    }

    uint16_t NumBlocksFat1()const noexcept{
        return *reinterpret_cast<const uint16_t *>(num_blocks_fat1);
    }

    uint16_t NumBlocksPerTrack()const noexcept{
        return *reinterpret_cast<const uint16_t *>(num_blocks_per_track);
    }

    uint16_t NumHeads()const noexcept{
        return *reinterpret_cast<const uint16_t *>(num_heads);
    }

    uint32_t NumHiddenBlocks()const noexcept{
        return *reinterpret_cast<const uint32_t *>(num_hidden_blocks);
    }

    uint32_t TotalBlocks()const noexcept{
        return *reinterpret_cast<const uint32_t *>(total_blocks);
    }

    uint16_t PhysDriveNo()const noexcept{
        return *reinterpret_cast<const uint16_t *>(phys_drive_num);
    }

    uint32_t VolumeSerialNumber()const noexcept{
        return *reinterpret_cast<const uint32_t *>(volume_serial);
    }

    const char * FsId()const noexcept{
        return reinterpret_cast<const char *>(fs_id);
    }

    uint16_t BlockSig()const noexcept{
        return *reinterpret_cast<const uint16_t *>(block_sig);
    }

};

enum FileAttrs : uint8_t
{
    READONLY = 0x01u,
    HIDDEN = 0x02u,
    SYSTEM = 0x04u,
    VOLLABEL = 0x08u,
    SUBDIR = 0x10u,
    ARCHIVE = 0x20u
};

struct DirectoryEntry
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attrs;
    uint8_t reserved[10];
    uint8_t create_time[2];
    uint8_t create_date[2];
    uint8_t start_cluster[2];
    uint8_t file_size[4];

    uint16_t CreateTime()const {
        return *reinterpret_cast<const uint16_t *>(create_time);
    }

    uint16_t CreateDate()const {
        return *reinterpret_cast<const uint16_t *>(create_date);
    }

    uint16_t StartCluster()const {
        return *reinterpret_cast<const uint16_t *>(start_cluster);
    }

    uint32_t FileSize()const {
        return *reinterpret_cast<const uint32_t *>(file_size);
    }
};

class FatVfsNode : public IVfsNode
{
    uint32_t read(uint64_t offset, uint64_t size, uint8_t *buffer)noexcept override;
    virtual uint32_t write(uint64_t offset, uint64_t size, const uint8_t *buffer)noexcept override;
    virtual void open(bool forWrite)noexcept override;
    virtual void close()noexcept override;
    virtual size_t  size()noexcept override;
    virtual IVfsNode *link()noexcept override;

};

#endif // FAT_H_INCLUDED
