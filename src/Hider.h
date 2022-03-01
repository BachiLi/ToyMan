#ifndef __HIDER_H__
#define __HIDER_H__

#include "TwoDArray.h"
#include "ImageSampler.h"
#include <memory>

using namespace std::tr1;

namespace ToyMan {

class Primitive;
class MicroPolygonGrid;
class Point;
class Color;
class Transform;
class AABB;
class ImageSampler;
class Display;

/**
 *   @brief Hider determines which surface is visible, and output pixels.
 *   Occlusion culling also happens here
 */
class Hider {
public:
    Hider(shared_ptr<Transform> cameraToRaster, shared_ptr<ImageSampler> sampler,
        shared_ptr<Display> display, bool rasterOrient) :
        m_CameraToRaster(cameraToRaster), m_Sampler(sampler), m_Display(display),
        m_RasterOrient(rasterOrient) {}
    //hider is responsible to fill in the buffer, it can fill it at anytime
    //between WorldBegin() & WorldEnd()
    virtual void WorldBegin() = 0;
    virtual void Insert(shared_ptr<Primitive> primitive) = 0;
    virtual bool BucketBegin() = 0;
    virtual shared_ptr<Primitive> PopPrimitive() = 0;
    virtual void Hide(MicroPolygonGrid *grid) = 0;
    virtual void BucketEnd() = 0;
    virtual void WorldEnd() = 0;
    virtual float GetShadingRate(const Primitive &prim) const = 0;
    virtual int MaxGridSize() const = 0;
    virtual float RasterEstimate(const Point &p1, const Point &p2) const;
    virtual bool NeedShade() const {return true;}
protected:
    shared_ptr<Transform> m_CameraToRaster;
	shared_ptr<ImageSampler> m_Sampler;
    shared_ptr<Display> m_Display;
    bool m_RasterOrient;
};

};

#endif //#ifndef __HIDER_H__
