#include "carpkeyboard.h"
#include "carpmemory.h"

static bool sIsValidKeyboardKey(CarpKeyboardKey key)
{
    return key != CarpKeyboardKey_Invalid
        && (s32)key >= 0
        && (s32)key < MAX_AMOUNT_INPUT;
}

// Validity is checked before calling this!!
static bool sIsKeyboardKeyDown(CarpMemory* mem, CarpKeyboardKey key)
{
    return mem && sIsValidKeyboardKey(key)
        ? mem->carp_keyboardState.carp_keyboard_buttonDown[(s32)key]
        : false;
}

// Validity is checked before calling this!!
static b8 sGetKeyboardKeyHalfPressCount(CarpMemory* mem, CarpKeyboardKey key)
{
    return mem && sIsValidKeyboardKey(key)
        ? mem->carp_keyboardState.carp_keyboard_halfPress[(s32)key]
        : 0;
}


CARP_FN void carp_keyboard_resetState(void)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return;

    for(s32 i = 0; i < MAX_AMOUNT_INPUT; ++i)
        mem->carp_keyboardState.carp_keyboard_halfPress[i] = 0;
}

CARP_FN void carp_keyboard_setKeyState(CarpKeyboardKey key, bool keyDown)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return;

    mem->carp_keyboardState.carp_keyboard_halfPress[(s32)key]++;
    mem->carp_keyboardState.carp_keyboard_buttonDown[(s32)key] = keyDown ? 1 : 0;
}


CARP_FN bool carp_keyboard_wasKeyPressed(CarpKeyboardKey key)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return false;

    return sGetKeyboardKeyHalfPressCount(mem, key) > 0
       && sIsKeyboardKeyDown(mem, key);
}

CARP_FN bool carp_keyboard_wasKeyReleased(CarpKeyboardKey key)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return false;

    return sGetKeyboardKeyHalfPressCount(mem, key) > 0
       && !sIsKeyboardKeyDown(mem, key);
}

CARP_FN bool carp_keyboard_isKeyDown(CarpKeyboardKey key)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return false;

    return sIsKeyboardKeyDown(mem, key);
}
