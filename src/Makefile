# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

CSOURCES=$(wildcard *.c)
CXXSOURCES=$(wildcard *.cpp)
SSOURCES=$(wildcard *.s)

MEMMOVE=$(wildcard libc/memmove*)


OBJECTS=$(SSOURCES:%.s=%.o) $(CSOURCES:%.c=%.o) $(CXXSOURCES:%.cpp=%.o) 

CFLAGS=-std=c11 -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-pie -O2
CXXFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-pie -O2 -Wno-main
LDFLAGS=-melf_i386 -Tlink.ld
ASFLAGS=-felf


.PHONY: all
all: kernel

clean:
	@rm -f *.o kernel

kernel: $(OBJECTS) 
	ld $(LDFLAGS) -o $@ $^

.s.o:
	nasm $(ASFLAGS) $<

