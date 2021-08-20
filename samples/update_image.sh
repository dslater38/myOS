#!/bin/bash

LOOPDEV="$(sudo /sbin/losetup --show -f floppy.img)"
#sudo /sbin/losetup /dev/loop0 floppy.img
sudo mount "$LOOPDEV" /mnt2
sudo cp src/kernel /mnt2/kernel
sudo umount "$LOOPDEV"
sudo /sbin/losetup -d "$LOOPDEV"
