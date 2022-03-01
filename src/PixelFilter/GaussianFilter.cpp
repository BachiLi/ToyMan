#include "GaussianFilter.h"

namespace ToyMan {

float GaussianFilter::Eval(float x, float y) const {
    x *= (1.f/m_XWidth);
    y *= (1.f/m_YWidth);
    return exp(-2.f*(x*x+y*y));
}

shared_ptr<PixelFilter> CreateGaussianFilter(
    float xWidth, float yWidth) {
    shared_ptr<PixelFilter> filter =
        shared_ptr<PixelFilter>(new GaussianFilter(xWidth, yWidth));
    return filter;
}

} //namespace ToyMan
