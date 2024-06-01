rem @echo off
@setlocal

set path=%path%;"c:/programming/c/tinycc-mob/win32/"

rem set compiler="tcc.exe"
set compiler="clang"

set links=-lopengl32 -lgdi32 -luser32 -lwinmm
set linkfolders=-Lc:/windows/SysWOW64

set final=%links% %linkfolders%

%compiler% main.c -g -omain.exe -std=c11 %final%  
