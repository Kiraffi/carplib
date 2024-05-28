#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "carpwindow.h"
#include "carpgl.h"
#include "mykey.h"

#include "mymemory.h"

#include <stdio.h>

#ifndef MAPVK_VSC_TO_VK
    #define MAPVK_VSC_TO_VK     (1)
#endif

static b8 s_quitRequested = false;
static b8 s_resized = false;

static const char className[] = "CarpWindow";

static LRESULT win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094


typedef HGLRC (APIENTRYP PFNCREATECONTEXTATTRIB_CARP)(HDC hdc, HGLRC hglrc, const s32* attribList);
PFNCREATECONTEXTATTRIB_CARP createContextAttribs = NULL;

typedef HGLRC (APIENTRYP PFNSWAPINTERVALEXT_CARP)(int);
PFNSWAPINTERVALEXT_CARP swapIntervalEXT = NULL;

typedef struct CarpWindowWin32
{
    HWND hwnd;
    HDC dc;
    HGLRC hglrc;
} CarpWindowWin32;

static b8 s_initWindow(CarpWindow* window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&window->data);

    WNDCLASSA wndclass = {0};

    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.lpfnWndProc = win32WndProc;
    wndclass.hInstance = GetModuleHandleA(NULL);
    wndclass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wndclass.hIcon = LoadIconA(NULL, IDI_WINLOGO);
    wndclass.lpszClassName = className;
    RegisterClassA(&wndclass);

    DWORD winStyle =
        WS_OVERLAPPED
        | WS_CAPTION
        | WS_THICKFRAME
        | WS_SYSMENU
        | WS_MINIMIZEBOX
        | WS_MAXIMIZEBOX;
    DWORD winExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    RECT rect = { .left = x, .top = y, .right = 0, .bottom = 0 };

    window->width = width;
    window->height = height;
    rect.right = width + x;
    rect.bottom = height + y;

    AdjustWindowRectEx(&rect, winStyle, false, winExStyle);

    DWORD winWidth = rect.right - rect.left;
    DWORD winHeight = rect.bottom - rect.top;

    wnd->hwnd = CreateWindowExA(winExStyle, className, windowName,
        winStyle, CW_USEDEFAULT, CW_USEDEFAULT, winWidth, winHeight,
        NULL, // hWndParent
        NULL, // hMenu
        GetModuleHandleA(NULL), // hInstance
        NULL // lParam
    );

    //ASSERT(wnd->hwnd != NULL);

    if (wnd->hwnd == NULL)
    {
        printf("Window handle is null!\n");
        return false;
    }

    ShowWindow(wnd->hwnd, SW_SHOW);
    wnd->dc = GetDC(wnd->hwnd);
    //ASSERT(wnd->dc != NULL);
    if (wnd->dc == NULL)
    {
        printf("Window dc is null!\n");
        return false;
    }
    printf("Window created!\n");

    return true;
}

static b8 s_initGL(CarpWindow* window)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&window->data);

    PIXELFORMATDESCRIPTOR pxFormatDesired = {0};
    pxFormatDesired.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pxFormatDesired.nVersion = 1;
    pxFormatDesired.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pxFormatDesired.iPixelType = PFD_TYPE_RGBA;
    pxFormatDesired.cColorBits = 32;
    pxFormatDesired.cAlphaBits = 8;
    pxFormatDesired.iLayerType = PFD_MAIN_PLANE;


    s32 suggestedPixelFormatIndex = ChoosePixelFormat(wnd->dc, &pxFormatDesired);
    PIXELFORMATDESCRIPTOR suggestedPxFormat = {0};

    s32 descriptorSuccess = DescribePixelFormat(wnd->dc, suggestedPixelFormatIndex,
        sizeof(PIXELFORMATDESCRIPTOR) , &suggestedPxFormat);

    if (descriptorSuccess == 0)
    {
        printf("Failed to describe pixel format!\n");
        return false;
    }

    if (SetPixelFormat(wnd->dc, suggestedPixelFormatIndex, &suggestedPxFormat) == 0)
    {
        printf("Failed to set pixel format!\n");
        return false;
    }

    HGLRC rc = wglCreateContext(wnd->dc);
    if(rc == 0)
    {
        printf("Failed to create opengl context.\n");
        return false;
    }

    wnd->hglrc = rc;

    // Check thread?
    if (wglMakeCurrent(wnd->dc, rc) == 0)
    {
        printf("Failed to make current opengl context.\n");
        return false;
    }
    // should check extensions support

    PROC proc = wglGetProcAddress("wglCreateContextAttribsARB\0");
    if (proc == 0)
    {
        printf("wglCreateContextAttribsARB not found!\n");
        return false;
    }

    createContextAttribs = (PFNCREATECONTEXTATTRIB_CARP)proc;

    PROC proc2 = wglGetProcAddress("wglSwapIntervalEXT\0");
    if (proc2 != 0)
    {

        swapIntervalEXT = (PFNSWAPINTERVALEXT_CARP)proc2;
    }

    int attrs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0, 0,
    };

    HGLRC shareContext = 0;

    HGLRC modernRC = createContextAttribs(wnd->dc, shareContext, attrs);
    if (modernRC != 0)
    {
        if (wglMakeCurrent(wnd->dc, modernRC) != 0)
        {
            wglDeleteContext(wnd->hglrc);
            wnd->hglrc = modernRC;
        }
    }
    int version = gladLoaderLoadGL();
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	//Checking GL version
	const GLubyte* GLVersionString = glGetString(GL_VERSION);
    printf("GL version: %s\n", GLVersionString);

    window->resized = true;




    return true;
}

static MyKey s_getKey(u32 key)
{
    s32 keyCode = (s32)(MapVirtualKeyA(key, MAPVK_VSC_TO_VK));
    MyKey result;
    switch(keyCode)
    {
        case VK_LEFT: result = MyKey_Left; break;
        case VK_UP: result = MyKey_Up; break;
        case VK_RIGHT: result = MyKey_Right; break;
        case VK_DOWN: result = MyKey_Down; break;
        case VK_ESCAPE: result = MyKey_Escape; break;
        case VK_RETURN: result = MyKey_Enter; break;
        case VK_F1: result = MyKey_F1; break;
        case VK_F2: result = MyKey_F2; break;
        case VK_F3: result = MyKey_F3; break;
        case VK_F4: result = MyKey_F4; break;
        case VK_F5: result = MyKey_F5; break;
        case VK_F6: result = MyKey_F6; break;
        case VK_F7: result = MyKey_F7; break;
        case VK_F8: result = MyKey_F8; break;
        case VK_F9: result = MyKey_F9; break;
        case VK_F10: result = MyKey_F10; break;
        case VK_F11: result = MyKey_F11; break;
        case VK_F12: result = MyKey_F12; break;

        case VK_CONTROL: result = MyKey_Ctrl; break;
        case VK_SHIFT: result = MyKey_Shift; break;
        case VK_MENU: result = MyKey_Alt; break;

        case VK_LCONTROL: result = MyKey_LCtrl; break;
        case VK_RCONTROL: result = MyKey_RCtrl; break;
        case VK_LMENU: result = MyKey_LAlt; break;
        case VK_RMENU: result = MyKey_RAlt; break;
        case VK_LSHIFT: result = MyKey_LShift; break;
        case VK_RSHIFT: result = MyKey_RShift; break;

        default:
        {
            if(keyCode >= 32 && keyCode < 128)
            {
                if(keyCode >= 97 && keyCode <= 122)
                    keyCode -= 32;
                result = (MyKey)(keyCode);
            }
            else
            {
                result = MyKey_InvalidKey;
            }
        }
    };

    return result;
}


static LRESULT win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_CLOSE:
        {
            s_quitRequested = true;
            PostQuitMessage(0);
            break;
        }

        /*
        WM_SIZE =>
        {
            let width = LOWORD(lParam as u32);
            let height = HIWORD(lParam as u32);

            println!("WM SIZE HERE: {}:{}, lparam: {}, wparam: {}", width, height, lParam, wParam);
        },
        WM_MOVE =>
        {
            let width = LOWORD(lParam as u32);
            let height = HIWORD(lParam as u32);

            println!("WM MOVE HERE: {}:{}", width, height);
        },
        WM_WINDOWPOSCHANGED =>
        {
            let width = LOWORD(lParam as u32);
            let height = HIWORD(lParam as u32);

            println!("WM pos changed: {}:{}", width, height);
        }
        */
        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            s8 down = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) ? 1 : 0;
            u32 button = (u32)(HIWORD((u32)lParam)) & ((u32)(0x1ff)); //0x1ff;
            s32 translated = s_getKey(button);
            if ((translated & 0x40000000) == 0x40000000)
            {
                translated = (translated & 0x1ff) + 512;
            }
            if( translated != MyKey_InvalidKey && translated < 1024)
            {
                MyMemory* mymemory = myMemory_get();
                mymemory->input.keysDown[translated] = down;
                mymemory->input.keysHalfPress[translated] += 1;
            }

            if (down && translated == VK_ESCAPE)
            {
                s_quitRequested = true;
            }
            break;
            //println!("button down: {}, b2: {}, transl: {}",  button, msg.wParam, transusize);
        };

        case WM_ENTERSIZEMOVE:
        {
            /*
            let width = LOWORD(lParam as u32);
            let height = HIWORD(lParam as u32);

            println!("WM pos sizemove start: {}:{}, lparam: {}, wparam: {}", width, height, lParam, wParam);
            */
        }

        case WM_EXITSIZEMOVE:
        {
            /*
            let width = LOWORD(lParam as u32);
            let height = HIWORD(lParam as u32);

            println!("WM pos sizemove end: {}:{}, lparam: {}, wparam: {}", width, height, lParam, wParam);
            */

            s_resized = true;
        };

        default:
        {
            if (!(
                (uMsg == WM_SIZE || uMsg == WM_MOVE || uMsg == WM_WINDOWPOSCHANGED) ||
                (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
                || (uMsg == SWP_FRAMECHANGED || uMsg == WM_NCHITTEST || uMsg == WM_NCMOUSELEAVE)
                || ( uMsg >= WM_NCMOUSEMOVE && uMsg <= WM_NCMBUTTONDBLCLK))
            )
            {
                //println!("window message: {}, lparam: {}, wparam: {}", uMsg, wParam, lParam);
            }
        }
    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}



b8 carpWindow_init(CarpWindow* window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    if(!s_initWindow(window, windowName, width, height, x, y))
    {
        printf("Window init failed\n");
        return false;
    }

    if(!s_initGL(window))
    {
        printf("GL init failed\n");
        return false;
    }
    return true;
}


void carpWindow_destroy(CarpWindow* window)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&window->data);

    gladLoaderUnloadGL();
}

b8 carpWindow_update(CarpWindow* window, f32 dt)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&window->data);

    MSG msg = {0};

    MyMemory* mymemory = myMemory_get();
    memset(mymemory->input.keysDown, 0, 1024);
    memset(mymemory->input.keysHalfPress, 0, 1024);

    while(PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE) != 0
        && window->running)
    {
        LONG t = GetMessageTime();
        PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        //printf("Message time: %u\n", (u32)t);
        b8 dispatch = false;

        switch(msg.message)
        {
            case WM_QUIT:
            {
                window->running = false;
                break;
            };

            case WM_KEYUP:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                s8 down = (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) ? 1 : 0;
                u32 button = (u32)(HIWORD((u32)msg.lParam)) & ((u32)(0x1ff)); //0x1ff;
                s32 translated = s_getKey(button);
                if ((translated & 0x40000000) == 0x40000000)
                {
                    translated = (translated & 0x1ff) + 512;
                }
                if( translated != MyKey_InvalidKey && translated < 1024)
                {
                    MyMemory* mymemory = myMemory_get();
                    mymemory->input.keysDown[translated] = down;
                    mymemory->input.keysHalfPress[translated] += 1;
                }
                else
                {
                    dispatch = true;
                }

                if (down && translated == VK_ESCAPE)
                {
                    s_quitRequested = true;
                    dispatch = true;
                }
                break;
                //println!("button down: {}, b2: {}, transl: {}",  button, msg.wParam, transusize);
            };
            deafult:
            {
                dispatch = true;
            }

        }

        //if(dispatch)
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    for(int i = 0; i < 1024; ++i)
    {
        if(mymemory->input.keysHalfPress[i] > 0)
        {
            printf("button: %i was pressed/released: %u times\n", i, mymemory->input.keysHalfPress[i]);
        }
    }

    if(s_resized)
    {
        RECT rect = {0};
        if (GetClientRect(wnd->hwnd, &rect) != 0)
        {
            DWORD width = rect.right - rect.left;
            DWORD height = rect.bottom - rect.top;

            if (window->width != width || window->height != height)
            {
                window->resized = true;
            }
            window->width = width;
            window->height = height;


            s_resized = false;
        }
    }
    if (s_quitRequested && window->running)
    {
        PostMessageA(wnd->hwnd, WM_CLOSE, 0, 0);
    }

    //window->running = false;
    return true;
}

void carpWindow_setWindowTitle(CarpWindow* window, const char* title)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&window->data);
    SetWindowTextA(wnd->hwnd, title);
}

void carpWindow_swapBuffers(CarpWindow* window)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&window->data);
    SwapBuffers(wnd->dc);
}
