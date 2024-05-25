#ifndef MYMEMORY_HH
#define MYMEMORY_HH

#include "carpwindow.h"

typedef struct MyInput
{
    u8 keysDown[1024];
    u8 keysHalfPress[1024];
} MyInput;



typedef struct MyMemory
{
    CarpWindow window;
    MyInput input;

} MyMemory;

b8 myMemory_init();
void myMemory_deinit();
MyMemory* myMemory_get();

#endif