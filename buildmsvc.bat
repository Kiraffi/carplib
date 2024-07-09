rem @echo off
@setlocal

set compiler=cl.exe /Zi /Zo /std:clatest /MT
rem /Zi debugging info /Zo optimized debugging information?

set links=kernel32.lib opengl32.lib gdi32.lib user32.lib winmm.lib
set linkoptions= /I.
set compileroptimize=/Oi /Ot /O2
set linkeroptions=/link /MACHINE:X64

rem set compilesource=examples/triangle.c
rem set compilesource=examples/hotreload.c
rem set compilesource=examples/cubes.c
set compilesource=examples/scripttest.c

rem Oi = intrinsics, Ot = fast, Gv use vectorcall, MT multithreading /Gv 
rem /arch:AVX 
rem /link OUT:outfile.exe if passing to linker
set final=%linkoptions% %compileroptimize% %links% %linkeroptions%
md buildout

rem /Fo. sets the obj file output folder
rem /Fd. sets pdb file output folder

rem %compiler% mathtest.c /Fo.\buildout\ /Fd.\buildout\ /O2 /Fe:buildout/mathtestcl.exe %final% && buildout\mathtestcl.exe > buildout/mathtestcl.txt
%compiler% %compilesource% /Fo.\buildout\ /Fd.\buildout\ /Fe:buildout/main.exe %final% && buildout\main.exe
