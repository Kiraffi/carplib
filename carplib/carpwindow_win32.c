#if _WIN32

#include "carpwindow.h"

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "carpgl.h"
#include "carpkeyboard.h"
#include "carpmemory.h"
#include "carpmouse.h"

#include <stdio.h>

#ifndef MAPVK_VSC_TO_VK
    #define MAPVK_VSC_TO_VK     (1)
#endif

#ifndef WM_MOUSEHWHEEL
    #define WM_MOUSEHWHEEL 0x020E
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
PFNCREATECONTEXTATTRIB_CARP winCreateContextAttribs = NULL;

typedef HGLRC (APIENTRYP PFNSWAPINTERVALEXT_CARP)(int);
PFNSWAPINTERVALEXT_CARP winSwapIntervalEXTFn = NULL;

typedef struct CarpWindowWin32
{
    HWND hwnd;
    HDC dc;
    HGLRC hglrc;
    WindowSizeChangedCallbackFn carpWindowSizeChangedCallbackFn;

} CarpWindowWin32;

static b8 s_initWindow(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&carp_window->data);

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

    carp_window->width = width;
    carp_window->height = height;
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

static b8 s_initGL(CarpWindow* carp_window)
{
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&carp_window->data);

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

    winCreateContextAttribs = (PFNCREATECONTEXTATTRIB_CARP)wglGetProcAddress("wglCreateContextAttribsARB\0");
    if (winCreateContextAttribs == NULL)
    {
        printf("wglCreateContextAttribsARB not found!\n");
        return false;
    }

    winSwapIntervalEXTFn = (PFNSWAPINTERVALEXT_CARP)wglGetProcAddress("wglSwapIntervalEXT\0");

    int attrs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0, 0,
    };

    HGLRC shareContext = 0;

    HGLRC modernRC = winCreateContextAttribs(wnd->dc, shareContext, attrs);
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

    carp_window->resized = true;




    return true;
}

static CarpKeyboardKey s_getKey(u32 key)
{
    s32 keyCode = (s32)(MapVirtualKeyA(key, MAPVK_VSC_TO_VK));
    CarpKeyboardKey result;
    switch(keyCode)
    {
        case VK_LEFT: result = CarpKeyboardKey_Left; break;
        case VK_UP: result = CarpKeyboardKey_Up; break;
        case VK_RIGHT: result = CarpKeyboardKey_Right; break;
        case VK_DOWN: result = CarpKeyboardKey_Down; break;
        case VK_ESCAPE: result = CarpKeyboardKey_Escape; break;
        case VK_RETURN: result = CarpKeyboardKey_Enter; break;
        case VK_F1: result = CarpKeyboardKey_F1; break;
        case VK_F2: result = CarpKeyboardKey_F2; break;
        case VK_F3: result = CarpKeyboardKey_F3; break;
        case VK_F4: result = CarpKeyboardKey_F4; break;
        case VK_F5: result = CarpKeyboardKey_F5; break;
        case VK_F6: result = CarpKeyboardKey_F6; break;
        case VK_F7: result = CarpKeyboardKey_F7; break;
        case VK_F8: result = CarpKeyboardKey_F8; break;
        case VK_F9: result = CarpKeyboardKey_F9; break;
        case VK_F10: result = CarpKeyboardKey_F10; break;
        case VK_F11: result = CarpKeyboardKey_F11; break;
        case VK_F12: result = CarpKeyboardKey_F12; break;

        case VK_CONTROL: result = CarpKeyboardKey_Ctrl; break;
        case VK_SHIFT: result = CarpKeyboardKey_Shift; break;
        case VK_MENU: result = CarpKeyboardKey_Alt; break;

        case VK_LCONTROL: result = CarpKeyboardKey_LCtrl; break;
        case VK_RCONTROL: result = CarpKeyboardKey_RCtrl; break;
        case VK_LMENU: result = CarpKeyboardKey_LAlt; break;
        case VK_RMENU: result = CarpKeyboardKey_RAlt; break;
        case VK_LSHIFT: result = CarpKeyboardKey_LShift; break;
        case VK_RSHIFT: result = CarpKeyboardKey_RShift; break;
        case VK_XBUTTON1: result = CarpKeyboardKey_MouseXButton1; break;
        case VK_XBUTTON2: result = CarpKeyboardKey_MouseXButton2; break;

        default:
        {
            if(keyCode >= 32 && keyCode < 128)
            {
                if(keyCode >= 97 && keyCode <= 122)
                    keyCode -= 32;
                result = (CarpKeyboardKey)(keyCode);
            }
            else
            {
                result = CarpKeyboardKey_Invalid;
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

            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_XBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            case WM_XBUTTONUP:
            {
                bool down = false;
                CarpMouseButton mouseButton = CarpMouseButton_Invalid;
                switch(uMsg)
                {
                    case WM_LBUTTONDOWN:
                    case WM_MBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                    case WM_XBUTTONDOWN:
                        down = true;
                        break;
                    default:
                        break;
                }

                switch(uMsg)
                {
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONUP:
                        mouseButton = CarpMouseButton_Left;
                        break;
                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONUP:
                        mouseButton = CarpMouseButton_Middle;
                        break;
                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONUP:
                        mouseButton = CarpMouseButton_Right;
                        break;
                    case WM_XBUTTONDOWN:
                    case WM_XBUTTONUP:
                    {
                        u32 button = (u32)(HIWORD((u32)lParam)) & ((u32)(0x1ff)); //0x1ff;
                        s32 translated = s_getKey(button);
                        if(translated == CarpKeyboardKey_MouseXButton2)
                            mouseButton = CarpMouseButton_Button5;
                        else
                            mouseButton = CarpMouseButton_Button4;

                        break;
                    }
                    default:
                        break;
                }
                carp_mouse_setButtonState(mouseButton, down);
                //dispatch = true;
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
            if( translated != CarpKeyboardKey_Invalid && translated < 1024)
            {
                printf("key: %i\n", translated);
                carp_keyboard_setKeyState((CarpKeyboardKey)translated, down);
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
            //int width = LOWORD(lParam);
            //int height = HIWORD(lParam);
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
                //println!("carp_window message: {}, lparam: {}, wparam: {}", uMsg, wParam, lParam);
            }
        }
    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}



b8 carp_window_init(CarpWindow* carp_window, const char* windowName, s32 width, s32 height, s32 x, s32 y)
{
    if(carp_window == NULL)
    {
        return false;
    }

    if(!s_initWindow(carp_window, windowName, width, height, x, y))
    {
        printf("Window init failed\n");
        return false;
    }

    if(!s_initGL(carp_window))
    {
        printf("GL init failed\n");
        return false;
    }
    return true;
}


void carp_window_destroy(CarpWindow* carp_window)
{
    if(carp_window == NULL)
    {
        return;
    }

    gladLoaderUnloadGL();
}

b8 carp_window_update(CarpWindow* carp_window, f32 dt)
{
    if(carp_window == NULL)
    {
        return false;
    }
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&carp_window->data);

    MSG msg = {0};

    carp_keyboard_resetState();
    carp_mouse_resetState();

    while(PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE) != 0
        && carp_window->running)
    {
        //LONG t = GetMessageTime();
        PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        //printf("Message time: %u\n", (u32)t);
        //b8 dispatch = false;
        switch(msg.message)
        {
            case WM_QUIT:
            {
                carp_window->running = false;
                break;
            };



            case WM_MOUSEWHEEL:
            {
                int16_t wheelAmount = HIWORD(msg.wParam);
                carp_mouse_addWheelMovement(0, wheelAmount);
                break;
            }
            case WM_MOUSEHWHEEL:
            {
                int16_t wheelAmount = HIWORD(msg.wParam);
                carp_mouse_addWheelMovement(wheelAmount, 0);
                break;
            }

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
                if( translated != CarpKeyboardKey_Invalid && translated < 1024)
                {
                    carp_keyboard_setKeyState((CarpKeyboardKey)translated, down);
                }
                else
                {
                    //dispatch = true;
                }

                break;
                //println!("button down: {}, b2: {}, transl: {}",  button, msg.wParam, transusize);
            };
            //deafult:
            //{
            //    dispatch = true;
            //}

        }

        //if(dispatch)
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    if(s_resized)
    {
        RECT rect = {0};
        if (GetClientRect(wnd->hwnd, &rect) != 0)
        {
            DWORD width = rect.right - rect.left;
            DWORD height = rect.bottom - rect.top;

            if (carp_window->width != width || carp_window->height != height)
            {
                carp_window->resized = true;
                carp_window->width = width;
                carp_window->height = height;

                if(wnd->carpWindowSizeChangedCallbackFn)
                {
                    wnd->carpWindowSizeChangedCallbackFn(width, height);
                }
            }


            s_resized = false;
        }
    }
    if (s_quitRequested && carp_window->running)
    {
        PostMessageA(wnd->hwnd, WM_CLOSE, 0, 0);
    }

    POINT mousePos;
    GetCursorPos(&mousePos);

    if (ScreenToClient(wnd->hwnd, &mousePos))
    {
        if(mousePos.x >= 0
            && mousePos.y >= 0
            && mousePos.x < carp_window->width
            && mousePos.y < carp_window->height
        )
        {
            carp_mouse_setPosition(mousePos.x, mousePos.y);
        }
    }

    return true;
}

void carp_window_setWindowTitle(CarpWindow* carp_window, const char* title)
{
    if(carp_window == NULL)
    {
        return;
    }
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&carp_window->data);
    SetWindowTextA(wnd->hwnd, title);
}

void carp_window_swapBuffers(CarpWindow* carp_window)
{
    if(carp_window == NULL)
    {
        return;
    }
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&carp_window->data);
    SwapBuffers(wnd->dc);
}

void carp_window_enableVSync(CarpWindow* carp_window, bool vSyncEnabled)
{
    if(carp_window == NULL || winSwapIntervalEXTFn == NULL)
    {
        return;
    }
    winSwapIntervalEXTFn(vSyncEnabled ? 1 : 0);
}


void carp_window_setWindowSizeChangedCallbackFn(
    CarpWindow* carp_window,
    WindowSizeChangedCallbackFn windowSizeChangedCallbackFn)
{
    if(carp_window == NULL)
    {
        return;
    }
    CarpWindowWin32* wnd = (CarpWindowWin32*)(&carp_window->data);
    wnd->carpWindowSizeChangedCallbackFn = windowSizeChangedCallbackFn;
}




#endif // _WIN32
