#include "carpkeyboard.h"
#include "carpmemory.h"

static bool sIsValidKeyboardKey(CarpKeyboardKey key)
{
    return key != CarpKeyboardKey_Invalid
        && (int)key >= 0
        && (int)key < MAX_AMOUNT_INPUT;
}

// Validity is checked before calling this!!
static bool sIsKeyboardKeyDown(CarpMemory* mem, CarpKeyboardKey key)
{
    return mem && sIsValidKeyboardKey(key)
        ? mem->carp_keyboardState.carp_keyboard_buttonDown[(int)key]
        : false;
}

// Validity is checked before calling this!!
static b8 sGetKeyboardKeyHalfPressCount(CarpMemory* mem, CarpKeyboardKey key)
{
    return mem && sIsValidKeyboardKey(key)
        ? mem->carp_keyboardState.carp_keyboard_halfPress[(int)key]
        : 0;
}


void carp_keyboard_resetState(void)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL)
        return;

    for(int i = 0; i < MAX_AMOUNT_INPUT; ++i)
        mem->carp_keyboardState.carp_keyboard_halfPress[i] = 0;
}

void carp_keyboard_setKeyState(CarpKeyboardKey key, bool keyDown)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return;

    mem->carp_keyboardState.carp_keyboard_halfPress[(int)key]++;
    mem->carp_keyboardState.carp_keyboard_buttonDown[(int)key] = keyDown ? 1 : 0;
}


bool carp_keyboard_wasKeyPressed(CarpKeyboardKey key)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return false;

    return sGetKeyboardKeyHalfPressCount(mem, key) > 0
       && sIsKeyboardKeyDown(mem, key);
}

bool carp_keyboard_wasKeyReleased(CarpKeyboardKey key)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return false;

    return sGetKeyboardKeyHalfPressCount(mem, key) > 0
       && !sIsKeyboardKeyDown(mem, key);
}

bool carp_keyboard_isKeyDown(CarpKeyboardKey key)
{
    CarpMemory* mem = carp_memory_get();
    if(mem == NULL || !sIsValidKeyboardKey(key))
        return false;

    return sIsKeyboardKeyDown(mem, key);
}
