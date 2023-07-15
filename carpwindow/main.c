
#include "carpwindow_linux.c"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>


#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include <malloc.h> // malloc, otherwise tcc will coredump
#include <stdalign.h> //alignof
#include <stdbool.h> //bool
#include <stdio.h> //printf
#include <stdlib.h> //free
//#include <stdatomic.h>
#include <stdint.h>

// linux
#include <pthread.h>

typedef char b8;

#define BIG 10000000UL
uint64_t counter = 0;
//atomic_int atint;

void countToBig()
{
    for(uint64_t i = 0; i < BIG; ++i)
        counter += i;
}

void *threadfunc(void *arg)
{
    countToBig();
    return NULL;
}

typedef struct data_t
{
    char x;
    char _Alignas(128) cacheline[128]; // over-aligned array of char,
                                      // not array of over-aligned chars
} data;
typedef struct sse_t
{
    float _Alignas(16) sse_data[4];
} sse_tt;


struct
{
    Display* carpDisplay;
    Window carpWindowRoot;
    Window carpWindow;
    int carpWindowSrc;
} typedef CarpWindow;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_POS_X 500
#define WINDOW_POS_Y 500
#define BORDER_WIDTH 0


static void s_destroyDisplay(CarpWindow* window);
static void s_destroyWindow(CarpWindow* window);


void destroyCarpWindow(CarpWindow* window);
bool s_initDisplay(CarpWindow* window);


static void s_destroyDisplay(CarpWindow* window)
{
    if(window == NULL || window->carpDisplay == NULL)
        return;
    XCloseDisplay(window->carpDisplay);
}
static void s_destroyWindow(CarpWindow* window)
{
    if(window == NULL)
        return;
    XUnmapWindow(window->carpDisplay, window->carpWindow);
    XDestroyWindow(window->carpDisplay, window->carpWindow);
}


void destroyCarpWindow(CarpWindow* window)
{
    s_destroyWindow(window);
    s_destroyDisplay(window);
}

bool s_initDisplay(CarpWindow* window)
{
    window->carpDisplay = XOpenDisplay(NULL);
    if(window->carpDisplay == NULL)
    {
        printf("Failed to create display\n");
        s_destroyDisplay(window);
        return false;
    }
    window->carpWindowSrc = DefaultScreen(window->carpDisplay);
    window->carpWindowRoot = RootWindow(window->carpDisplay, window->carpWindowSrc);

    XSetWindowAttributes xwa;
    xwa.background_pixel = WhitePixel(window->carpDisplay, window->carpWindowSrc);
    xwa.border_pixel = BlackPixel(window->carpDisplay, window->carpWindowSrc);
    xwa.event_mask = ButtonPressMask;

    window->carpWindow = XCreateWindow(
        window->carpDisplay,
        window->carpWindowRoot,
        WINDOW_POS_X,
        WINDOW_POS_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        BORDER_WIDTH,
        DefaultDepth(window->carpDisplay, window->carpWindowSrc),
        InputOutput,
        DefaultVisual(window->carpDisplay, window->carpWindowSrc),
        CWBackPixel | CWEventMask | CWBorderPixel,
        &xwa);

    XMapWindow(window->carpDisplay, window->carpWindow);

    return true;
}

int main(int argc, char** argv)
{
    pthread_t t1, t2;
    pthread_create(&t1, NULL, threadfunc, NULL);
    pthread_create(&t2, NULL, threadfunc, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Result is: %lu\n", counter);

    CarpWindow window = {0};
    if(window.carpDisplay == NULL) { printf("Works\n"); }
    XEvent event;

   printf("sizeof(data) = %zu (1 byte + 127 bytes padding + 128-byte array)\n",
           sizeof(data));

    printf("alignment of sse_t is %zu\n", alignof(sse_tt));
    printf("alignment of data is %zu\n", alignof(data));

    static const int ALLOCSIZE = 1024 * 1024 * 1024;
    int *p1 = malloc(ALLOCSIZE);
    for(int i = 0; i < ALLOCSIZE / sizeof(int); ++i)
    {
        p1[i] = i;
    }
    printf("Value: %i\n", p1[4546]);
    printf("default-aligned addr:   %p\n", (void*)p1);

    //int* p2 = aligned_alloc(256, 1024 * 1024 * 1024);
    //printf("1024-byte aligned addr: %p\n", (void*)p2);
    //free(p2);
    free(p1);

    if(!s_initDisplay(&window))
    {
        return -1;
    }
    while(XNextEvent(window.carpDisplay, &event) == 0)
    {
        switch(event.type)
        {
            case ButtonPress: printf("button pressed: %i\n",
                event.xbutton.button); break;


        }

    }

    destroyCarpWindow(&window);
    return 0;
}