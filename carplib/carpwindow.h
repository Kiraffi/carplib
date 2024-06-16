#ifndef CARPWINDOW_HH
#define CARPWINDOW_HH

#include "carptype.h"

typedef void (*WindowSizeChangedCallbackFn)(int,int);


typedef struct CarpWindow
{
    struct
    {
        union
        {
            struct
            {
                f64 runningTime;
                s32 width;
                s32 height;
                b8 running;
                b8 resized;

                char windowName[104];
                char data[768];
            };
            _Alignas(16) uint8_t alignedData[1024];
        };
         
    };
} CarpWindow;
_Static_assert(sizeof(CarpWindow) == 1024, "Carp window size should be 1024 bytes!");


CARP_FN b8 carp_window_init(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y);
CARP_FN void carp_window_destroy(CarpWindow* carp_window);
CARP_FN b8 carp_window_update(CarpWindow* carp_window, f32 dt);

CARP_FN void carp_window_setWindowTitle(CarpWindow* carp_window, const char* title);
CARP_FN void carp_window_swapBuffers(CarpWindow* carp_window);
CARP_FN void carp_window_enableVSync(CarpWindow* carp_window, bool vSyncEnabled);

CARP_FN void carp_window_setWindowSizeChangedCallbackFn(CarpWindow* carp_window, WindowSizeChangedCallbackFn windowSizeChangedCallBackFn);

#endif //CARPWINDOW_HH
