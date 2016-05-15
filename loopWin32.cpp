#include "FrameBuffer.h"
#include "loop.h"
#include "WindowInput.h"
#include "IApplication.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Global.h"
#include <windows.h>

static HDC memoryDC;
static int screewidth,screeheight;

void* srSocketEvent(void){
    static HANDLE sockEvent=CreateEvent(
        NULL,         // default security attributes
        FALSE,         // auto-reset event object
        FALSE,         // initial state is not signaled
        NULL  // object name
        );
    return sockEvent;
}

static inline void get_xy(LPARAM lParam, int *x, int *y)
{
	*x = (short)(lParam & 0xffff);
	*y = (short)((lParam>>16) & 0xffff);
}

static inline toWKey(WPARAM p){
    if(p == MK_CONTROL) return WindowInput::MODIFIER_CONTROL;
    else if(p == MK_SHIFT) return WindowInput::MODIFIER_SHIFT;
    else return WindowInput::MODIFIER_BUTTON;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_CREATE:
    {
        WindowInput::OnInitialize();
    } break;
    case WM_PAINT:
    {
        if (GetUpdateRect(hWnd, NULL, FALSE))
        {
            HDC hDC = GetDC(hWnd);
            BitBlt(hDC, 0, 0, screewidth,screeheight,memoryDC, 0, 0, SRCCOPY);
            ValidateRect(hWnd, NULL);
            ReleaseDC(hWnd, hDC);
        }
    } return 0;
    case WM_TIMER :
    {
        InvalidateRect(hWnd, NULL , FALSE);
    } break;
    case WM_DESTROY:
    {
        WindowInput::OnTerminate();
        PostQuitMessage(0);
    } return 0;
    case WM_CHAR:
    {
        unsigned char ucKey = (unsigned char)(char)wParam;
        POINT kPoint;
        GetCursorPos(&kPoint);
        ScreenToClient(hWnd,&kPoint);
        int x = (int)kPoint.x;
        int y = (int)kPoint.y;
        WindowInput::OnKey(ucKey,x,y);
    }break;
    case WM_SIZE:
    {
        int iWidth = int(LOWORD(lParam));
        int iHeight = int(HIWORD(lParam));
        WindowInput::OnResize(iWidth,iHeight);
    }break;
    case WM_LBUTTONUP:
    {
        int x,y;
        get_xy(lParam, &x, &y);
        WindowInput::OnClickUp(x,y,toWKey(wParam));
    } break;
    case WM_LBUTTONDOWN:
    {
        int x,y;
        get_xy(lParam, &x, &y);
        WindowInput::OnClickDown(x,y,toWKey(wParam));
    } break;
    case WM_MOUSEMOVE:
    {
        int x,y;
        get_xy(lParam, &x, &y);
        if (wParam & MK_LBUTTON || wParam & MK_MBUTTON)
            WindowInput::OnMotion(x,y);
        else
            WindowInput::OnPassiveMotion(x,y);
    } break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}


// 初始化窗口并设置标题
int srloop(int w, int h, const char *title)
{
    screewidth=w,screeheight=h;
    WNDCLASSA wc = { CS_BYTEALIGNCLIENT, (WNDPROC)WndProc, 0, 0, 0,
                     NULL, NULL, NULL, NULL, "SCREEN3.1415926" };
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&wc))
        return -1;

    HWND screen_handle = CreateWindowA("SCREEN3.1415926", title,
                                  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                  0, 0, w, h, NULL, NULL, wc.hInstance, NULL);
    if (screen_handle == NULL)
        return -2;

    HDC windowDC;
    windowDC = GetDC(screen_handle);
    memoryDC = CreateCompatibleDC(windowDC);
    ReleaseDC(screen_handle, windowDC);
    BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
                        (DWORD)(w*h*sizeof(uint32_t)) , 0, 0, 0, 0 }  };
    uint32_t* ptr;
    auto bitmap_handle  = CreateDIBSection(memoryDC, &bi, DIB_RGB_COLORS, (void**)&ptr, 0, 0);
    auto oldHandle = SelectObject(memoryDC,bitmap_handle);
    Global::screenBuf()->init(w,h,ptr);

    int sx, sy;
    RECT rect = { 0, 0, w, h };
    AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
    auto wx = rect.right - rect.left;
	auto wy = rect.bottom - rect.top;
    sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
    sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
    if (sy < 0) sy = 0;
    SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
    SetForegroundWindow(screen_handle);

    ShowWindow(screen_handle, SW_NORMAL);
    UpdateWindow(screen_handle);
    HANDLE eventHds[2] = {INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE};
    eventHds[0]=CreateWaitableTimer(NULL,FALSE,NULL);
    eventHds[1]=srSocketEvent();
    constexpr int WTIMER      = 0;
    constexpr int SOCKEVENT   = 1;
    constexpr int TOTALTIMEOUT= 2;
    LARGE_INTEGER interval;
    interval.QuadPart = 200000;//-30000000;
    SetWaitableTimer(eventHds[0],&interval,0, NULL, NULL, 0);
    //interval.QuadPart = -200000;
    
    // first frame 
    WindowInput::OnDisplay();
    InvalidateRect(screen_handle, NULL , FALSE);
    
	MSG wndMsg;
    DWORD dwRet = 0;
    bool bExit = false;
    while(!bExit)
    {
        dwRet=MsgWaitForMultipleObjects(2,eventHds,FALSE,INFINITE,QS_ALLINPUT);
        switch(dwRet - WAIT_OBJECT_0)
        {
        case WTIMER:
        {
            WindowInput::OnDisplay();
            InvalidateRect(screen_handle, NULL , FALSE);
            SetWaitableTimer(eventHds[0],&interval,0, NULL, NULL, 0);
        } break;
        case TOTALTIMEOUT:
        {
            while(PeekMessage(&wndMsg,NULL,0,0,PM_REMOVE)) {
                if(WM_QUIT != wndMsg.message) {
                    TranslateMessage(&wndMsg);
                    DispatchMessage(&wndMsg);
                }
                else {
                    bExit = TRUE;
                    break;
                }
            }
            WindowInput::OnIdle();
        }break;
        case SOCKEVENT:
        {
        } break;
        default:
            break;
        }
    }
    return 0;
}

