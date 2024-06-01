#ifndef CARPWINDOW_HH
#define CARPWINDOW_HH

#include "mytype.h"
#include <stdalign.h> //alignof

typedef void (*WindowSizeChangedFn)(int,int);


typedef struct CarpWindow
{
    f64 runningTime;
    s32 width;
    s32 height;
    b8 running;
    b8 resized;

    char windowName[104];
    _Alignas(16) char data[768];
} CarpWindow;

b8 carpWindow_init(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y);
void carpWindow_destroy(CarpWindow* carp_window);
b8 carpWindow_update(CarpWindow* carp_window, f32 dt);

void carpWindow_setWindowTitle(CarpWindow* carp_window, const char* title);
void carpWindow_swapBuffers(CarpWindow* carp_window);
void carpWindow_enableVSync(CarpWindow* carp_window, bool vSyncEnabled);

void carpWindow_setWindowSizeChangedFn(CarpWindow* carp_window, WindowSizeChangedFn windowSizeChangedFn);

#endif