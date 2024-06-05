rem @echo off
@setlocal

set compiler=tcc.exe
rem set compiler=clang.exe  -g

set links=-lopengl32 -lgdi32 -luser32 -lwinmm
set linkfolders=-Lc:/windows/SysWOW64

set final=%links% %linkfolders%

%compiler% main.c -omain.exe -m64 -std=c11 %final%  
