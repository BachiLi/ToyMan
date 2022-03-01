#ifndef __GAUSSIAN_FILTER_H__
#define __GAUSSIAN_FILTER_H__

#include "../PixelFilter.h"
#include <memory>

using namespace std::tr1;

namespace ToyMan {

class GaussianFilter : public PixelFilter {
public:
    GaussianFilter(float xWidth, float yWidth)
        : PixelFilter(xWidth, yWidth) {}

    virtual float Eval(float x, float y) const;

};

shared_ptr<PixelFilter> CreateGaussianFilter(float xWidth, float yWidth);

} //namespace ToyMan

#endif //#ifndef __BOX_FILTER_H__