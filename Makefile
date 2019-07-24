MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)
SUBDIRS64:=$(SUBDIRS:asm32=)

export CC=gcc
export CXX=g++
export OBJCOPY=objcopy

export OBJCOPYFLAGS=-O elf64-x86-64 --elf-stt-common=yes 

LDFLAGS=-Tlink.ld 

LDFLAGS32=-Tlink32.ld -m elf_i386 -L/usr/lib/gcc-cross/i686-linux-gnu/8/x32

export CPPFLAGS:=-I $(realpath ./src)
export CFLAGS:=-std=c11 -mno-sse2 -nostdlib -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -ffreestanding  -O0 -g
export CXXFLAGS:=-std=c++17 -mno-sse2 -nostdlib -fno-exceptions -fno-threadsafe-statics -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -ffreestanding  -O0 -Wno-main -g

.PHONY: all $(SUBDIRS) clean

all : $(SUBDIRS) os.iso

$(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)
	
$(OBJECTS) : $(SUBDIRS)

os.iso : asm32/kernel32 src/kernel isofiles/boot/grub/grub.cfg
	cp asm32/kernel32 isofiles/boot/kernel32
	cp src/kernel isofiles/boot/kernel
	grub-mkrescue -o os.iso isofiles

clean: $(SUBDIRS)
	-rm -f isofiles/boot/kernel
	-rm -f os.iso

asm32/kernel32 : asm32


src/kernel: src 

.SUFFIXES:
