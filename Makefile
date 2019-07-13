MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)

all : $(SUBDIRS) os.iso


$(SUBDIRS):
	-$(MAKE) -C $@

# src/kernel : | $(SUBDIRS)

# floppy.img : src/kernel
#	./update_image.sh
#


os.iso : src/kernel | isofiles/boot/grub/grub.cfg
	./update_image.sh


clean:
	make -C src clean
	-rm -f isofiles/boot/kernel
	-rm -f os.iso


.PHONY: all $(SUBDIRS)
