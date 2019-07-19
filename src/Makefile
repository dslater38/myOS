# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

CC=clang
CXX=clang++

CSOURCES=$(shell find . -maxdepth 1 -name '*.c' -a -not -name '*.32.c' -a -not -name '*.6432.c' -print)
CSOURCES32=$(wildcard *.32.c)
CSOURCES6432=$(wildcard *.6432.c)

CXXSOURCES=$(shell find . -maxdepth 1 -name '*.cpp' -a -not -name '*.32.cpp' -a -not -name '*.6432.cpp' -print)
CXXSOURCES32=$(wildcard *.32.cpp)
CXXSOURCES6432=$(wildcard *.6432.cpp)

SSOURCES=$(wildcard *.s)

OBJECTS32=$(CXXSOURCES32:%.cpp=%.o)  $(CXXSOURCES6432:%.6432.cpp=%.32.o) $(CSOURCES32:%.c=%.o) $(CSOURCES6432:%.6432.c=%.32.o)
OBJECTS=$(CXXSOURCES:%.cpp=%.o) $(CXXSOURCES6432:%.6432.cpp=%.64.o) $(CSOURCES:%.c=%.o) $(CSOURCES6432:%.6432.c=%.64.o) $(SSOURCES:%.s=%.o)

CPPFLAGS+=-I/usr/include/x86_64-linux-gnu

CFLAGS=-std=c11 -mno-sse2 -nostdlib -mno-red-zone -mno-mmx -mno-sse -mno-sse2  -O0 -I.
CXXFLAGS=-std=c++17 -mno-sse2 -nostdlib -fno-exceptions -fno-threadsafe-statics -mno-red-zone -mno-mmx -mno-sse -mno-sse2  -O0 -Wno-main -I.
LDFLAGS=-Tlink.ld
ASFLAGS=-felf64

CFLAGS32=-m32 $(CFLAGS)
CXXFLAGS32=-m32 $(CXXFLAGS)

OBJCOPYFLAGS=-O elf64-x86-64 --elf-stt-common=yes --redefine-sym memset=memset32 

.PHONY: all
all: kernel 

clean:
	@rm -f *.o *.d *.d.* *.ds kernel

kernel: $(OBJECTS32) $(OBJECTS) | link.ld
	ld $(LDFLAGS) -o $@ $^
	nm $@ | grep " T " | awk '{ print $$1" "$$3 }' > $@.sym


.s.o:
	nasm $(ASFLAGS) $<

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.32.o: %.32.c
	$(CC) $(CPPFLAGS) $(CFLAGS32) -c $< -o $@.o
	x86_64-linux-gnu-objcopy $(OBJCOPYFLAGS) $@.o $@
	-rm $@.o

%.32.o : %.6432.c
	$(CC) $(CPPFLAGS) $(CFLAGS32) -c $< -o $@.o
	x86_64-linux-gnu-objcopy $(OBJCOPYFLAGS) $@.o $@
	-rm $@.o

%.64.o : %.6432.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.32.o: %.32.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS32) -c $< -o $@.o
	x86_64-linux-gnu-objcopy $(OBJCOPYFLAGS)  $@.o $@
	-rm $@.o

%.32.o: %.6432.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS32) -c $< -o $@.o
	x86_64-linux-gnu-objcopy $(OBJCOPYFLAGS)  $@.o $@
	-rm $@.o

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

%.32.d: %.6432.cpp
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.32.d: %.6432.c
	@set -e; rm -f $@; \
	 $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.64.d: %.6432.cpp
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

%.64.d: %.6432.c
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
-include $(CSOURCES6432:.6432.c=.64.d)
-include $(CSOURCES6432:.6432.c=.32.d)
-include $(CXXSOURCES6432:.6432.cpp=.64.cpp)
-include $(CXXSOURCES6432:.6432.cpp=.32.cpp)
