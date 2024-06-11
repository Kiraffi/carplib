rem @echo off
@setlocal

set compiler=tcc.exe
rem set compiler=clang.exe  -g

set links=-lopengl32 -lgdi32 -luser32 -lwinmm -m64 -std=c11 -g
set linkfolders=-Lc:/windows/SysWOW64

set final=%links% %linkfolders%
md buildout


clang mathtest.c -O2 -obuildout/mathtestclang.exe %links% && buildout\mathtestclang.exe
tcc mathtest.c -O2 -obuildout/mathtesttcc.exe %final% && buildout\mathtesttcc.exe

%compiler% compileall.c -obuildout/main.exe %final% && buildout\main.exe
