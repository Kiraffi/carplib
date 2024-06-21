rem @echo off
@setlocal

set compiler=cl.exe /Zi /Zo
rem /Zi debugging info /Zo optimized debugging information?

set links=opengl32.lib gdi32.lib user32.lib winmm.lib
set linkoptions= /I.
set compileroptimize= /std:clatest /Oi /Ot /MT /LD

rem /LD for dll

rem set compilesource=examples\triangle.c
set compilesource=examples\hotreloadfn.c

rem Oi = intrinsics, Ot = fast, Gv use vectorcall, MT multithreading /Gv 
rem /arch:AVX 
rem /link OUT:outfile.exe if passing to linker
set final=%linkoptions% %compileroptimize% %links%
md buildout

rem /Fo. sets the obj file output folder
rem /Fd. sets pdb file output folder

%compiler% %compilesource% /Fo.\buildout\ /Fd.\buildout\ /Fe:buildout/hotreloadfn.dll %final%
