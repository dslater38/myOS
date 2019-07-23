MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)


CSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.c' -a -not -name '*.32.c' -a -not -name '*.6432.c' -print)
CSOURCES32:=$(wildcard $(SUBDIRS:%=%/*.32.c)) 
CSOURCES6432:= $(wildcard $(SUBDIRS:%=%/*.6432.c))

CXXSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.cpp' -a -not -name '*.32.cpp' -a -not -name '*.6432.cpp' -print)
CXXSOURCES32:=$(wildcard $(SUBDIRS:%=%/*.32.cpp))
CXXSOURCES6432:= $(wildcard $(SUBDIRS:%=%/*.6432.cpp))

SSOURCES:=$(wildcard $(SUBDIRS:%=%/*.s)) 

OBJECTS32:=$(CSOURCES32:%.c=%.o) $(CSOURCES6432:%.6432.c=%.32.o)  $(CXXSOURCES32:%.32.cpp=%.o) $(CXXSOURCES6432:%.6432.cpp=%.32.o) 
OBJECTS64:=$(CSOURCES6432:%.6432.c=%.64.o) $(CXXSOURCES:%.cpp=%.o) $(CXXSOURCES6432:%.6432.cpp=%.64.o) 
OBJECTS:=$(OBJECTS32) $(OBJECTS64) $(CSOURCES:%.c=%.o) $(CXXSOURCES:%.cpp=%.o) $(SSOURCES:%.s=%.o)

export CC=clang
export CXX=clang++
export OBJCOPY=objcopy

export RENAME_SYMS=--redefine-sym _32_placement_address=placement_address \
--redefine-sym _32_end=end \
--redefine-sym _32_start=start \
--redefine-sym _32_long_mode_start=long_mode_start \
--redefine-sym _32_init_stack32=init_stack32 \
--redefine-sym _32_cursor_x=cursor_x \
--redefine-sym _32_cursor_y=cursor_y \
--redefine-sym _32_backColor=backColor \
--redefine-sym _32_foreColor=foreColor \
--redefine-sym _32_cur_line=cur_line \
--redefine-sym _32_back_buffer=back_buffer \
--redefine-sym _32_mboot_header=mboot_header

export OBJCOPYFLAGS=-O elf64-x86-64 --elf-stt-common=yes --prefix-symbols=_32_ 
export OBJCOPYFLAGS2=-O elf64-x86-64 --elf-stt-common=yes $(RENAME_SYMS)


# OBJECTS32=$(CXXSOURCES32:%.cpp=%.o)  $(CXXSOURCES6432:%.6432.cpp=%.32.o) $(CSOURCES32:%.c=%.o) $(CSOURCES6432:%.6432.c=%.32.o)

# OBJECTS:=$(CSOURCES:%.c=%.o) $(CXXSOURCES:%.cpp=%.o)

# $(info $(OBJECTS))

# OBJECTS:=$(wildcard $(SUBDIRS:%=%/*.o))
LDFLAGS=-Tlink.ld


export CPPFLAGS:=-I $(realpath ./src) -I $(realpath ./asm32)
export CFLAGS:=-std=c11 -mno-sse2 -nostdlib -mno-red-zone -mno-mmx -mno-sse -mno-sse2  -O0 -g
export CXXFLAGS:=-std=c++17 -mno-sse2 -nostdlib -fno-exceptions -fno-threadsafe-statics -mno-red-zone -mno-mmx -mno-sse -mno-sse2  -O0 -Wno-main -g 

.PHONY: all $(SUBDIRS) clean

all : $(SUBDIRS) os.iso

$(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)
	
$(OBJECTS) : $(SUBDIRS)

os.iso : kernel isofiles/boot/grub/grub.cfg
	cp kernel isofiles/boot/kernel
	grub-mkrescue -o os.iso isofiles

clean: $(SUBDIRS)
	-rm -f isofiles/boot/kernel
	-rm -f os.iso


kernel: $(OBJECTS) | link.ld
	ld $(LDFLAGS) -o $@ $^
	nm $@ | grep " T " | awk '{ print $$1" "$$3 }' > $@.sym


.SUFFIXES:
