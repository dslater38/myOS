#ifndef VFS_H_INCLUDED__
#define VFS_H_INCLUDED__

#include <cstdint>
#include <cstddef>


struct IVfsDirectoryNode;
struct IVfsFileNode;

struct IVfsNode
{
    virtual uint32_t read(uint64_t offset, uint64_t size, uint8_t *buffer)noexcept =0;
    virtual uint32_t write(uint64_t offset, uint64_t size, const uint8_t *buffer)noexcept =0;
    virtual void open(bool forWrite)noexcept = 0;
    virtual void close()noexcept = 0;
    virtual size_t  size()noexcept = 0;
    virtual IVfsNode *link()noexcept = 0;
    uint64_t    inode{0};
    uint64_t    flags{0};
    uint32_t    uid{0};
    uint32_t    gid{0};
};

#endif // VFS_H_INCLUDED__