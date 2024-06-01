#ifndef MYMEMORY_HH
#define MYMEMORY_HH

#include "carpwindow.h"

#define MAX_AMOUNT_INPUT 1024
typedef struct MyInput
{
    u8 _Alignas(16) keysDown[MAX_AMOUNT_INPUT];
    u8 _Alignas(16) keysHalfPress[MAX_AMOUNT_INPUT];
} MyInput;



typedef struct MyMemory
{
    CarpWindow _Alignas(16) window;
    MyInput _Alignas(16) input;

} MyMemory;

b8 myMemory_init();
void myMemory_deinit();
MyMemory* myMemory_get();

#endif