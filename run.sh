#!/bin/sh

if [[ ! -d "build" ]];then
	mkdir build
fi

gcc -o build/fuzzy main.c helpers.c -lm -I./include/
./build/fuzzy
