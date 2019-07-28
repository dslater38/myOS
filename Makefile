MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)


CSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.c' -a -not -name '*.32.c' -a -not -name '*.6432.c' -print)
# CSOURCES32:=$(wildcard $(SUBDIRS:%=%/*.32.c)) 
CSOURCES6432:= $(wildcard $(SUBDIRS:%=%/*.6432.c))

CXXSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.cpp' -a -not -name '*.32.cpp' -a -not -name '*.6432.cpp' -print)
# CXXSOURCES32:=$(wildcard $(SUBDIRS:%=%/*.32.cpp))
CXXSOURCES6432:= $(wildcard $(SUBDIRS:%=%/*.6432.cpp))

SSOURCES:=$(wildcard $(SUBDIRS:%=%/*.s)) 

OBJECTS32:=$(CSOURCES32:%.c=%.o) $(CXXSOURCES32:%.32.cpp=%.o) 
OBJECTS64:=$(CSOURCES6432:%.6432.c=%.64.o) $(CXXSOURCES:%.cpp=%.o) $(CXXSOURCES6432:%.6432.cpp=%.64.o) 
OBJECTS:=$(OBJECTS32) $(OBJECTS64) $(CSOURCES:%.c=%.o) $(CXXSOURCES:%.cpp=%.o) $(SSOURCES:%.s=%.o)

export CC=clang
export CXX=clang++
export OBJCOPY=objcopy

export OBJCOPYFLAGS=-O elf64-x86-64 --elf-stt-common=yes 

LDFLAGS=-Tlink.ld


export CPPFLAGS:=
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
