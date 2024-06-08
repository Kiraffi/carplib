#!/bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
mkdir buildout
clang compileall.c -O2 -obuildout/main.out  -std=c11 -lX11 -lGL -lm && buildout/main.out
