#!/bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
mkdir buildout

clang mathtest.c -O2 -obuildout/mathtestclang.out  -std=c11 -lX11 -lGL -lm && buildout/mathtestclang.out
gcc mathtest.c -O2 -obuildout/mathtestgcc.out  -std=c11 -lX11 -lGL -lm && buildout/mathtestgcc.out
tcc mathtest.c -O2 -obuildout/mathtestgcc.out  -std=c11 -lX11 -lGL -lm && buildout/mathtestgcc.out

tcc compileall.c -O2 -obuildout/main.out  -std=c11 -lX11 -lGL -lm && buildout/main.out
