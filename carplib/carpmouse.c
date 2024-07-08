#include "carpmouse.h"
#include "carpmemory.h"

static bool sIsValidMouseButton(CarpMouseButton button)
{
    return button != CarpMouseButton_Invalid
        && (s32)button >= 0
        && (s32)button < MAX_AMOUNT_MOUSEINPUT;
}

// Validity is checked before calling this!!
static bool sIsMouseButtonDown(CarpMemory* mem, CarpMouseButton key)
{
    return mem && sIsValidMouseButton(key)
        ? mem->carp_mouseState.carp_mouse_buttonDown[(s32)key]
        : false;
}

// Validity is checked before calling this!!
static b8 sGetMouseButtonHalfPressCount(CarpMemory* mem, CarpMouseButton key)
{
    return mem && sIsValidMouseButton(key)
        ? mem->carp_mouseState.carp_mouse_buttonHalfPress[(s32)key]
        : 0;
}


CARP_FN void carp_mouse_resetState(void)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return;

    mem->carp_mouseState.carp_mouse_wheelX = 0;
    mem->carp_mouseState.carp_mouse_wheelY = 0;
    for(s32 i = 0; i < MAX_AMOUNT_MOUSEINPUT; ++i)
        mem->carp_mouseState.carp_mouse_buttonHalfPress[i] = 0;
}

CARP_FN void carp_mouse_setPosition(s32 x, s32 y)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return;
    mem->carp_mouseState.carp_mouse_moveX = x - mem->carp_mouseState.carp_mouse_x;
    mem->carp_mouseState.carp_mouse_moveY = y - mem->carp_mouseState.carp_mouse_y;

    mem->carp_mouseState.carp_mouse_x = x;
    mem->carp_mouseState.carp_mouse_y = y;
}

CARP_FN void carp_mouse_addWheelMovement(s32 x, s32 y)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return;

    mem->carp_mouseState.carp_mouse_wheelX += x;
    mem->carp_mouseState.carp_mouse_wheelY += y;

}

CARP_FN void carp_mouse_setButtonState(CarpMouseButton button, bool down)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return;

    mem->carp_mouseState.carp_mouse_buttonHalfPress[(s32)button]++;
    mem->carp_mouseState.carp_mouse_buttonDown[(s32)button] = down ? 1 : 0;
}


CARP_FN s32 carp_mouse_getPositionX(void)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_x;
}
CARP_FN s32 carp_mouse_getPositionY(void)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_y;
}

CARP_FN s32 carp_mouse_getWheelX(void)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_wheelX;
}
CARP_FN s32 carp_mouse_getWheelY(void)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return 0;
    return mem->carp_mouseState.carp_mouse_wheelY;
}


CARP_FN bool carp_mouse_wasButtonPressed(CarpMouseButton button)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return false;

    return sGetMouseButtonHalfPressCount(mem, button) > 0
       && sIsMouseButtonDown(mem, button);
}

CARP_FN bool carp_mouse_wasButtonReleased(CarpMouseButton button)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return false;

    return sGetMouseButtonHalfPressCount(mem, button) > 0
       && !sIsMouseButtonDown(mem, button);
}

CARP_FN bool carp_mouse_isButtonDown(CarpMouseButton button)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidMouseButton(button))
        return false;

    return sIsMouseButtonDown(mem, button);
}
