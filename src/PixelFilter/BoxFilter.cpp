#include "BoxFilter.h"

namespace ToyMan {

float BoxFilter::Eval(float x, float y) const {
    return 1.f;
}

shared_ptr<PixelFilter> CreateBoxFilter(
    float xWidth, float yWidth) {
    shared_ptr<PixelFilter> filter =
        shared_ptr<PixelFilter>(new BoxFilter(xWidth, yWidth));
    return filter;
}

} //namespace ToyMan