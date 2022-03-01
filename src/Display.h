#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "CropWindow.h"
#include "Color.h"
#include "TwoDArray.h"
#include "PixelFilter.h"
#include "CommonDef.h"
#include <memory>

using namespace std::tr1;

namespace ToyMan {


/**
 *  @brief A display represents a image, it can be a image file or framebuffer
 */
class Display {
public:	
	enum Channel {
		RGB,
		RGBA,
		Z
	};

    Display(int xRes, int yRes, const float crop[4], const Channel &channel,
            shared_ptr<PixelFilter> filter);

    int GetXResolution() const {return m_XResolution;}
    int GetYResolution() const {return m_YResolution;}    

	virtual CropWindow GetSampleExtent() const {
        return m_CropWindow;
        /*CropWindow win = m_CropWindow;
        win.xStart -= (int)m_Filter->GetXWidth();
        win.xEnd += (int)m_Filter->GetXWidth();
        win.yStart -= (int)m_Filter->GetYWidth();
        win.yEnd += m_Filter->GetXWidth();
        return win;*/
    }
	virtual CropWindow GetPixelExtent() const {return m_CropWindow;}

	Channel GetChannel() const {return m_Channel;}

    void AddSample(float x, float y, const Color &color, float alpha, float depth);

    //usually for framebuffer, update currentstatus and show to user
    virtual void UpdateScreen(int xs, int xe, int ys, int ye) =0;
    //usually for file, it means save the current image to a file
    virtual void Done() = 0;

protected:
    struct Pixel {
        Pixel() {
            color = Color(0.f,0.f,0.f);
            depth = INFINITY;
            alpha = 0.f;
            weight = 0.f;
        }

	    Color color;
	    float depth;
        float alpha;
        float weight;
    };

	CropWindow m_CropWindow;
    int m_XResolution, m_YResolution;
	Channel m_Channel;
    TwoDArray<Pixel> m_Pixels;
    shared_ptr<PixelFilter> m_Filter;
};

};

#endif //#ifndef __DISPLAY_H__
