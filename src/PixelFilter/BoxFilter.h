#ifndef __BOX_FILTER_H__
#define __BOX_FILTER_H__

#include "../PixelFilter.h"
#include <memory>

using namespace std::tr1;

namespace ToyMan {

class BoxFilter : public PixelFilter {
public:
    BoxFilter(float xWidth, float yWidth)
        : PixelFilter(xWidth, yWidth) {}

    virtual float Eval(float x, float y) const;

};

shared_ptr<PixelFilter> CreateBoxFilter(float xWidth, float yWidth);

} //namespace ToyMan

#endif //#ifndef __BOX_FILTER_H__