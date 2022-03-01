#pragma once
#include "../Display.h"
#include "../TwoDArray.h"
#include <memory>
#include <string>
#include <FreeImagePlus.h>
#include <windows.h>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

class Color;
class ParameterList;

class Mutex
{
public:
   Mutex( void ) { InitializeCriticalSection(&cs); }
   ~Mutex( ) { DeleteCriticalSection(&cs); }
   
   void Lock( void ) { EnterCriticalSection(&cs); }
   void Unlock( void ) { LeaveCriticalSection(&cs); }
private:
   CRITICAL_SECTION cs;
};

class FrameBufferDisplay : public Display
{
public:
	FrameBufferDisplay(int xRes, int yRes, const float crop[4], 
        const Channel &channel, const string &name, shared_ptr<PixelFilter> filter);
    ~FrameBufferDisplay();
		
    
    virtual void UpdateScreen(int xs, int xe, int ys, int ye);
	virtual void Done();

    DWORD WinMain();
    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	string m_Name;
	fipWinImage m_Image;
    //fipWinImage m_DisplayImage;

    HANDLE m_HWinThread;
    HWND m_HWnd;

    Mutex m_Mutex;
    bool m_Done;
};

shared_ptr<Display> CreateFrameBufferDisplay(const string &name, int xRes, int yRes, 
    const float crop[4], const Display::Channel &channel, shared_ptr<PixelFilter> filter, 
    const ParameterList &params);

};