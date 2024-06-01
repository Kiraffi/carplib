#ifndef MYMEMORY_HH
#define MYMEMORY_HH

#include "carpwindow.h"

#define MAX_AMOUNT_INPUT 1024
typedef struct CarpKeyboardState
{
    u8 _Alignas(16) carp_keyboard_buttonDown[MAX_AMOUNT_INPUT];
    u8 _Alignas(16) carp_keyboard_halfPress[MAX_AMOUNT_INPUT];
} CarpKeyboardState;



#define MAX_AMOUNT_MOUSEINPUT 16
typedef struct CarpMouseState
{
    u8 _Alignas(16) carp_mouse_buttonDown[MAX_AMOUNT_MOUSEINPUT];
    u8 _Alignas(16) carp_mouse_buttonHalfPress[MAX_AMOUNT_MOUSEINPUT];
    
    int carp_mouse_x;
    int carp_mouse_y;
    int carp_mouse_xMove;
    int carp_mouse_yMove;
    
    int carp_mouse_wheel;
    int carp_mouse_padding[3];
} CarpMouseState;



typedef struct CarpMemory
{
    CarpWindow _Alignas(16) carp_window;
    CarpKeyboardState _Alignas(16) carp_inputKeysState;
    CarpMouseState _Alignas(16) carp_mouseState;

} CarpMemory;

b8 carpMemory_init();
void carpMemory_deinit();
CarpMemory* carpMemory_get();

#endif