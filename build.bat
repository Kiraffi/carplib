rem @echo off
@setlocal

set compiler=tcc.exe
rem set compiler=clang.exe  -g

set links=-lopengl32 -lgdi32 -luser32 -lwinmm
set linkfolders=-Lc:/windows/SysWOW64

set final=%links% %linkfolders%
md buildout

%compiler% compileall.c -obuildout/main.exe -m64 -std=c11 %final%  
