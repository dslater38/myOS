#!/bin/bash


BOCHS=bochs
BOCHSRC=bochsrc.txt

#~ set -x

#~ if [ "$1" == "-g" ]; then
	#~ PATH=/home/slaterd/.local/bochs269/bin:${PATH} /home/slaterd/.local/bochs269/bin/bochs -q -f bochsrc-gdb.txt
#~ else
	#~ bochs -q -dbglog debug.log -f bochsrc.txt
#~ fi

BOCHS=bochs
BOCHSRC=bochsrc.txt

if [ "$1" == "-g" -o "$2" == "-g" ]; then
BOCHS=/usr/local/bochs-gdb/bin/bochs
BOCHSRC=bochsrc-gdb.txt
fi

if [ "$1" == "-t" -o "$2" == "-t" ]; then
BOCHSRC=bochsrc-term.txt
fi


#~ if [ "$1" == "-g" ]; then
#~ /usr/local/bochs269able-avx/bin/bochs -q -f bochsrc-gdb.txt
#~ else

${BOCHS} -q -dbglog debug.log -f ${BOCHSRC}

#~ fi

