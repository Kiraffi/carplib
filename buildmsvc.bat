rem @echo off
@setlocal

set compiler=cl.exe -Zi

set links=opengl32.lib gdi32.lib user32.lib winmm.lib
set linkoptions= 
set compileroptimize= /std:clatest /Oi /Ot /MT
rem Oi = intrinsics, Ot = fast, Gv use vectorcall, MT multithreading /Gv 
rem /arch:AVX 

set final=%linkoptions% %compileroptimize% /link %links%
md buildout

cl mathtest.c /O2 %final% /OUT:buildout/mathtestcl.exe  && buildout\mathtestcl.exe > buildout/mathtestcl.txt
%compiler% compileall.c %final% /OUT:buildout/main.exe && buildout\main.exe
