#if !_WIN32

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>


#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include "../external/glad/gl.h"

#include "carpkeyboard.h"
#include "carplog.h"
#include "carpmouse.h"
#include "carptype.h"
#include "carpwindow.h"

#define BORDER_WIDTH 0

typedef GLXContext (*GLXCreateContextAttribs)(Display* dpy, GLXFBConfig config, GLXContext shareContext, bool directRendering, const int* attribList);
GLXCreateContextAttribs glxCreateContextAttribsFn = NULL;

typedef void (*GLXSwapIntervalEXT)(Display *dpy, GLXDrawable drawable, int interval);
GLXSwapIntervalEXT glxSwapIntervalEXTFn = NULL;


static void s_destroyDisplay(CarpWindow* carp_window);
static void s_destroyWindow(CarpWindow* carp_window);
static b8 s_initDisplay(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y);

typedef struct CarpWindowInternal
{
    Display* carpDisplay;
    WindowSizeChangedCallbackFn carpWindowSizeChangedCallbackFn;
    Window carpWindowRoot;
    Window carpWindow;
    Colormap carpColormap;
    int carpWindowScreen;
    Atom wmDeleteWindow;
    GLXContext carpGLContext;

} CarpWindowInternal;

_Static_assert(sizeof(CarpWindowInternal) <= sizeof(((CarpWindow*)0)->data), "CarpWindowInternal size is should be less than CarpWindow::data bytes!");



static CarpKeyboardKey s_getKeyFromSym(KeySym key)
{
    CarpKeyboardKey result = CarpKeyboardKey_Invalid;
    switch(key)
    {
        case XK_Left: result = CarpKeyboardKey_Left; break;
        case XK_Up: result = CarpKeyboardKey_Up; break;
        case XK_Right: result = CarpKeyboardKey_Right; break;
        case XK_Down: result = CarpKeyboardKey_Down; break;
        case XK_Escape: result = CarpKeyboardKey_Escape; break;
        case XK_Return: result = CarpKeyboardKey_Enter; break;
        case XK_F1: result = CarpKeyboardKey_F1; break;
        case XK_F2: result = CarpKeyboardKey_F2; break;
        case XK_F3: result = CarpKeyboardKey_F3; break;
        case XK_F4: result = CarpKeyboardKey_F4; break;
        case XK_F5: result = CarpKeyboardKey_F5; break;
        case XK_F6: result = CarpKeyboardKey_F6; break;
        case XK_F7: result = CarpKeyboardKey_F7; break;
        case XK_F8: result = CarpKeyboardKey_F8; break;
        case XK_F9: result = CarpKeyboardKey_F9; break;
        case XK_F10: result = CarpKeyboardKey_F10; break;
        case XK_F11: result = CarpKeyboardKey_F11; break;
        case XK_F12: result = CarpKeyboardKey_F12; break;

        case XK_Control_L: result = CarpKeyboardKey_LCtrl; break;
        case XK_Control_R: result = CarpKeyboardKey_RCtrl; break;
        case XK_Alt_L: result = CarpKeyboardKey_LAlt; break;
        case XK_Alt_R: result = CarpKeyboardKey_RAlt; break;
        case XK_Shift_L: result = CarpKeyboardKey_LShift; break;
        case XK_Shift_R: result = CarpKeyboardKey_RShift; break;

        default:
        {
            if(key >= XK_A && key < 128)
            {
                if(key >= XK_a && key <= XK_z)
                    key -= 32;
                result = (CarpKeyboardKey)(key);
            }
            else
            {
                result = CarpKeyboardKey_Invalid;
            }
        }
    };

    return result;
}

static KeySym s_getKeySym(CarpWindow* carp_window, XKeyEvent* event)
{
    KeySym sym;
    XLookupString(event, NULL, 0, &sym, NULL);
    return sym;
}



static void s_destroyDisplay(CarpWindow* carp_window)
{

    if(carp_window == NULL)
        return;
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);


    if(wnd->carpColormap != 0)
    {
        XFreeColormap(wnd->carpDisplay, wnd->carpColormap);
        wnd->carpColormap = 0;
    }
    if(wnd->carpGLContext != NULL)
    {
        glXDestroyContext(wnd->carpDisplay, wnd->carpGLContext);
        wnd->carpGLContext = NULL;
    }


    if(wnd->carpDisplay == NULL)
    {
        return;
    }
    XCloseDisplay(wnd->carpDisplay);
}
static void s_destroyWindow(CarpWindow* carp_window)
{
    if(carp_window == NULL)
        return;
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);
    XUnmapWindow(wnd->carpDisplay, wnd->carpWindow);
    XDestroyWindow(wnd->carpDisplay, wnd->carpWindow);
}


static b8 s_initDisplay(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    // TODO: Notice this, if using multisamples, should pass it as init?
    static const int multisample = 0;

    if(carp_window == NULL)
        return false;
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);
    wnd->carpDisplay = XOpenDisplay(NULL);
    if(wnd->carpDisplay == NULL)
    {
        CARP_LOGERROR("Failed to create display\n");
        s_destroyDisplay(carp_window);
        return false;
    }
    wnd->carpWindowScreen = DefaultScreen(wnd->carpDisplay);
    wnd->carpWindowRoot = RootWindow(wnd->carpDisplay, wnd->carpWindowScreen);


    int majorGlxVersion = 0;
    int minorGlxVersion = 0;

    int queryGlx  = glXQueryVersion(wnd->carpDisplay, &majorGlxVersion, &minorGlxVersion);
    CARP_LOGINFO("Query glx result: %i, major: %i, minor: %i\n", queryGlx, majorGlxVersion, minorGlxVersion);
    if(queryGlx == 0 || majorGlxVersion < 1 || (majorGlxVersion == 1 && minorGlxVersion < 4))
    {
        CARP_LOGERROR("Need to have glx 1.4 at least.\n");
        return false;
    }


    int attribs[] = {
        GLX_X_RENDERABLE    , 1,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , 1,
        0,0
    };

    int fbCount = 0;
    GLXFBConfig* fbConfigs = glXChooseFBConfig(wnd->carpDisplay, wnd->carpWindowScreen, attribs, &fbCount);
    if(fbConfigs == NULL)
    {
        CARP_LOGERROR("Failed to retrieve framebuffer.\n");
        return false;
    }

    CARP_LOGINFO("Found %i matching framebuffers.\n", fbCount);

    int bestFBConfigIndex =  -1;
    int bestNumSamp = -1;

    bool foundSRGB = false;
    bool foundMultisample = false;

    for(int i = 0; i < fbCount; ++i)
    {
        GLXFBConfig fbConf = fbConfigs[i];
        XVisualInfo* visualInfoTmp = glXGetVisualFromFBConfig( wnd->carpDisplay, fbConf);

        if(visualInfoTmp != NULL)
        {
            int sampBuf = 0;
            int samples = 0;
            int srgb = 0;

            glXGetFBConfigAttrib( wnd->carpDisplay, fbConf, GLX_SAMPLE_BUFFERS, &sampBuf );
            glXGetFBConfigAttrib( wnd->carpDisplay, fbConf, GLX_SAMPLES, &samples  );
            glXGetFBConfigAttrib( wnd->carpDisplay, fbConf, GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, &srgb);

            if(bestFBConfigIndex < 0 || (srgb > 0 && !foundSRGB)
                || (srgb > 0 && multisample == samples))
            {
                foundSRGB = srgb > 0;
                bestFBConfigIndex = i;
                bestNumSamp = samples;
            }
        }
        XFree(visualInfoTmp);
    }
    CARP_LOGINFO("Best visual info index: %i\n", bestFBConfigIndex);
    GLXFBConfig bestFBConfig = fbConfigs[bestFBConfigIndex];
    XFree(fbConfigs); // Make sure to free this!

    XVisualInfo* visualInfo = glXGetVisualFromFBConfig(wnd->carpDisplay, bestFBConfig);
    if(visualInfo == NULL)
    {
        CARP_LOGERROR("Could not create correct visualInfo carp_window.\n");
        return false;
    }

    if(visualInfo->visual == NULL)
    {
        CARP_LOGERROR("Could not create correct visualInfo->visual.\n");
        XFree(visualInfo);
        return false;
    }

    if(wnd->carpWindowScreen != visualInfo->screen)
    {
        CARP_LOGERROR("screenId(%i) does not match visualInfo->screen(%i)", wnd->carpWindowScreen, visualInfo->screen);
        XFree(visualInfo);
        return false;
    }

    XSetWindowAttributes xwa = {0};
    xwa.background_pixel = WhitePixel(wnd->carpDisplay, wnd->carpWindowScreen);
    xwa.border_pixel = BlackPixel(wnd->carpDisplay, wnd->carpWindowScreen);
    xwa.event_mask =
        StructureNotifyMask
            | SubstructureRedirectMask | SubstructureNotifyMask
            //| ResizeRedirectMask
            | KeyPressMask | KeyReleaseMask
            | ButtonPressMask | ButtonReleaseMask
            | ExposureMask;
    xwa.colormap = XCreateColormap(wnd->carpDisplay, wnd->carpWindowRoot, visualInfo->visual, AllocNone);

    wnd->carpWindow = XCreateWindow(
        wnd->carpDisplay,
        wnd->carpWindowRoot,
        x,
        y,
        width,
        height,
        BORDER_WIDTH,
        visualInfo->depth, //DefaultDepth(wnd->carpDisplay, wnd->carpWindowScreen),
        InputOutput,
        visualInfo->visual, // DefaultVisual(wnd->carpDisplay, wnd->carpWindowScreen),
        CWBackPixel | CWColormap  | CWBorderPixel | CWEventMask | CWBorderPixel,
        &xwa);

    XFree(visualInfo);

    int contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0, 0
    };


    glxCreateContextAttribsFn = (GLXCreateContextAttribs)glXGetProcAddress((const GLubyte*)("glXCreateContextAttribsARB"));
    if(glxCreateContextAttribsFn == NULL)
    {
        CARP_LOGERROR("Couldnt find glXCreateContextAttribsARB function\n");
        return false;
    }
    glxSwapIntervalEXTFn = (GLXSwapIntervalEXT)glXGetProcAddress((const GLubyte*)("glXSwapIntervalEXT"));
    if(glxSwapIntervalEXTFn == NULL)
    {
        CARP_LOGERROR("Could not find glXSwapIntervalEXT function\n");
    }

    wnd->carpGLContext = glxCreateContextAttribsFn(wnd->carpDisplay, bestFBConfig, NULL, true, contextAttribs);
    if(wnd->carpGLContext == NULL)
    {
        CARP_LOGERROR("Failed to create opengl context. Might need some cleaning up?\n");
        return false;
    }

    // sync
    XSync(wnd->carpDisplay, false);

    CARP_LOGINFO("make current: %i\n", glXMakeCurrent(wnd->carpDisplay, wnd->carpWindow, wnd->carpGLContext));


    // Handle pressing X-button on carp_window
    wnd->wmDeleteWindow = XInternAtom(wnd->carpDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(wnd->carpDisplay, wnd->carpWindow, &wnd->wmDeleteWindow, 1);


    CARP_LOGINFO("Clear: %i\n", XClearWindow(wnd->carpDisplay, wnd->carpWindow));
    CARP_LOGINFO("xmap raised: %i\n", XMapRaised(wnd->carpDisplay, wnd->carpWindow));

    carp_window_setWindowTitle(carp_window, windowName);

    int version = gladLoaderLoadGL();
    CARP_LOGINFO("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	//Checking GL version
	const GLubyte* GLVersionString = glGetString(GL_VERSION);
    CARP_LOGINFO("GL version: %s\n", GLVersionString);


    return true;
}




CARP_FN void carp_window_destroy(CarpWindow* carp_window)
{
    s_destroyWindow(carp_window);
    s_destroyDisplay(carp_window);
}

CARP_FN b8 carp_window_init(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    return s_initDisplay(carp_window, windowName, width, height, x, y);
}

CARP_FN b8 carp_window_update(CarpWindow* carp_window, f32 dt)
{

    if(carp_window == NULL)
        return false;
    carp_keyboard_resetState();
    carp_mouse_resetState();

    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);
    int minKeyCodes = 0;
    int maxKeyCodes = 0;
    XDisplayKeycodes(wnd->carpDisplay, &minKeyCodes, &maxKeyCodes);

    XEvent event;
    while(XPending(wnd->carpDisplay))
    {
        XNextEvent(wnd->carpDisplay, &event);
        switch(event.type)
        {
            case KeyPress:
            {
                KeySym keysym = s_getKeyFromSym(s_getKeySym(carp_window, &event.xkey));
                carp_keyboard_setKeyState(keysym, true);

                break;
            }
            case KeyRelease:
            {
                KeySym keysym = s_getKeyFromSym(s_getKeySym(carp_window, &event.xkey));
                carp_keyboard_setKeyState(keysym, false);
                break;
            }
            case ButtonPress:
            case ButtonRelease:
            {
                bool down = event.type == ButtonPress;
                switch(event.xbutton.button)
                {
                    case Button1: carp_mouse_setButtonState(CarpMouseButton_Left, down); break;
                    case Button2: carp_mouse_setButtonState(CarpMouseButton_Middle, down); break;
                    case Button3: carp_mouse_setButtonState(CarpMouseButton_Right, down); break;
                    case Button5 + 3: carp_mouse_setButtonState(CarpMouseButton_Button4, down); break;
                    case Button5 + 4: carp_mouse_setButtonState(CarpMouseButton_Button5, down); break;

                    case Button4: carp_mouse_addWheelMovement(1, 0); break;
                    case Button5: carp_mouse_addWheelMovement(-1, 0); break;
                    case Button5 + 1: carp_mouse_addWheelMovement(0, 1); break;
                    case Button5 + 2: carp_mouse_addWheelMovement(0, -1); break;

                    default: break;
                }
                break;
            }

            case ClientMessage:
            {
                if ((Atom)event.xclient.data.l[0] == wnd->wmDeleteWindow)
                    carp_window->running = false;
                break;
            }
            case ConfigureNotify:
            {
                if(carp_window->width != event.xconfigure.width
                    || carp_window->height != event.xconfigure.height)
                {
                    carp_window->width = event.xconfigure.width;
                    carp_window->height = event.xconfigure.height;
                    if(wnd->carpWindowSizeChangedCallbackFn)
                    {
                        wnd->carpWindowSizeChangedCallbackFn(carp_window->width, carp_window->height);
                    }
                }
                break;
            }
            case DestroyNotify:
            {
                carp_window->running = false;
                break;
            }
        }

    }
    int mousePosX = 0;
    int mousePosY = 0;
    int mouseWindowPosX = 0;
    int mouseWindowPosY = 0;

    unsigned int tmpMask = 0;

    Window tmpWindow1 = {0};
    Window tmpWindow2 = {0};


    if(XQueryPointer(wnd->carpDisplay, wnd->carpWindow, &tmpWindow1, &tmpWindow2,
        &mousePosX, &mousePosY,
        &mouseWindowPosX, &mouseWindowPosY,
        &tmpMask))
    {
        if(mouseWindowPosX >= 0
            && mouseWindowPosY >= 0
            && mouseWindowPosX < carp_window->width
            && mouseWindowPosY < carp_window->height
        )
        {
            carp_mouse_setPosition(mouseWindowPosX, mouseWindowPosY);
        }
    }
    return true;
}

CARP_FN void carp_window_setWindowTitle(CarpWindow* carp_window, const char* title)
{
    if(carp_window == NULL || title == NULL)
    {
        return;
    }
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);
    XStoreName(wnd->carpDisplay, wnd->carpWindow, title);
}

CARP_FN void carp_window_swapBuffers(CarpWindow* carp_window)
{
    if(carp_window == NULL)
    {
        return;
    }
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);
    glXSwapBuffers(wnd->carpDisplay, wnd->carpWindow);
}
CARP_FN void carp_window_enableVSync(CarpWindow* carp_window, bool vSyncEnabled)
{
    if(carp_window == NULL || glxSwapIntervalEXTFn == NULL)
    {
        return;
    }
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);
    glxSwapIntervalEXTFn(wnd->carpDisplay, wnd->carpWindow, vSyncEnabled ? 1 : 0);
}


 CARP_FN void carp_window_setWindowSizeChangedCallbackFn(
    CarpWindow* carp_window,
    WindowSizeChangedCallbackFn windowSizeChangedCallbackFn)
{
    if(carp_window == NULL)
    {
        return;
    }
    CarpWindowInternal* wnd = (CarpWindowInternal*)(&carp_window->data);
    wnd->carpWindowSizeChangedCallbackFn = windowSizeChangedCallbackFn;
}




#endif // !_WIN32
