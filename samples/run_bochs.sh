#!/bin/bash

# run_bochs.sh
# mounts the correct loopback device, runs bochs, then unmounts.

sudo /sbin/losetup /dev/loop0 floppy.img
sudo /usr/local/bochs-gdb/bin/bochs -f bochsrc.txt
sudo /sbin/losetup -d /dev/loop0
