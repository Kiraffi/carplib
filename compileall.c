#define WIN32_LEAN_AND_MEAN 1
#define COMPILE_SINGLE_SOURCE 1

#include "carplib/carptype.h"

#include "external/glad/gl.c"

#if CARP_WIN32
    #include "carplib/carpwindow_win32.c"
#elif CARP_LINUX
    #include "carplib/carpwindow_linux.c"
#else
#error "Unsupported system"
#endif

#include "carplib/carpdynlib.c"
#include "carplib/carpgl.c"
#include "carplib/carpkeyboard.c"
#include "carplib/carpmath.c"
#include "carplib/carpmemory.c"
#include "carplib/carpmouse.c"
#include "carplib/carpshader.c"
