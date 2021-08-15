#!/bin/bash
set -x

SCRIPTDIR="$(dirname $(realpath $0))"

cd "$SCRIPTDIR"

BOCHS=bochs
BOCHSRC=bochsrc.txt

DISPLAY=${DISPLAY:-192.168.3.175:1}

if [ -r /usr/local/bochs/bin/bochs ]; then
	BOCHS=/usr/local/bochs/bin/bochs
fi

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

${BOCHS} -q -f ${BOCHSRC}

#~ fi

