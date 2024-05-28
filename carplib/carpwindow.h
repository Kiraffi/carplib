#ifndef CARPWINDOW_HH
#define CARPWINDOW_HH

#include "mytype.h"
#include <stdalign.h> //alignof

typedef struct CarpWindow
{
    s32 width;
    s32 height;
    b8 running;
    b8 resized;

    char windowName[112];
    _Alignas(16) char data[256];    
} CarpWindow;


b8 carpWindow_init(CarpWindow* window, const char* windowName, s32 width, s32 height, s32 x, s32 y);
void carpWindow_destroy(CarpWindow* window);
b8 carpWindow_update(CarpWindow* window, f32 dt);

void carpWindow_setWindowTitle(CarpWindow* window, const char* title);
void carpWindow_swapBuffers(CarpWindow* window);


#endif