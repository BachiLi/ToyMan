#ifndef __ZBUFFER_HIDER_H__
#define __ZBUFFER_HIDER_H__

#include "Hider.h"
#include "TwoDArray.h"
#include "RNG.h"
#include "Bucket.h"
#include "CropWindow.h"
#include "Color.h"
#include "CommonDef.h"
#include "DOFParameters.h"

#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER
#include <vector>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

class AABB;
class Display;
class ZBufferHider;
class ImageSampler;
class ParameterList;

struct ZBufferSample {
	ZBufferSample(const ImageSample &is) {
		imageSample = is;
		color = Color(0.f,0.f,0.f);
		opacity = Color(1.f,1.f,1.f);
		depth = INFINITY;
	}
	ImageSample imageSample;
	Color color, opacity;
	float depth;
};

class ZBuffer {
public:
	ZBuffer() {}
	ZBuffer(ZBufferHider *hider, int xs, int xe, int ys, int ye, shared_ptr<ImageSampler> sampler, RNG &rng);
	void Sample(int x, int y, const MicroPolygon &mp);
	void Gather(int x, int y, shared_ptr<Display> display);
    bool Cull(const AABB &bound) const;
private:
	TwoDArray<vector<ZBufferSample> > m_Buffer;
	int m_XStart, m_YStart;	
	ZBufferHider *m_Hider;
};

class ZBufferHider : public Hider {
public:
    ZBufferHider(shared_ptr<Transform> cameraToRaster, shared_ptr<ImageSampler> sampler, 
                 shared_ptr<Display> display, bool rasterOrient,
		         int bucketWidth, int bucketHeight, const DOFParameters &dofParams,
                 bool occlusionCulling)
        : Hider(cameraToRaster, sampler, display, rasterOrient), 
            m_BucketWidth(bucketWidth), m_BucketHeight(bucketHeight),
			m_CurBucketCol(0), m_CurBucketRow(0), m_DOFParams(dofParams), 
            m_OcclusionCulling(occlusionCulling) {}
    virtual void WorldBegin();
    virtual void Insert(shared_ptr<Primitive> primitive);
    virtual bool BucketBegin();
    virtual shared_ptr<Primitive> PopPrimitive();
    virtual void Hide(MicroPolygonGrid *grid);
    virtual void BucketEnd();
    virtual void WorldEnd();
    virtual float GetShadingRate(const Primitive &prim) const;
    virtual int MaxGridSize() const;
    virtual bool NeedShade() const;
private:
	void Hide(const MicroPolygon &mp);
	void ToBucketCoord(const AABB &box, int *bColMin, int *bColMax, int *bRowMin, int *bRowMax);
	
	CropWindow m_CropWindow;

	ZBuffer m_ZBuffer;	

	RNG m_RNG;	

	int m_BucketWidth, m_BucketHeight;
	TwoDArray<Bucket> m_Buckets;
	int m_CurBucketCol, m_CurBucketRow;
	int m_CurBucketMinX, m_CurBucketMaxX;
	int m_CurBucketMinY, m_CurBucketMaxY;

	DOFParameters m_DOFParams;
    bool m_OcclusionCulling;

    vector<shared_ptr<Primitive> > m_CulledPrimitives;
};

shared_ptr<Hider> CreateZBufferHider(
    shared_ptr<Transform> cameraToRaster, shared_ptr<Display> display, 
    shared_ptr<ImageSampler> sampler, bool rasterOrient, const DOFParameters &dofParams, 
    const ParameterList &params);

}

#endif //#ifndef __ZBUFFER_HIDER_H__
