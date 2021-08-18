#include "fat.h"

#if 1

#include "common.h"
#include "ata.h"
#include <new>
#include <cstdio>
#include <list>
#include <algorithm>
#include "kmalloc.h"
#include "NewObj.h"
#include <vector>
#include <cstdint>


std::vector<int> globalVector {};

// template<class T>
// struct KAllocator
// {
//     using value_type=T;
//     KAllocator() = default;
//     template <class U> constexpr KAllocator(const KAllocator<U>&) noexcept {}
//   [[nodiscard]] T* allocate(std::size_t n)noexcept {
//     if(n > std::size_t(-1) / sizeof(T)) return nullptr;
//     return static_cast<T*>(kmalloc(n*sizeof(T)));
//   }
//   void deallocate(T* p, std::size_t) noexcept { kfree(p); }

// };

std::list<uint16_t ,KAllocator<uint16_t>> get_clusters(const FATFileSystem &fs, uint16_t startCluster);
/*
namespace std {
    namespace __detail {
        void
        _List_node_base::_M_hook(_List_node_base* const __position) _GLIBCXX_USE_NOEXCEPT
        {
        this->_M_next = __position;
        this->_M_prev = __position->_M_prev;
        __position->_M_prev->_M_next = this;
        __position->_M_prev = this;
        }
    }
}
*/

uint16_t fat16_entry(const uint8_t *fatTable, uint16_t cluster)
{
    return reinterpret_cast<const uint16_t *>(fatTable)[cluster];
}

void fat16_entry(uint8_t *fatTable, uint16_t cluster, uint16_t value)
{
    reinterpret_cast<uint16_t *>(fatTable)[cluster] = value;
}

// FAT12 entries are 12 bit integers.
// So, 2 entries use 3 bytes. forming a 24-bit 
// strictly little-endian integer. The layout is:
// If we number the nibbles from lowest to highest,
// Then the first index is 123 and the second is 456
// and they're laid as out bytes in memory like this:
// 0x12 0x43 0x56
// so index 0 == (0x12) | ((0x0F & 0x43)<<8)
//    index 1 == ((0xF0 & 0x43)>>4) | (0x56 << 8)
uint16_t fat12_entry(const uniquePtr<uint8_t[]> &fatTable, uint16_t cluster)
{
    const auto *offset = fatTable.get() + 3*(cluster/2);
    uint16_t value=0;
    if( (cluster & 0x01) == 0 )
    {
	    auto b1 = *(offset + 0);
	    auto b2 = *(offset + 1);
	    /* mjh: little-endian CPUs are ugly! */
	    value = ((0x0f & b2) << 8) | b1;
    }
    else
    {
	    auto b1 = *(offset + 1);
	    auto b2 = *(offset + 2);
	    value = b2 << 4 | ((0xf0 & b1) >> 4);
    }
    return value;
}

void fat12_entry(uniquePtr<uint8_t[]> &fatTable, uint16_t cluster, uint16_t value)
{
    auto *offset = fatTable.get() + 3*(cluster/2);
    if(cluster & 0x01)
    {
	    auto *p1 = (offset + 1);
	    auto *p2 = (offset + 2);
        *p1 = (uint8_t)((0x0f & (*p1)) | ((0x0f & value) << 4));
        *p2 = (uint8_t)(0xff & (value >> 4));
    }
    else
    {
	    auto *p1 = (offset);
	    auto *p2 = (offset + 1);
        /* mjh: little-endian CPUs are really ugly! */
        *p1 = (uint8_t)(0xff & value);
        *p2 = (uint8_t)((0xf0 & (*p2)) | (0x0f & (value >> 8)));
    }
}

uniquePtr<uint8_t[]> read_fat_table(const FATFileSystem &fs)
{
	auto table_size = fs.fatTableSize();
	auto buffer = uniquePtr<uint8_t[]> {new(std::nothrow) uint8_t[table_size]};
    if( buffer )
    {
        fs.ctl->read(fs.d, buffer.get(), fs.fatTableOffset(), table_size);
    }
    return buffer;
}

void dump_fat_table(const FATFileSystem &fs)
{	
    auto table = read_fat_table(fs);
	const auto &boot = fs.boot;
    for( uint16_t cluster = 0; cluster<boot.TotalNumBlocks(); ++cluster)
    {
        uint16_t value = fat12_entry(table, cluster);
        if( value > 0 )
        {
            printf("cluster %d: %d (0x%x)\n", cluster, value, value);
        }
    }
}

uniquePtr<DirectoryEntry[]> read_root_directory(const FATFileSystem &fs)
{
	auto root_directory_size = fs.rootDirectorySize();
    const auto nEntries = fs.boot.NumRootDirEntries();
	uniquePtr<DirectoryEntry[]> dir { new(std::nothrow) DirectoryEntry[nEntries] };
    memset(dir.get(),'\0', nEntries*sizeof(DirectoryEntry));
	auto offset = fs.rootDirectoryOffset();
    fs.ctl->read(fs.d, dir.get(), offset, root_directory_size);
	return dir;
}

struct FileDate
{
    uint8_t month;
    uint8_t day;
    uint16_t year;
};

struct FileTime
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

FileDate getFileDate(uint16_t date)
{
    FileDate dt{};
    dt.month = ((date & 0x1E0) >> 5);
    dt.day = (date & 0x1F);
    dt.year = ((date & 0xFE00) >> 9) + 1980;
    return dt;
}

FileTime getFileTime(uint16_t time)
{
    FileTime tm{};
    tm.hours = ((time & 0xF800) >> 11);
    tm.minutes = (time & 0x07E0) >> 5;
    tm.seconds = (time & 0x1F)<< 1;
    return tm;
}

void print_file_date(uint16_t date)
{
    printf("%02.2d/%02.2d/%04.4d", ((date & 0x1E0) >> 5), (date & 0x1F), ((date & 0xFE00) >> 9) + 1980 );
}

void print_file_time(uint16_t time)
{
    printf("%02.2d:%02.2d:%02.2d", ((time & 0xF800) >> 11), (time & 0x07E0) >> 5 , (time & 0x1F)<< 1 );
}

const DirectoryEntry *getDirectoryEntry(const BootBlock &boot, const DirectoryEntry *dir, const char *fileName, const char *ext)
{
    for( auto i=0; i<boot.NumRootDirEntries(); ++i )
    {
        const DirectoryEntry &e = dir[i];
        if( e.attrs & VOLLABEL)
        {
            continue;   // skip the volume label entry
        }
        if( e.filename[0] != '\0')
        {
            if(strncmp(reinterpret_cast<const char *>(e.filename), fileName, 8) == 0 &&
                strncmp(reinterpret_cast<const char *>(e.ext), ext, 3) == 0)
                {
                    return &e;
                }
        }
    }
    return nullptr;
}

uint16_t open_file(const FATFileSystem &fs, const char *fileName, const char *ext)
{
    auto rootDir = read_root_directory(fs);
    if(rootDir)
    {
	    const auto &boot = fs.boot;
        auto *dir = getDirectoryEntry(boot, rootDir.get(), fileName, ext);
        if(dir)
        {
            return dir->StartCluster();
        }
    }
    return 0;
}

void print_file(const FATFileSystem &fs, const char *fileName, const char *ext)
{
    auto start_cluster = open_file(fs, fileName, ext);
    if( start_cluster > 0 )
    {
	    const auto &boot = fs.boot;
        auto list = get_clusters(fs, start_cluster);
	    auto rootDir = read_root_directory(fs);
        const DirectoryEntry *dir = getDirectoryEntry(boot, rootDir.get(), fileName, ext);
        if(dir)
        {
            auto size = dir->FileSize();
            if( size>0 )
            {
              char buffer[513];
                for( auto it = std::begin(list), e = std::end(list);
                    it != e;
                    ++it )
                    {
                        auto cluster = *it;
                        auto read_size = std::min(size, 512u);
	                    const auto addr = fs.dataOffset() + (cluster - 2)*fs.sectorsPerCluster();
                        fs.ctl->read(fs.d, buffer, addr, read_size);
                        buffer[read_size] = '\0';
                        buffer[read_size+1] = '\0';
                        printf("%s",buffer);
                        size -= read_size;
                    }  
            }
        }
    }
}

std::list<uint16_t ,KAllocator<uint16_t>> get_clusters(const FATFileSystem &fs, uint16_t startCluster)
{
    auto table = read_fat_table(fs);
    auto end_of_chain = fat12_entry(table, 1);
    
    auto l = std::list<uint16_t ,KAllocator<uint16_t>>{};

    for(auto cluster = startCluster; cluster!=end_of_chain && cluster != 0; cluster = fat12_entry(table, cluster))
    {
        l.emplace_back(cluster);
    }
    return l;
}

void print_clusters(const FATFileSystem &fs, uint16_t startCluster)
{
    auto l = get_clusters(fs, startCluster);

    auto it = std::begin(l);
    printf("{ %d", *it);
    ++it;
    for(auto e = std::end(l); it != e; ++it)
    {
        printf(", %d", *it);
    }
    printf("}\n");
}

void dump_root_dir(const FATFileSystem &fs)
{
    auto dir = read_root_directory(fs);
    if( dir )
    {
	    const auto &boot = fs.boot;
        printf(" Volume in drive A is %.8s\n", boot.volume_label);
        printf(" Volume Serial Number is %04.4x-%04.4x\n", (boot.VolumeSerialNumber() & 0xFFFF0000) >> 16, boot.VolumeSerialNumber() & 0x0000FFFF);
        printf("\n Directory of A:\\\n\n");
        for( auto i=0; i<boot.NumRootDirEntries(); ++i )
        {
            const DirectoryEntry &e = dir[i];
            if( e.attrs & VOLLABEL)
            {
                continue;   // skip the volume label entry
            }
            if( e.filename[0] != '\0')
            {
                auto dt = getFileDate(e.CreateDate());
                auto tm = getFileTime(e.CreateTime());

                printf("%.8s.%.3s\t%d\t%02.2d/%02.2d/%04.4d %02.2d:%02.2d:%02.2d %d\n",
                    e.filename, e.ext, e.FileSize(), dt.month, dt.day, 
                    dt.year, tm.hours, tm.minutes, tm.seconds, e.StartCluster());

                // printf("%.8s.%.3s, attrs: %02.2x, size: %d, Date: ",e.filename, e.ext, e.attrs, e.FileSize());
                // print_file_date(e.CreateDate());
                // printf(", time: ");
                // print_file_time(e.CreateTime());
                // printf(", start cluster: %d\n", e.StartCluster());
            }
        }
    }
}

#endif // 0
