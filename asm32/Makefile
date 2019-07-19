# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.


CSOURCES=$(wildcard *.c)
CXXSOURCES=$(wildcard *.cpp)
SSOURCES=$(wildcard *.s)

OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CSOURCES:%.c=%.o) $(SSOURCES:%.s=%.o)

# CPPFLAGS+=-I/usr/include/x86_64-linux-gnu

CFLAGS:=-m32 $(CFLAGS)
CXXFLAGS:=-m32 $(CXXFLAGS)
ASFLAGS=-felf64


.PHONY: all
all: $(OBJECTS)

clean:
	@rm -f *.o *.d *.d.* *.ds kernel

%.o: %.s
	nasm $(ASFLAGS) -o $@.o $<
	$(OBJCOPY) $(OBJCOPYFLAGS) $@.o $@.oo
	$(OBJCOPY) $(OBJCOPYFLAGS2) $@.oo $@
	-rm $@.o $@.oo

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@.o
	$(OBJCOPY) $(OBJCOPYFLAGS) $@.o $@.oo
	$(OBJCOPY) $(OBJCOPYFLAGS2) $@.oo $@
	-rm $@.o $@.oo

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@.o
	$(OBJCOPY) $(OBJCOPYFLAGS)  $@.o $@.oo
	$(OBJCOPY) $(OBJCOPYFLAGS2) $@.oo $@
	-rm $@.o $@.oo

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
