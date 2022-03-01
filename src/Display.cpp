#include "Display.h"
#include "Util.h"
#include <cmath>
#include <algorithm>

using namespace std;

namespace ToyMan {

Display::Display(int xRes, int yRes, const float crop[4], const Channel &channel,
            shared_ptr<PixelFilter> filter)
	: m_XResolution(xRes), m_YResolution(yRes), m_Channel(channel), m_Filter(filter) {
	int xs = (int)ceil(xRes*crop[0]);
	int xe = max(xs+1, (int)ceil(xRes*crop[1]));
	int ys = (int)ceil(yRes*crop[2]);
	int ye = max(ys+1, (int)ceil(yRes*crop[3]));
	m_CropWindow = CropWindow(xs, xe, ys, ye);

    m_Pixels = TwoDArray<Pixel>(
        m_CropWindow.GetWidth(), m_CropWindow.GetHeight());
}

void Display::AddSample(float x, float y, const Color &color, float alpha, float depth) {
    int ix = (int)x - m_CropWindow.xStart;
    int iy = (int)y - m_CropWindow.yStart;
    int xs = Clamp<int>((int)floor(ix-m_Filter->GetXWidth()), 0, m_CropWindow.GetWidth()-1);
    int xe = Clamp<int>((int) ceil(ix+m_Filter->GetXWidth()), 0, m_CropWindow.GetWidth()-1);
    int ys = Clamp<int>((int)floor(iy-m_Filter->GetYWidth()), 0, m_CropWindow.GetHeight()-1);
    int ye = Clamp<int>((int) ceil(iy+m_Filter->GetYWidth()), 0, m_CropWindow.GetHeight()-1);
    
    m_Pixels(ix,iy).depth = 
        min(m_Pixels(ix,iy).depth,depth);

    for(int yy = ys; yy <= ye; yy++)
        for(int xx = xs; xx <= xe; xx++) {
            float dx = (float)xx-x;
            float dy = (float)yy-y;
            if(abs(dx) > m_Filter->GetXWidth() ||
               abs(dy) > m_Filter->GetYWidth())
                continue;
            float weight = m_Filter->Eval(dx,dy);
            m_Pixels(xx,yy).color += weight*color;
            m_Pixels(xx,yy).alpha += weight*alpha;            
            m_Pixels(xx,yy).weight += weight;
        }
}

};