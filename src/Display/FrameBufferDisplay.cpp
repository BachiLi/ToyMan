#include "FrameBufferDisplay.h"
#include "../Color.h"
#include "../Util.h"
#include "../ParameterList.h"

namespace ToyMan {

static DWORD WINAPI WinThreadFunc( LPVOID lpParam ) 
{
    FrameBufferDisplay *display = (FrameBufferDisplay*)lpParam;
    return display->WinMain();
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    FrameBufferDisplay *display = reinterpret_cast<FrameBufferDisplay*>(GetWindowLong(hWnd, GWL_USERDATA));
    return display->WndProc(hWnd, message, wParam, lParam);
}

FrameBufferDisplay::FrameBufferDisplay(int xRes, int yRes, const float crop[4], 
									   const Channel &channel, const string &name,
                                       shared_ptr<PixelFilter> filter)
	: Display(xRes, yRes, crop, channel, filter), m_Name(name) {
	switch(channel) {
		case RGB:
		case RGBA:
			m_Image = fipWinImage(FIT_BITMAP, xRes, yRes, 32);
			break;
		case Z:
			m_Image = fipWinImage(FIT_FLOAT, xRes, yRes, 32);
	}	

    m_HWinThread = CreateThread(
        NULL,
        0,
        WinThreadFunc,
        this,
        0,
        NULL);

    m_Done = false;
}

FrameBufferDisplay::~FrameBufferDisplay() {
    if(m_Done) //render success, so we wait for user to close the window
        WaitForSingleObject(m_HWinThread, INFINITE);
    CloseHandle(m_HWinThread);
}

DWORD FrameBufferDisplay::WinMain()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wndClass;
    wndClass.style         = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc   = ToyMan::WndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hInstance     = hInstance;
    wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName  = NULL;
    wndClass.lpszClassName = m_Name.c_str();

    RegisterClass(&wndClass);

    m_HWnd = CreateWindow( m_Name.c_str(),          /* window's class name */
                           m_Name.c_str(),          /* caption */
                           WS_OVERLAPPEDWINDOW,     /* style */
                           CW_USEDEFAULT,           /* x position */
                           CW_USEDEFAULT,           /* y position */
                           CW_USEDEFAULT,           /* width */
                           CW_USEDEFAULT,           /* height */
                           NULL,                    /* parent window handle */
                           NULL,                    /* window menu handle */
                           hInstance,               /* program handle */
                           NULL                     /* creation parameters */
                           );

    RECT rcClient, rcWindow;
    POINT ptDiff;
    GetClientRect(m_HWnd, &rcClient);
    GetWindowRect(m_HWnd, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(m_HWnd,rcWindow.left, rcWindow.top, m_Image.getWidth() + ptDiff.x, m_Image.getHeight() + ptDiff.y, TRUE);

    SetWindowLong(m_HWnd, GWL_USERDATA, reinterpret_cast<LONG>(this));
    ShowWindow(m_HWnd, SW_SHOWNORMAL);
    UpdateWindow(m_HWnd);

    MSG msg;
    BOOL ret;
    while((ret = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if(ret == -1) {
            return -1;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

LRESULT FrameBufferDisplay::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{    
    switch(message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(m_HWnd, &ps);       
            HDC hDC = GetDC(m_HWnd);
            RECT rcClient;
            GetClientRect(m_HWnd, &rcClient);
            m_Image.draw(hDC, rcClient);          
            EndPaint(m_HWnd, &ps);
            return 0;
        } case WM_KEYUP: {
            if(wParam == TEXT('D')) {
                RECT rcClient, rcWindow;
                POINT ptDiff;
                GetClientRect(m_HWnd, &rcClient);
                GetWindowRect(m_HWnd, &rcWindow);
                ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
                ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
                MoveWindow(m_HWnd,rcWindow.left, rcWindow.top, m_Image.getWidth() + ptDiff.x, m_Image.getHeight() + ptDiff.y, TRUE);
            }
            return 0;
        } case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        } default: {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    /* this line should not be executed, but just in case */
    assert(0);
    return -1;
}

void FrameBufferDisplay::UpdateScreen(int xs, int xe, int ys, int ye)
{
	switch(m_Channel) {
		case RGB: {
            RGBQUAD *pixels = (RGBQUAD*)m_Image.accessPixels();
            for(int y = ys; y <= ye; y++) {
                 for(int x = xs; x <= xe; x++) {
                    int xx = x-m_CropWindow.xStart;
                    int yy = y-m_CropWindow.yStart;
                    Color c = m_Pixels(xx,yy).color/m_Pixels(xx,yy).weight;
                    RGBQUAD quad;
                    quad.rgbRed      = Clamp<int>(int(pow(c.x, 1.f/1.f)*255), 0, 255);
                    quad.rgbGreen    = Clamp<int>(int(pow(c.y, 1.f/1.f)*255), 0, 255);
                    quad.rgbBlue     = Clamp<int>(int(pow(c.z, 1.f/1.f)*255), 0, 255);
                    quad.rgbReserved = 255;
                    pixels[yy*m_Image.getWidth()+xx] = quad;
                }
            }
			break;
        } case RGBA: {
            RGBQUAD *pixels = (RGBQUAD*)m_Image.accessPixels();
            for(int y = ys; y <= ye; y++) {
                 for(int x = xs; x <= xe; x++) {
                    int xx = x-m_CropWindow.xStart;
                    int yy = y-m_CropWindow.yStart;
                    float invWeight = 1.f/m_Pixels(xx,yy).weight;
                    Color c = m_Pixels(xx,yy).color*invWeight;
                    RGBQUAD quad;
                    quad.rgbRed      = Clamp<int>(int(pow(c.x, 1.f/1.f)*255), 0, 255);
                    quad.rgbGreen    = Clamp<int>(int(pow(c.y, 1.f/1.f)*255), 0, 255);
                    quad.rgbBlue     = Clamp<int>(int(pow(c.z, 1.f/1.f)*255), 0, 255);
                    quad.rgbReserved = (BYTE)(255.f*m_Pixels(xx,yy).alpha*invWeight);
                    pixels[yy*m_Image.getWidth()+xx] = quad;
                }
            }
			break;
        } case Z: {
            float *pixels = (float*)m_Image.accessPixels();
            for(int y = ys; y <= ye; y++)
                for(int x = xs; x <= xe; x++) {
                    int xx = x-m_CropWindow.xStart;
                    int yy = y-m_CropWindow.yStart;
                    pixels[yy*m_Image.getWidth()+xx] = 
                        m_Pixels(xx,yy).depth != INFINITY ? m_Pixels(xx,yy).depth : 0.f;
                }
            
            break;
        }
	}

    m_Image.setModified();
    PostMessage(m_HWnd, WM_PAINT, 0, 0);
}

void FrameBufferDisplay::Done()
{
    PostMessage(m_HWnd, WM_PAINT, 0, 0);    
    m_Done = true;
    m_Image.save("out.exr");
}

shared_ptr<Display> CreateFrameBufferDisplay(const string &name, int xRes, int yRes, 
    const float crop[4], const Display::Channel &channel, shared_ptr<PixelFilter> filter, 
    const ParameterList &params) {

    shared_ptr<Display> display = 
        shared_ptr<Display>(new FrameBufferDisplay(xRes, yRes, crop, channel, name, filter));
    return display;
}

};