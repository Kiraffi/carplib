#!/bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
mkdir buildout
clang compileall.c -g -obuildout/main.out  -std=c11 -lX11 -lGL && buildout/main.out
