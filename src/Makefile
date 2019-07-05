# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

CC=clang
CXX=clang++

CSOURCES=$(wildcard *.c)
CXXSOURCES=$(wildcard *.cpp)
SSOURCES=$(wildcard *.s)

MEMMOVE=$(wildcard libc/memmove*)


OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CSOURCES:%.c=%.o) $(SSOURCES:%.s=%.o)  

CFLAGS=-std=c11 -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -O2 -g
CXXFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -O2 -g -Wno-main
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

