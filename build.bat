rem @echo off
@setlocal

rem set compiler=tcc.exe
rem set compiler=clang.exe -O2
set compiler=clang.exe  -g -Wpedantic

set links=-lopengl32 -lgdi32 -luser32 -lwinmm -m64 -std=c11
set linkfolders=-Lc:/windows/SysWOW64 -I./
set final=%links% %linkfolders% 
rem set compilesource=examples/triangle.c
rem set compilesource=examples/hotreload.c
rem set compilesource=examples/cubes.c
rem set compilesource=examples/scripttest.c
set compilesource=examples/ecstest.c


md buildout


rem clang mathtest.c -O2  -g -obuildout/mathtestclang.exe %links% && buildout\mathtestclang.exe > buildout/mathtestclang.txt
rem tcc mathtest.c -O2 -g -obuildout/mathtesttcc.exe %final% && buildout\mathtesttcc.exe > buildout/mathtesttcc.txt

rem tcc mathtest.c -O2 -g -obuildout/mathtesttcc.exe %final% && buildout\mathtesttcc.exe

%compiler% %compilesource% -obuildout/main.exe %final% && buildout\main.exe
