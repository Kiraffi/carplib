


#if _MSC_VER > 0
    #include "carplib/carpwindow_win32.c"
#else
    #include "carplib/carpwindow_linux.c"
#endif 

#include "carplib/carpwindow.h"

#include "carplib/mymemory.h"

#include "carplib/mymemory.c"
#include "carplib/carpgl.h"
#include "carplib/carpgl.c"


//#include <malloc.h> // malloc, otherwise tcc will coredump
#include <stdalign.h> //alignof
#include <stdbool.h> //bool
#include <stdio.h> //printf
#include <stdlib.h> //free
//#include <stdatomic.h>
#include <stdint.h>



#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_POS_X 500
#define WINDOW_POS_Y 500
#define BORDER_WIDTH 0



s32 theMain()
{
    if(!myMemory_init())
    {
        printf("Failed to init the memory\n");
        return -1;
    }
    MyMemory* memory = myMemory_get();

    if(!carpWindow_init(&memory->window, "My window", WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        myMemory_deinit();
        return -1;
    }
    memory->window.running = true;
    while(memory->window.running)
    {
        carpWindow_update(&memory->window, 0.0f);

        

    }

    carpWindow_destroy(&memory->window);
    myMemory_deinit();
    return 0;
}

int main(int argc, char** argv)
{
    return theMain();
}
