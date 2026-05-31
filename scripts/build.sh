#!/bin/zsh

mkdir    -p bin
cmake .  -B bin
make     -C bin
./bin/brredit
