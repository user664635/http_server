#!/bin/bash
cd ${1%/*}
obj=$(mktemp)
bin=$(mktemp)
cc="clang -std=c2y -c"
src=${1##*/}

echo -e "compiling...\n"
$cc $src -o $obj &&\
echo -e "linking..." &&\
clang $obj ../../objs/001.o -o $bin &&\
echo -e "executing..." &&\
ulimit -v 524288
timeout 1 $bin
