#!/bin/bash
cd ${1%/*}
obj=$(mktemp)
bin=$(mktemp)
cc="clang -fno-pic -fno-PIC -fno-plt -fno-pie -std=c23 -xc -c"
ld="clang -fuse-ld=lld -no-pie -fno-pic -fno-PIC -fno-plt -fno-pie "
src=${1##*/}
tl=0.01
ml=16384

echo -e "compiling..."
if $cc $src -o $obj &&
	echo -e "success!\nlinking..." &&
	$ld $obj ../../objs/$src.o -o $bin; then
	echo -e "success!\nexecuting...\n"
	ulimit -v $ml
	timeout $tl $bin && echo "success!" || echo "failed"
	rm /tmp/tmp*
fi
