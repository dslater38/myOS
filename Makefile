MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)


CSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.c' -print)
CXXSOURCES=$(shell find $(SUBDIRS) -maxdepth 1 -name '*.cpp' -print)
SSOURCES:=$(wildcard $(SUBDIRS:%=%/*.s)) 
OBJECTS:=$(CSOURCES:%.c=%.o) $(CXXSOURCES:%.cpp=%.o) $(SSOURCES:%.s=%.o)

VM_BASE=0xFFFF800000000000

export LD=clang
export CC=clang
export CXX=clang++

export CPPFLAGS:=-I../include -D_LIBCPP_HAS_NO_BUILTIN_OPERATOR_NEW_DELETE -DVM_BASE=$(VM_BASE)
export CFLAGS:=-std=c11 -fno-use-cxa-atexit -mno-sse2 -nostdlib -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O0 -gdwarf-3 -fpie -ffreestanding
export CXXFLAGS:=-std=c++17 -fno-use-cxa-atexit -mno-sse2 -nostdlib -stdlib=libc++ -fno-rtti -fno-exceptions -fno-threadsafe-statics -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O0 -gdwarf-3 -Wno-main -fpie -ffreestanding
export ASFLAGS=-felf64 -Xvc -gdwarf -DDEBUG -DVM_BASE=$(VM_BASE)
LDFLAGS=-no-pie -ffreestanding -nostdlib -fno-exceptions -fno-threadsafe-statics -fno-rtti -mno-red-zone -Xlinker -Tlink.ld -Xlinker --no-relax -Xlinker --defsym="VM_BASE=$(VM_BASE)"

.PHONY: all $(SUBDIRS) clean

all : $(SUBDIRS) os.iso

$(SUBDIRS):
	@$(MAKE) -C $@ CC="$(CC)" CXX="$(CXX)" CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" ASFLAGS="$(ASFLAGS)" LDFLAGS="$(LDFLAGS)"  $(MAKECMDGOALS)

$(OBJECTS) : $(SUBDIRS)

os.iso : kernel hello.ko isofiles/boot/grub/grub.cfg
	cp kernel isofiles/boot/kernel
	cp hello.ko isofiles/boot/hello.ko
	grub-mkrescue -o os.iso isofiles

clean: $(SUBDIRS)
	-rm -f isofiles/boot/kernel
	-rm -f isofiles/boot/hello.ko
	-rm -f os.iso

src/kernel : $(SUBDIRS)

modules/hello.ko : $(SUBDIRS)

hello.ko : modules/hello.ko
	cp $^ $@

# kernel: $(OBJECTS) | link.ld
kernel: src/kernel
	cp $^ $@
	nm $@ | awk '{ print $$1" "$$3 }' > $@.sym

#	nm $@ | grep -i " [R|T] " | awk '{ print $$1" "$$3 }' > $@.sym


.SUFFIXES:
