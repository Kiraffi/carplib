#ifndef CARPMEMORY_HH
#define CARPMEMORY_HH

#include "carptype.h"
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

    s32 carp_mouse_x;
    s32 carp_mouse_y;
    s32 carp_mouse_moveX;
    s32 carp_mouse_moveY;

    s32 carp_mouse_wheelX;
    s32 carp_mouse_wheelY;
    s32 carp_mouse_padding[2];
} CarpMouseState;



typedef struct CarpMemory
{
    CarpWindow _Alignas(16) carp_window;
    CarpKeyboardState _Alignas(16) carp_keyboardState;
    CarpMouseState _Alignas(16) carp_mouseState;

} CarpMemory;

CARP_FN b8 carp_memory_init(void);
CARP_FN void carp_memory_destroy(void);
CARP_FN CarpMemory* carp_memory_get(void);

#endif //CARPMEMORY_HH
