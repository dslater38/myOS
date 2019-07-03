#!/bin/bash

# set -x


sudo losetup /dev/loop0 floppy.img
sudo mount /dev/loop0 /mnt/loop0
sudo cp src/kernel /mnt/loop0/kernel
sudo umount /dev/loop0
sudo losetup -d /dev/loop0

