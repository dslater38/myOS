# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

CSOURCES=$(wildcard *.c)
CXXSOURCES=$(wildcard *.cpp)
SSOURCES=$(wildcard *.s)

OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CSOURCES:%.c=%.o) $(SSOURCES:%.s=%.o)

# CPPFLAGS+=-I/usr/include/x86_64-linux-gnu

CFLAGS=-std=c11 -mno-sse2 -nostdlib -mno-red-zone -mno-mmx -mno-sse -mno-sse2  -O0 -g -I.
CXXFLAGS=-std=c++17 -mno-sse2 -nostdlib -fno-exceptions -fno-threadsafe-statics -mno-red-zone -mno-mmx -mno-sse -mno-sse2  -O0 -g -Wno-main -I.
LDFLAGS=-Tlink.ld -L /usr/lib/gcc/x86_64-linux-gnu/8  -lgcc
ASFLAGS=-felf64


.PHONY: all
all: kernel

clean:
	@rm -f *.o *.d *.d.* *.ds kernel



kernel: $(OBJECTS) | link.ld
	$(LD) $(LDFLAGS) -o $@ $^
	nm $@ | grep " T " | awk '{ print $$1" "$$3 }' > $@.sym


.s.o:
	nasm $(ASFLAGS) $<

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


-include $(CSOURCES:.c=.d)
-include $(CXXSOURCES:.cpp=.d)
-include $(SSOURCES:.s=.ds)
