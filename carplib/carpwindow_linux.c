#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>


#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

static void s_destroyDisplay(CarpWindow* window);
static void s_destroyWindow(CarpWindow* window);


void destroyCarpWindow(CarpWindow* window);
bool s_initDisplay(CarpWindow* window);


struct
{
    Display* carpDisplay;
    Window carpWindowRoot;
    Window carpWindow;
    int carpWindowSrc;
} typedef CarpWindowInternal;


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

void carpWindow_destroy(CarpWindow* window)
{
    s_destroyWindow(window);
    s_destroyDisplay(window);
}

bool carpWindow_init(CarpWindow* window)
{
    return s_initDisplay(window);
}

void carpWindow_update(CarpWindow* window, f32 dt)
{

    XEvent event;
    while(XNextEvent(window.carpDisplay, &event) == 0)
    {
        switch(event.type)
        {
            case ButtonPress: printf("button pressed: %i\n",
                event.xbutton.button); break;


        }

    }

}