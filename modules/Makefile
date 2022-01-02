# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

HELLO_OBJS=hello.o 


ASFLAGS+=-g -F dwarf

CPPFLAGS+=-I../include

.PHONY: all
all : hello.ko

clean:
	@rm -f *.o *.d *.d.* *.ds *.ko *.ko.sym


hello.o : hello.cpp

hello.ko : $(HELLO_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ 
	$(LD) -Xlinker --defsym="VM_BASE=0xFFFF800000200000" -Xlinker --defsym="MOD_ENTRY=hello" $(LDFLAGS) -o $@ $^ ../kernel
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
