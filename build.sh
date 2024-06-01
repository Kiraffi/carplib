#!/bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
clang main.c -g -omain.out  -std=c11 -lX11 -lGL && ./main.out
