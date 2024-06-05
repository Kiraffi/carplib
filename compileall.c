#define WIN32_LEAN_AND_MEAN 1

#include "external/glad/gl.c"


#include "main.c"

#if _WIN32
    #include "carplib/carpwindow_win32.c"
#else
    #include "carplib/carpwindow_linux.c"
#endif

#include "carplib/carpgl.c"
#include "carplib/carpkeyboard.c"
#include "carplib/carpmemory.c"
#include "carplib/carpmouse.c"
#include "carplib/carpshader.c"
