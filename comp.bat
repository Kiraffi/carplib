rem @echo off
@setlocal

set path=%path%;"c:/programming/c/tinycc-mob/win32/"

set compiler="tcc.exe"
rem set compiler="clang"

set links=-lopengl32 -lgdi32 -luser32 -lwinmm
set linkfolders=-Lc:/windows/SysWOW64

set final=%links% %linkfolders%

%compiler% main.c -g -std=c11 %final%  
