#ifndef __PIXEL_FILTER_H__
#define __PIXEL_FILTER_H__

namespace ToyMan {

class PixelFilter {
public:
    PixelFilter(float xWidth, float yWidth)
        : m_XWidth(xWidth), m_YWidth(yWidth) {}

    virtual float Eval(float x, float y) const = 0;

    float GetXWidth() const {return m_XWidth;}
    float GetYWidth() const {return m_YWidth;}

protected:
    float m_XWidth, m_YWidth;

};

} //namespace ToyMan

#endif //#ifndef __PIXEL_FILTER_H__