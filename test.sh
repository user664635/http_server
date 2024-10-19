#!/bin/bash
cd ${1%/*}
obj=$(mktemp)
bin=$(mktemp)
cc="clang -std=c2y -c"
src=${1##*/}

echo -e "compiling...\n"
$cc $src -o $obj &&\
echo -e "linking...\n" &&\
clang $obj ../../objs/001.o -o $bin &&\
echo -e "executing...\n" &&\
ulimit -v 65536
timeout 0.001 $bin &&\
echo success!
