#!/bin/bash

# set -x

update_grub1() {
	sudo losetup /dev/loop0 floppy.img
	sudo mount /dev/loop0 /mnt/loop0
	sudo cp src/kernel /mnt/loop0/kernel
	sudo umount /dev/loop0
	sudo losetup -d /dev/loop0
}

update_grub2(){
	cp src/kernel isofiles/boot/kernel
	grub-mkrescue -o os.iso isofiles
}


update_grub2

