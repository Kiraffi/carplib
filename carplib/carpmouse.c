#include "carpmouse.h"
#include "carpmemory.h"

static bool sIsValidMouseButton(CarpMouseButton button)
{
    return button != CarpMouseButton_Invalid
        && (int)button >= 0
        && (int)button < MAX_AMOUNT_MOUSEINPUT;
}

// Validity is checked before calling this!!
static bool sIsMouseButtonDown(CarpMemory* mem, CarpMouseButton key)
{
    return mem && sIsValidMouseButton(key)
        ? mem->carp_mouseState.carp_mouse_buttonDown[(int)key]
        : false;
}

// Validity is checked before calling this!!
static b8 sGetMouseButtonHalfPressCount(CarpMemory* mem, CarpMouseButton key)
{
    return mem && sIsValidMouseButton(key)
        ? mem->carp_mouseState.carp_mouse_buttonHalfPress[(int)key]
        : 0;
}


void carp_mouse_resetState()
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL)
        return;

    mem->carp_mouseState.carp_mouse_wheelX = 0;
    mem->carp_mouseState.carp_mouse_wheelY = 0;
    for(int i = 0; i < MAX_AMOUNT_MOUSEINPUT; ++i)
        mem->carp_mouseState.carp_mouse_buttonHalfPress[i] = 0;
}

void carp_mouse_setPosition(int x, int y)
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL)
        return;
    mem->carp_mouseState.carp_mouse_moveX = x - mem->carp_mouseState.carp_mouse_x;
    mem->carp_mouseState.carp_mouse_moveY = y - mem->carp_mouseState.carp_mouse_y;

    mem->carp_mouseState.carp_mouse_x = x;
    mem->carp_mouseState.carp_mouse_y = y;
}

void carp_mouse_addWheelMovement(int x, int y)
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL)
        return;

    mem->carp_mouseState.carp_mouse_wheelX += x;
    mem->carp_mouseState.carp_mouse_wheelY += y;

}

void carp_mouse_setButtonState(CarpMouseButton button, bool down)
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return;

    mem->carp_mouseState.carp_mouse_buttonHalfPress[(int)button]++;
    mem->carp_mouseState.carp_mouse_buttonDown[(int)button] = down ? 1 : 0;
}


int carp_mouse_getMousePositionX()
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_x;
}
int carp_mouse_getMousePositionY()
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_y;
}

int carp_mouse_getMouseWheelX()
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_wheelX;
}
int carp_mouse_getMouseWheelY()
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_wheelY;
}


bool carp_mouse_wasButtonPressed(CarpMouseButton button)
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return false;

    return sGetMouseButtonHalfPressCount(mem, button) > 0
       && sIsMouseButtonDown(mem, button);
}

bool carp_mouse_wasButtonReleased(CarpMouseButton button)
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return false;

    return sGetMouseButtonHalfPressCount(mem, button) > 0
       && !sIsMouseButtonDown(mem, button);
}

bool carp_mouse_isButtonDown(CarpMouseButton button)
{
    CarpMemory* mem = carpMemory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return false;

    return sIsMouseButtonDown(mem, button);
}
