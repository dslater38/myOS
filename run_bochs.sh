#!/bin/bash

# run_bochs.sh
# mounts the correct loopback device, runs bochs, then unmounts.

# sudo /sbin/losetup /dev/loop0 floppy.img
# sudo bochs -f bochsrc.txt
# sudo /sbin/losetup -d /dev/loop0

#~ set -x

#~ if [ "$1" == "-g" ]; then
	#~ PATH=/home/slaterd/.local/bochs269/bin:${PATH} /home/slaterd/.local/bochs269/bin/bochs -q -f bochsrc-gdb.txt
#~ else
	#~ bochs -q -dbglog debug.log -f bochsrc.txt
#~ fi


if [ "$1" == "-g" ]; then
#	/home/slaterd/.local/bochs269/bin/bochs -q -f bochsrc-gdb.txt
	/usr/local/bochs269able-avx/bin/bochs -q -f bochsrc-gdb.txt
else

bochs -q -dbglog debug.log -f bochsrc.txt

fi

