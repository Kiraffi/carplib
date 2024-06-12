#!/bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
./clean.sh
mkdir buildout


compiler=$1
compileoptions="-std=c11 -lX11 -lGL -lm -m64 -g"
compileincludes=""
tcccompileoptionsextra="-L../tinycc -I../tinycc/include"

echo ''
echo 'clang'
clang mathtest.c -O2 -obuildout/mathtestclang.out $compileoptions && buildout/mathtestclang.out > buildout/mathtestclang.txt
echo ''
echo 'gcc'
gcc mathtest.c -O2 -obuildout/mathtestgcc.out  $compileoptions && buildout/mathtestgcc.out > buildout/mathtestgcc.txt
echo ''
echo 'tcc'
tcc mathtest.c -O2 -obuildout/mathtesttcc.out  $compileoptions $tcccompileoptionsextra && buildout/mathtesttcc.out > buildout/mathtesttcc.txt


if [[ "$compiler" != "tcc" ]]; then
$compiler compileall.c -O2 -obuildout/main.out  $compileoptions && buildout/main.out
else
tcc compileall.c -O2 -obuildout/main.out  $compileoptions $tcccompileoptionsextra && buildout/main.out
fi
