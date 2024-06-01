#include "mykey.h"
#include "mymemory.h"

void carpKeys_resetState()
{
    MyMemory* mem = myMemory_get();
    if(mem == NULL)
        return;

    for(int i = 0; i < MAX_AMOUNT_INPUT; ++i)
        mem->input.keysHalfPress[i] = 0;
}

void carpKeys_setKeyState(MyKey key, bool keyDown)
{
    if(key == MyKey_InvalidKey && (int)key >= 0 && (int)key < MAX_AMOUNT_INPUT)
        return;
    MyMemory* mem = myMemory_get();
    if(mem == NULL)
        return;

    mem->input.keysHalfPress[(int)key]++;
    mem->input.keysDown[(int)key] = keyDown ? 1 : 0;
}


bool carpKeys_wasPressed(MyKey key)
{
    if(key == MyKey_InvalidKey && (int)key >= 0 && (int)key < MAX_AMOUNT_INPUT)
        return false;
    MyMemory* mem = myMemory_get();
    if(mem == NULL)
        return false;

    return mem->input.keysHalfPress[(int)key] == 1
       && mem->input.keysDown[(int)key] == 1;
}

bool carpKeys_wasReleased(MyKey key)
{
    if(key == MyKey_InvalidKey && (int)key >= 0 && (int)key < MAX_AMOUNT_INPUT)
        return false;
    MyMemory* mem = myMemory_get();
    if(mem == NULL)
        return false;

    return mem->input.keysHalfPress[(int)key] == 1
       && mem->input.keysDown[(int)key] == 0;
}
bool carpKeys_isDown(MyKey key)
{
    if(key == MyKey_InvalidKey && (int)key >= 0 && (int)key < MAX_AMOUNT_INPUT)
        return false;
    MyMemory* mem = myMemory_get();
    if(mem == NULL)
        return false;

    return mem->input.keysDown[(int)key] == 1;

}
