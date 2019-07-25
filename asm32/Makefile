# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

export CC:=i686-linux-gnu-gcc
export CXX:=i686-linux-gnu-g++
export LD:=i686-linux-gnu-ld

#~ export CC:=clang --target=i686-linux-gnu
#~ export CXX:=clang++ --target=i686-linux-gnu
#~ export LD:=ld

CSOURCES=$(wildcard *.c)
CXXSOURCES=$(wildcard *.cpp)
SSOURCES=$(wildcard *.s)

OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CSOURCES:%.c=%.o) $(SSOURCES:%.s=%.o)

CPPFLAGS+=-I../include

CFLAGS:=$(CFLAGS)
CXXFLAGS:=$(CXXFLAGS)
ASFLAGS=-felf
LDFLAGS=-Tlink.ld -m elf_i386 -L/usr/lib/gcc-cross/i686-linux-gnu/8/ -lgcc


.PHONY: all
all: kernel32

clean:
	@rm -f *.o *.d *.d.* *.ds 

%.o: %.s
	nasm $(ASFLAGS) -o $@ $<

kernel32 : $(OBJECTS) | link.ld
	$(LD) $(LDFLAGS) -o $@ $^
	nm $@ | grep " T " | awk '{ print $$1" "$$3 }' > $@.sym

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

#~ %.o: %.c
	#~ $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

#~ %.o: %.cpp
	#~ $(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.d: %.cpp
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$$

%.ds : %.s
	@set -e; rm -f $@; \
	echo $(patsubst %.s,%.o,$<) $(patsubst %.s,%.ds,$<) : $< > $@

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.ds : %.s
	@set -e; rm -f $@; \
	echo $(patsubst %.s,%.o,$<) $(patsubst %.s,%.ds,$<) : $< > $@

-include $(CSOURCES:.c=.d)
-include $(CXXSOURCES:.cpp=.d)
-include $(SSOURCES:.s=.ds)
