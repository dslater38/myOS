#!/bin/bash


BOCHS=bochs
BOCHSRC=bochsrc.txt

if [ "$1" == "-g" -o "$2" == "-g" ]; then
	BOCHS=/usr/local/bochs-gdb/bin/bochs
	BOCHSRC=bochsrc-gdb.txt
	export BXSHARE=/usr/local/bochs-gdb/share/bochs
fi

if [ "$1" == "-t"  -o "$2" == "-t" ]; then
	BOCHSRC=bochsrc-term.txt
fi

exec $BOCHS -q -f $BOCHSRC

