#! /bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
#./clean.sh

if [[ ! -e "buildout/" ]]; then
echo "Creating folder: buildout"
mkdir buildout
fi

compiler=$1
compileoptions="-std=c11 -lX11 -lGL -lm -m64 -g -gdwarf -Wpedantic"
#compileoptionsreport="-ftime-report"
#"-fno-inline-small-functions"
#compilernoinline="-fno-inline"
compileincludes="-I./"
compilealloptions="$compileoptions $compileincludes $compileoptionsreport $compilernoinline"
tcccompileoptionsextra="-L../tinycc -I../tinycc/include"

#compilesource="examples/triangle.c"
#compilesource="examples/hotreload.c"
#compilesource="examples/cubes.c"
#compilesource="examples/scripttest.c"
compilesource="examples/ecstest.c"

launchprog="buildout/main.out"

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
$compiler $compilesource -obuildout/main.out  $compilealloptions && $launchprog
else
echo "using tcc:"
tcc $compilesource -obuildout/main.out  $compilealloptions $tcccompileoptionsextra && $launchprog
fi
