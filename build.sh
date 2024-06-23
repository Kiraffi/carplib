#! /bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
#./clean.sh

if [[ ! -e "buildout/" ]]; then
echo "Creating folder: buildout"
mkdir buildout
fi

compiler=$1
compileoptions="-std=c11 -lX11 -lGL -lm -m64 -g"
compileincludes="-I./"
compilealloptions="$compileoptions $compileincludes"
tcccompileoptionsextra="-L../tinycc -I../tinycc/include"

#compilesource="examples/triangle.c"
compilesource="examples/hotreload.c"


# echo ''
# echo 'clang'
# clang mathtest.c -O2 -obuildout/mathtestclang.out $compilealloptions && buildout/mathtestclang.out > buildout/mathtestclang.txt
# echo ''
# echo 'gcc'
# gcc mathtest.c -O2 -obuildout/mathtestgcc.out  $compilealloptions && buildout/mathtestgcc.out > buildout/mathtestgcc.txt
# echo ''
# echo 'tcc'
# #tcc mathtest.c -O2 -obuildout/mathtesttcc.out  $compileoptions $tcccompileoptionsextra && buildout/mathtesttcc.out > buildout/mathtesttcc.txt
# tcc mathtest.c -O2 -obuildout/mathtesttcc.out  $compilealloptions $tcccompileoptionsextra && buildout/mathtesttcc.out


if [[ "$compiler" ]] && [[ "$compiler" != "tcc" ]]; then
echo "using $compiler:"
$compiler $compilesource -O2 -obuildout/main.out  $compilealloptions && buildout/main.out
else
echo "using tcc:"
tcc $compilesource -O2 -obuildout/main.out  $compilealloptions $tcccompileoptionsextra && buildout/main.out
fi
