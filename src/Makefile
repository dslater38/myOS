# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

CSOURCES=$(wildcard *.c)
CXXSOURCES=$(wildcard *.cpp)

SSOURCES=$(wildcard *.s)
OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CSOURCES:%.c=%.o) $(SSOURCES:%.s=%.o)

CPPFLAGS+=-I.
# CFLAGS+=
# CXXFLAGS+=
LDFLAGS=-Tlink.ld
ASFLAGS=-felf64

CFLAGS32=-m32 $(CFLAGS)
CXXFLAGS32=-m32 $(CXXFLAGS)

.PHONY: all
all: $(OBJECTS32) $(OBJECTS)

clean:
	@rm -f *.o *.d *.d.* *.ds kernel

.s.o:
	nasm $(ASFLAGS) $<

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.32.o: %.32.c
	$(CC) $(CPPFLAGS) $(CFLAGS32) -c $< -o $@.o
	$(OBJCOPY) $(OBJCOPYFLAGS) $@.o $@.oo
	$(OBJCOPY) $(OBJCOPYFLAGS2) $@.oo $@
	-rm $@.o $@.oo

%.32.o : %.6432.c
	$(CC) $(CPPFLAGS) $(CFLAGS32) -c $< -o $@.o
	$(OBJCOPY) $(OBJCOPYFLAGS) $@.o $@.oo
	$(OBJCOPY) $(OBJCOPYFLAGS2) $@.oo $@
	-rm $@.o $@.oo

%.64.o : %.6432.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.32.o: %.32.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS32) -c $< -o $@.o
	$(OBJCOPY) $(OBJCOPYFLAGS)  $@.o $@.oo
	$(OBJCOPY) $(OBJCOPYFLAGS2) $@.oo $@
	-rm $@.o $@.oo

%.32.o: %.6432.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS32) -c $< -o $@.o
	$(OBJCOPY) $(OBJCOPYFLAGS)  $@.o $@.oo
	$(OBJCOPY) $(OBJCOPYFLAGS2) $@.oo $@
	-rm $@.o $@.oo

%.64.o: %.6432.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

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

%.6432.d: %.6432.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed -e 's,\($*\)\.o[ :]*,\1.o $@ : ,g'  -e 's,\.6432\.,.64.,' < $@.$$$$ > $@; \
	 sed -e 's,\($*\)\.o[ :]*,\1.o $@ : ,g'  -e 's,\.6432\.,.32.,' < $@.$$$$ >> $@; \
	 rm -f $@.$$$$

%.6432.d: %.6432.cpp
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed -e 's,\($*\)\.o[ :]*,\1.o $@ : ,g'  -e 's,\.6432\.,.64.,' < $@.$$$$ > $@; \
	 sed -e 's,\($*\)\.o[ :]*,\1.o $@ : ,g'  -e 's,\.6432\.,.32.,' < $@.$$$$ >> $@; \
	 rm -f $@.$$$$

%.ds : %.s
	@set -e; rm -f $@; \
	echo $(patsubst %.s,%.o,$<) $(patsubst %.s,%.ds,$<) : $< > $@

-include $(CSOURCES:.c=.d)
-include $(CXXSOURCES:.cpp=.d)
-include $(CSOURCES32:.c=.d)
-include $(CXXSOURCES32:.cpp=.d)
-include $(SSOURCES:.s=.ds)
-include $(CSOURCES6432:.6432.c=.6464.d)
-include $(CXXSOURCES6432:.6432.cpp=.6432.d)
