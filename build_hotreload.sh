#!/bin/bash
#tcc main.c -std=c11 -lX11 -lGL -run
#./clean.sh

# there needs to be space after if and [[ and before ]]
# otherwise its not valid
if [[ ! -e "buildout/" ]]; then
echo "Creating folder: buildout"
mkdir buildout
fi

compiler=$1
compileoptions="-std=c11 -lX11 -lGL -lm -m64 -g -shared"
compileincludes="-I./"
compilealloptions="$compileoptions $compileincludes"
tcccompileoptionsextra="-L../tinycc -I../tinycc/include"

compilesource="examples/hotreloadfn.c"


# -n = non-zero, -z = zero length string

if [[ "$compiler" ]] && [[ "$compiler" != "tcc" ]]; then
echo "using $compiler:"
$compiler $compilesource -O2 -obuildout/hotreloadfn.so $compilealloptions
else
echo "using tcc:"
tcc $compilesource -O2 -obuildout/hotreloadfn.so  $compilealloptions $tcccompileoptionsextra
fi
