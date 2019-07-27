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


BOCHS=bochs
BOCHSRC=bochsrc.txt

if [ "$1" == "-g" ]; then
	if [ -e /home/slaterd/.local/bochs269/bin/bochs ]; then
		export BXSHARE=/home/slaterd/.local/bochs269/share/bochs
		BOCHS=/home/slaterd/.local/bochs269/bin/bochs
		BOCHSRC=bochsrc-gdb.txt

	elif [ -e /usr/local/bochs269able-avx/bin/bochs ]; then
		export BXSHARE=/usr/local/bochs269able-avx/share/bochs
		BOCHS=/usr/local/bochs269able-avx/bin/bochs
		BOCHSRC=bochsrc-gdb.txt
	fi
fi

$BOCHS -q -f $BOCHSRC

# if [ "$1" == "-g" ]; then
#	if [ -e /home/slaterd/.local/bochs269/bin/bochs ]; then
#		BXSHARE=/usr/local/bochs269able-avx/share/bochs 
#	else
#	fi
##	/home/slaterd/.local/bochs269/bin/bochs -q -f bochsrc-gdb.txt
#	/usr/local/bochs269able-avx/bin/bochs -q -f bochsrc-gdb.txt
#else
#
#bochs -q -dbglog debug.log -f bochsrc.txt
#
#fi

