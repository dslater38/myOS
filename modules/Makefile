# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

# CSOURCES=$(wildcard *.c)
# CXXSOURCES=$(wildcard *.cpp)

# SSOURCES=$(wildcard *.s)
# OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CSOURCES:%.c=%.o) $(SSOURCES:%.s=%.o)
# old VM_BASE 0xFFFF800000000000

# export LD=clang
# export CC=clang
# export CXX=clang++

HELLO_OBJS=hello.o 


ASFLAGS+=-g -F dwarf

CPPFLAGS+=-I../include
# export CFLAGS:=-std=c11 -fno-use-cxa-atexit -mno-sse2 -nostdlib -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O0 -gdwarf-3 -fpie -ffreestanding
# export CXXFLAGS:=-std=c++17 -fno-use-cxa-atexit -mno-sse2 -nostdlib -stdlib=libc++ -fno-rtti -fno-exceptions -fno-threadsafe-statics -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O0 -gdwarf-3 -Wno-main -fpie -ffreestanding
# export LDFLAGS=-no-pie -ffreestanding -nostdlib -fno-exceptions -fno-threadsafe-statics -fno-rtti -mno-red-zone -Xlinker -Tlink.ld -Xlinker --no-relax

.PHONY: all
all : hello.ko

clean:
	@rm -f *.o *.d *.d.* *.ds *.ko *.ko.sym


hello.o : hello.cpp

hello.ko : $(HELLO_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ 
#	$(LD) -Xlinker --defsym="VM_BASE=0xFFFFFFFF80200000" -Xlinker --defsym="MOD_ENTRY=hello" $(LDFLAGS) -o $@ $^ ../kernel
	nm $@ | awk '{ print $$1" "$$3 }' > $@.sym

.s.o:
	nasm $(ASFLAGS) -o $@ $<

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.d: %.cpp
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$${file/.c/.32.c}

%.ds : %.s
	@set -e; rm -f $@; \
	echo $(patsubst %.s,%.o,$<) $(patsubst %.s,%.ds,$<) : $< > $@

%.ds : %.s
	@set -e; rm -f $@; \
	echo $(patsubst %.s,%.o,$<) $(patsubst %.s,%.ds,$<) : $< > $@

-include $(CSOURCES:.c=.d)
-include $(CXXSOURCES:.cpp=.d)
-include $(SSOURCES:.s=.ds)
