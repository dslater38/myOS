MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)

all : os.iso

$(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

src/kernel : $(SUBDIRS)

os.iso : src/kernel isofiles/boot/grub/grub.cfg
	./update_image.sh

clean: $(SUBDIRS)
	-rm -f isofiles/boot/kernel
	-rm -f os.iso


.PHONY: all $(SUBDIRS) clean

