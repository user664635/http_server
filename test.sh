#!/bin/bash
cd ${1%/*}
echo -e "compiling...\n"
clang -std=c2y -c ${1##*/} && echo "success"
