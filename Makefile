MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)


CSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.c' -print)
CXXSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.cpp' -print)
SSOURCES:=$(wildcard $(SUBDIRS:%=%/*.s))
OBJECTS:=$(CSOURCES:%.c=%.o) $(CXXSOURCES:%.cpp=%.o) $(SSOURCES:%.s=%.o)
# $(info OBJECTS: $(OBJECTS))
# OBJECTS:=$(OBJECTS:src/boot.o=)
# $(info OBJECTS: $(OBJECTS))

export LD=gcc
export CC=clang
export CXX=clang++
export CPPFLAGS:=-I../include
export CFLAGS:=-std=c11 -mno-sse2 -nostdlib -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O0 -g
export CXXFLAGS:=-std=c++17 -mno-sse2 -nostdlib -fno-exceptions -fno-threadsafe-statics -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O0 -g -Wno-main
export ASFLAGS=-felf64 
LDFLAGS=-no-pie -ffreestanding -nostdlib -fno-exceptions -fno-threadsafe-statics -mno-red-zone  -Xlinker -Tlink.ld -lgcc


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
	$(LD) $(LDFLAGS) -o $@ $^
	nm $@ | awk '{ print $$1" "$$3 }' > $@.sym
	
#	nm $@ | grep " T " | awk '{ print $$1" "$$3 }' > $@.sym


.SUFFIXES:
