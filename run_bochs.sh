#!/bin/bash

# run_bochs.sh
# mounts the correct loopback device, runs bochs, then unmounts.

# sudo /sbin/losetup /dev/loop0 floppy.img
# sudo bochs -f bochsrc.txt
# sudo /sbin/losetup -d /dev/loop0

if [ "$1" == "-g" ]; then
/usr/local/bochs269able-avx/bin/bochs -q -f bochsrc-gdb.txt
else

bochs -q -dbglog debug.log -f bochsrc.txt

fi

