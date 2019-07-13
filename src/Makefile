# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

CC=clang
CXX=clang++

CSOURCES=$(shell find . -maxdepth 1 -name '*.c' -a -not -name '*.32.c' -print)
CSOURCES32=$(wildcard *.32.c)
CXXSOURCES=$(shell find . -maxdepth 1 -name '*.cpp' -a -not -name '*.32.cpp' -print)
CXXSOURCES32=$(wildcard *.32.cpp)
SSOURCES=$(wildcard *.s)

OBJECTS32=$(CXXSOURCES32:%.cpp=%.o) $(CSOURCES32:%.c=%.o)
OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CSOURCES:%.c=%.o) $(SSOURCES:%.s=%.o)  


CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -O2
CXXFLAGS=-std=c++17 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-exceptions -O2 -Wno-main
LDFLAGS=-Tlink.ld
ASFLAGS=-felf64

CFLAGS32=-m32 $(CFLAGS)
CXXFLAGS32=-m32 $(CXXFLAGS)

.PHONY: all
all: kernel 

clean:
	@rm -f *.o *.d *.d.* *.ds kernel

kernel: $(OBJECTS32) $(OBJECTS) | link.ld
	ld $(LDFLAGS) -o $@ $^

.s.o:
	nasm $(ASFLAGS) $<

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$


%.32.o: %.32.c
	$(CC) $(CPPFLAGS) $(CFLAGS32) -c $< -o $@.o
	x86_64-linux-gnu-objcopy -O elf64-x86-64 --elf-stt-common=yes $@.o $@
	-rm $@.o
	
%.32.o: %.32.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS32) -c $< -o $@.o
	x86_64-linux-gnu-objcopy -O elf64-x86-64 --elf-stt-common=yes $@.o $@
	-rm $@.o

%.d: %.cpp
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$${file/.c/.32.c}

%.ds : %.s
	@set -e; rm -f $@; \
	echo $(patsubst %.s,%.o,$<) $(patsubst %.s,%.ds,$<) : $< > $@

%.32.d: %.32.cpp
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.32.d: %.32.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$


%.ds : %.s
	@set -e; rm -f $@; \
	echo $(patsubst %.s,%.o,$<) $(patsubst %.s,%.ds,$<) : $< > $@

-include $(CSOURCES:.c=.d)
-include $(CXXSOURCES:.cpp=.d)
-include $(CSOURCES32:.c=.d)
-include $(CXXSOURCES32:.cpp=.d)
-include $(SSOURCES:.s=.ds)
