#ifndef FSNODE_H_INCLUDED
#define FSNODE_H_INCLUDED
#include <stdint.h>

constexpr size_t MAX_NAME_LENGTH=256;

struct dirent
{
    char name[MAX_NAME_LENGTH];
    uint64_t    ino{0};
};

class FsNode
{
public:
    enum Flags
    {
        FS_FILE = 0x01;
        FS_DIRECTORY = 0x02;
        FS_CHARDEVICE = 0x03;
        FS_BLOCKDEVICE = 0x04;
        FS_PIPE = 0x05;
        FS_SYMLINK = 0x06;
        FS_MOUNTPOINT = 0x08; // Is the file an active mountpoint?        
    };
public:
    virtual size_t read(size_t offset, size_t size, uint8_t *buffer)=0;
    virtual size_t write(size_t offset, size_t size, const uint8_t *buffer)=0;
    virtual void open(bool read, bool write)=0;
    virtual void close()=0;
    virtual struct dirent *readdir(size_t index)=0;
    virtual FsNode *finddir(const char *name)=0;
private:
    char name[MAX_NAME_LENGTH];
    uint32_t    perms{0};
    uint32_t    flags{0};
    uint32_t    uid{0};
    uint32_t    gid{0};
    uint32_t    flags{0};
    uint32_t    impl{0};
    uint64_t    length{0};
    uint64_t    inode{0};
    FsNode      *ptr{nullptr};
};


#endif // FSNODE_H_INCLUDED
