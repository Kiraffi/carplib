#ifndef CARPWINDOW_HH
#define CARPWINDOW_HH

#include "carptype.h"

typedef void (*WindowSizeChangedCallbackFn)(int,int);


typedef struct CarpWindow
{
    _Alignas(16) struct
    { 
        f64 runningTime;
        s32 width;
        s32 height;
        b8 running;
        b8 resized;

        char windowName[104];
        char data[768];
    };
} CarpWindow;

b8 carp_window_init(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y);
void carp_window_destroy(CarpWindow* carp_window);
b8 carp_window_update(CarpWindow* carp_window, f32 dt);

void carp_window_setWindowTitle(CarpWindow* carp_window, const char* title);
void carp_window_swapBuffers(CarpWindow* carp_window);
void carp_window_enableVSync(CarpWindow* carp_window, bool vSyncEnabled);

void carp_window_setWindowSizeChangedCallbackFn(CarpWindow* carp_window, WindowSizeChangedCallbackFn windowSizeChangedCallBackFn);

#endif //CARPWINDOW_HH
