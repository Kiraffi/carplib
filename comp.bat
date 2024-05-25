rem @echo off
@setlocal


set compiler="c:/programming/c/tinycc-mob/win32/tcc.exe"
rem set compiler="clang"

%compiler% main.c -std=c11 -lopengl32 -lgdi32 -luser32 -lwinmm -Lc:/windows/SysWOW64
