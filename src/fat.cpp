#include "fat.h"
#include "common.h"
#include "ata.h"
#include <new>
#include <cstdio>
#include <list>
#include <algorithm>
#include "kmalloc.h"


template<class T>
struct KAllocator
{
    using value_type=T;
    KAllocator() = default;
    template <class U> constexpr KAllocator(const KAllocator<U>&) noexcept {}
  [[nodiscard]] T* allocate(std::size_t n)noexcept {
    if(n > std::size_t(-1) / sizeof(T)) return nullptr;
    return static_cast<T*>(kmalloc(n*sizeof(T)));
  }
  void deallocate(T* p, std::size_t) noexcept { kfree(p); }

};

std::list<uint16_t ,KAllocator<uint16_t>> get_clusters(const BootBlock &boot, uint16_t startCluster);

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

uint16_t fat16_entry(const uint8_t *fatTable, uint16_t cluster)
{
    return reinterpret_cast<const uint16_t *>(fatTable)[cluster];
}

void fat16_entry(uint8_t *fatTable, uint16_t cluster, uint16_t value)
{
    reinterpret_cast<uint16_t *>(fatTable)[cluster] = value;
}

uint16_t fat12_entry(const uint8_t *fatTable, uint16_t cluster)
{
    auto *offset = fatTable + 3*(cluster/2);
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

void fat12_entry(uint8_t *fatTable, uint16_t cluster, uint16_t value)
{
    auto *offset = fatTable + 3*(cluster/2);
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

uint8_t *read_fat_table(const BootBlock &boot)
{
    auto table_size = boot.NumBlocksFat1() * boot.BytesPerBlock();
    uint8_t *buffer = new(std::nothrow) uint8_t[table_size];
    if( buffer )
    {
        read(buffer, 1, table_size);
    }
    return buffer;
}

void dump_fat_table(const BootBlock &boot)
{
    auto *table = read_fat_table(boot);
    for( uint16_t cluster = 0; cluster<boot.TotalNumBlocks(); ++cluster)
    {
        uint16_t value = fat12_entry(table, cluster);
        if( value > 0 )
        {
            printf("cluster %d: %d (0x%x)\n", cluster, value, value);
        }
        // else
        // {
        //     printf(".");
        // }
        
    }
    :: operator delete [](table, std::nothrow);
}

DirectoryEntry *read_root_directory(const BootBlock &boot)
{
    auto fat_tables_size = boot.NumBlocksFat1() * boot.BytesPerBlock() * boot.num_FAT_tables;
    auto root_directory_size = boot.NumRootDirEntries() * 32;
    uint8_t *buffer = new(std::nothrow) uint8_t[root_directory_size];
    auto offset = (sizeof(BootBlock) + fat_tables_size) / 512;
    read(buffer, offset, root_directory_size);
    return reinterpret_cast<DirectoryEntry *>(buffer);
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
            if(strcmp(reinterpret_cast<const char *>(e.filename), fileName) == 0 &&
                strcmp(reinterpret_cast<const char *>(e.ext), ext) == 0)
                {
                    return &e;
                }
        }
    }
    return nullptr;
}

uint16_t open_file(const BootBlock &boot, const char *fileName, const char *ext)
{
    const DirectoryEntry *dir = read_root_directory(boot);
    if(dir)
    {
        dir = getDirectoryEntry(boot, dir, fileName, ext);
        if(dir)
        {
            return dir->StartCluster();
        }
    }
    return 0;
}

void print_file(const BootBlock &boot, const char *fileName, const char *ext)
{
    auto start_cluster = open_file(boot, fileName, ext);
    if( start_cluster > 0 )
    {
        auto list = get_clusters(boot, start_cluster);
        const DirectoryEntry *dir = getDirectoryEntry(boot, read_root_directory(boot), fileName, ext);
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
                        read(buffer, cluster, read_size);
                        buffer[read_size] = '\0';
                        printf("%s",buffer);
                        size -= read_size;
                    }  
            }
        }
    }
}

std::list<uint16_t ,KAllocator<uint16_t>> get_clusters(const BootBlock &boot, uint16_t startCluster)
{
    auto *table = read_fat_table(boot);
    auto end_of_chain = fat12_entry(table, 1);
    auto cluster = startCluster;
    
    auto l = std::list<uint16_t ,KAllocator<uint16_t>>{};

    for(++cluster; cluster!=end_of_chain; cluster = fat12_entry(table, cluster))
    {
        l.emplace_back(cluster);
    }
    return l;
}

void print_clusters(const BootBlock &boot, uint16_t startCluster)
{
    auto l = get_clusters(boot, startCluster);

    auto it = std::begin(l);
    printf("{ %d", *it);
    ++it;
    for(auto e = std::end(l); it != e; ++it)
    {
        printf(", %d", *it);
    }
    printf("}\n");

    // auto *table = read_fat_table(boot);
    // auto end_of_chain = fat12_entry(table, 1);
    // auto cluster = startCluster;
    // printf("{%d", cluster);
    // for(++cluster; cluster!=end_of_chain; cluster = fat12_entry(table, cluster))
    // {
    //     printf(", %d", cluster);
    // }
    // printf("}\n");
    // for( auto cluster = startCluster; cluster != 0xFFF; )
}

void dump_root_dir(const BootBlock &boot)
{
    DirectoryEntry *dir = read_root_directory(boot);
    if( dir )
    {
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
        ::operator delete[]((uint8_t *)dir, std::nothrow);
    }
}