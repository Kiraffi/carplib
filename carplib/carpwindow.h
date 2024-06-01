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

b8 carpWindow_init(CarpWindow* window, const char* windowName, s32 width, s32 height, s32 x, s32 y);
void carpWindow_destroy(CarpWindow* window);
b8 carpWindow_update(CarpWindow* window, f32 dt);

void carpWindow_setWindowTitle(CarpWindow* window, const char* title);
void carpWindow_swapBuffers(CarpWindow* window);
void carpWindow_enableVSync(CarpWindow* window, bool vSyncEnabled);

void carpWindow_setWindowSizeChangedFn(CarpWindow* window, WindowSizeChangedFn windowSizeChangedFn);

#endif