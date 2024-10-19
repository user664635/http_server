#!/bin/bash
cd ${1%/*}
2>&1 clang -std=c2y -c ${1##*/}
