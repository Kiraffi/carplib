#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>


#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include <stdio.h>

#include "mykey.h"
#include "mytype.h"
#include "carpwindow.h"

#define BORDER_WIDTH 0

static void s_destroyDisplay(CarpWindow* window);
static void s_destroyWindow(CarpWindow* window);
static b8 s_initDisplay(CarpWindow* window, const char* windowName, s32 width, s32 height, s32 x, s32 y);

typedef struct CarpWindowInternal
{
    Display* carpDisplay;
    Window carpWindowRoot;
    Window carpWindow;
    int carpWindowSrc;
     Atom wmDeleteWindow;

} CarpWindowInternal;




static MyKey s_getKeyFromSym(KeySym key)
{
    MyKey result = MyKey_InvalidKey;
    switch(key)
    {
        case XK_Left: result = MyKey_Left; break;
        case XK_Up: result = MyKey_Up; break;
        case XK_Right: result = MyKey_Right; break;
        case XK_Down: result = MyKey_Down; break;
        case XK_Escape: result = MyKey_Escape; break;
        case XK_Return: result = MyKey_Enter; break;
        case XK_F1: result = MyKey_F1; break;
        case XK_F2: result = MyKey_F2; break;
        case XK_F3: result = MyKey_F3; break;
        case XK_F4: result = MyKey_F4; break;
        case XK_F5: result = MyKey_F5; break;
        case XK_F6: result = MyKey_F6; break;
        case XK_F7: result = MyKey_F7; break;
        case XK_F8: result = MyKey_F8; break;
        case XK_F9: result = MyKey_F9; break;
        case XK_F10: result = MyKey_F10; break;
        case XK_F11: result = MyKey_F11; break;
        case XK_F12: result = MyKey_F12; break;

        case XK_Control_L: result = MyKey_LCtrl; break;
        case XK_Control_R: result = MyKey_RCtrl; break;
        case XK_Alt_L: result = MyKey_LAlt; break;
        case XK_Alt_R: result = MyKey_RAlt; break;
        case XK_Shift_L: result = MyKey_LShift; break;
        case XK_Shift_R: result = MyKey_RShift; break;

        default:
        {
            if(key >= XK_A && key < 128)
            {
                if(key >= XK_a && key <= XK_z)
                    key -= 32;
                result = (MyKey)(key);
            }
            else
            {
                result = MyKey_InvalidKey;
            }
        }
    };

    return result;
}



static void s_destroyDisplay(CarpWindow* window)
{

    if(window == NULL)
        return;
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&window->data);
    if(wnd->carpDisplay == NULL)
    {
        return;
    }
    XCloseDisplay(wnd->carpDisplay);
}
static void s_destroyWindow(CarpWindow* window)
{
    if(window == NULL)
        return;
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&window->data);
    XUnmapWindow(wnd->carpDisplay, wnd->carpWindow);
    XDestroyWindow(wnd->carpDisplay, wnd->carpWindow);
}


static b8 s_initDisplay(CarpWindow* window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&window->data);
    wnd->carpDisplay = XOpenDisplay(NULL);
    if(wnd->carpDisplay == NULL)
    {
        printf("Failed to create display\n");
        s_destroyDisplay(window);
        return false;
    }
    wnd->carpWindowSrc = DefaultScreen(wnd->carpDisplay);
    wnd->carpWindowRoot = RootWindow(wnd->carpDisplay, wnd->carpWindowSrc);

    XSetWindowAttributes xwa;
    xwa.background_pixel = WhitePixel(wnd->carpDisplay, wnd->carpWindowSrc);
    xwa.border_pixel = BlackPixel(wnd->carpDisplay, wnd->carpWindowSrc);
    xwa.event_mask = ButtonPressMask;

    wnd->carpWindow = XCreateWindow(
        wnd->carpDisplay,
        wnd->carpWindowRoot,
        x,
        y,
        width,
        height,
        BORDER_WIDTH,
        DefaultDepth(wnd->carpDisplay, wnd->carpWindowSrc),
        InputOutput,
        DefaultVisual(wnd->carpDisplay, wnd->carpWindowSrc),
        CWBackPixel | CWEventMask | CWBorderPixel,
        &xwa);

    XSelectInput(wnd->carpDisplay,
        wnd->carpWindow,
        StructureNotifyMask | SubstructureRedirectMask | SubstructureNotifyMask
            | ResizeRedirectMask | KeyPressMask | KeyReleaseMask
            | ButtonPressMask | ButtonReleaseMask);

    XMapWindow(wnd->carpDisplay, wnd->carpWindow);

    // Handle pressing X-button on window
    wnd->wmDeleteWindow = XInternAtom(wnd->carpDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(wnd->carpDisplay, wnd->carpWindow, &wnd->wmDeleteWindow, 1);

    return true;
}

void carpWindow_destroy(CarpWindow* window)
{
    s_destroyWindow(window);
    s_destroyDisplay(window);
}

b8 carpWindow_init(CarpWindow* window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    return s_initDisplay(window, windowName, width, height, x, y);
}

KeySym s_getKeySym(CarpWindow* window, XKeyEvent* event)
{
    KeySym sym;
    XLookupString(event, NULL, 0, &sym, NULL);
    return sym;
}

b8 carpWindow_update(CarpWindow* window, f32 dt)
{
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&window->data);
    int minKeyCodes = 0;
    int maxKeyCodes = 0;
    XDisplayKeycodes(wnd->carpDisplay, &minKeyCodes, &maxKeyCodes);

    // reminder for keyboard symbols: XK_Escape
    XEvent event;
    while(XPending(wnd->carpDisplay))
    {
        XNextEvent(wnd->carpDisplay, &event);
        switch(event.type)
        {
            case KeyPress: printf("Key pressed: %i, state: %i, symbol: %i\n",
                event.xkey.keycode, event.xkey.state, (int)s_getKeySym(window, &event.xkey));
                if(s_getKeyFromSym(s_getKeySym(window, &event.xkey)) == MyKey_Escape)
                    window->running = false;
                break;
            case KeyRelease: printf("Key released: %i, state: %i, symbol: %i\n",
                event.xkey.keycode, event.xkey.state, (int)s_getKeySym(window, &event.xkey));
                break;
            case ButtonPress: printf("Mouse button pressed: %i\n", event.xbutton.button);
                break;
            case ButtonRelease: printf("Mouse button released: %i\n", event.xbutton.button);
                break;
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == wnd->wmDeleteWindow)
                    window->running = false;
                break;

            case DestroyNotify:
                window->running = false;
                break;
        }

    }

    return true;
}