rem @echo off
@setlocal

set compiler=tcc.exe
rem set compiler=clang.exe -O2
rem set compiler=clang.exe  -g -Wpedantic

set links=-lopengl32 -lgdi32 -luser32 -lwinmm -m64 -std=c11
set linkfolders=-Lc:/windows/SysWOW64

set final=%links% %linkfolders%
md buildout


clang mathtest.c -O2  -g -obuildout/mathtestclang.exe %links% && buildout\mathtestclang.exe > buildout/mathtestclang.txt
tcc mathtest.c -O2 -g -obuildout/mathtesttcc.exe %final% && buildout\mathtesttcc.exe > buildout/mathtesttcc.txt
rem tcc mathtest.c -O2 -g -obuildout/mathtesttcc.exe %final% && buildout\mathtesttcc.exe

%compiler% compileall.c -obuildout/main.exe %final% && buildout\main.exe
