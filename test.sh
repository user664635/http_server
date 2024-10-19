#!/bin/bash
cd ${1%/*}
obj=$(mktemp)
bin=$(mktemp)
cc="clang -std=c2y -xc -c"
src=${1##*/}

echo -e "compiling..."
$cc $src -o $obj &&\
echo -e "success!\nlinking..." &&\
clang $obj ../../objs/001.o -o $bin &&\
echo -e "success!\nexecuting..." &&\
ulimit -v 65536 &&\
timeout 0.001 $bin &&\
echo success!
rm /tmp/tmp*
