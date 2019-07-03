MAKEFILES:=$(wildcard */Makefile)
SUBDIRS:=$(MAKEFILES:/Makefile=)

all : $(SUBDIRS) floppy.img 


$(SUBDIRS):
	-$(MAKE) -C $@

# src/kernel : | $(SUBDIRS)

floppy.img : src/kernel
	./update_image.sh

clean:
	make -C src clean	


.PHONY: all $(SUBDIRS)
