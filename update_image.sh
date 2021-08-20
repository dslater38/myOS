#!/bin/bash

# set -x

update_grub1() {
	local loopDev="$(sudo losetup --show -f floppy.img)"
#	sudo losetup /dev/loop0 floppy.img
	sudo mount "$loopDev" /mnt/loop0
	sudo cp src/kernel /mnt/loop0/kernel
	sudo umount "$loopDev"
	sudo losetup -d "$loopDev"
}

update_grub2(){
	cp src/kernel isofiles/boot/kernel
	grub-mkrescue -o os.iso isofiles
}


update_grub2

