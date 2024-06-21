rem @echo off
@setlocal

set compiler=cl.exe /Zi /Zo
rem /Zi debugging info /Zo optimized debugging information?

set links=opengl32.lib gdi32.lib user32.lib winmm.lib
set linkoptions= /I.
set compileroptimize= /std:clatest /Oi /Ot /MT

rem set compilesource=examples\triangle.c
set compilesource=examples\hotreload.c

rem Oi = intrinsics, Ot = fast, Gv use vectorcall, MT multithreading /Gv 
rem /arch:AVX 

set final=%linkoptions% %compileroptimize% /link %links%
md buildout

rem /Fo. sets the obj file output folder
rem /Fd. sets pdb file output folder

%compiler% mathtest.c /Fo.\buildout\ /Fd.\buildout\ /O2 %final% /OUT:buildout/mathtestcl.exe  && buildout\mathtestcl.exe > buildout/mathtestcl.txt
%compiler% %compilesource% /Fo.\buildout\ /Fd.\buildout\ %final% /OUT:buildout/main.exe && buildout\main.exe
