rem @echo off
@setlocal

set compiler=tcc.exe
rem set compiler=clang.exe -O2
rem set compiler=clang.exe  -g -Wpedantic

set links=-lopengl32 -lgdi32 -luser32 -lwinmm -m64 -std=c11 -shared
set linkfolders=-Lc:/windows/SysWOW64 -I./

set final=%links% %linkfolders%
rem set compilesource=examples\triangle.c
set compilesource=examples\hotreloadfn.c
rem set compilesource=compileall.c

md buildout


%compiler% %compilesource% -obuildout/hotreloadfn.dll %final%
