#include "CommonDef.h"
#include "ZBufferHider.h"
#include "MicroPolygon.h"
#include "MicroPolygonGrid.h"
#include "Util.h"
#include "TwoDArray.h"
#include "Display.h"
#include "Primitive.h"
#include "Transform.h"
#include "ParameterList.h"

#include <algorithm>
#include <vector>
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER
#include <iostream>
#include <cstdio>

using namespace std;

namespace ToyMan {

ZBuffer::ZBuffer(ZBufferHider *hider, int xs, int xe, int ys, int ye, shared_ptr<ImageSampler> sampler, RNG &rng)
{
	m_XStart = xs; m_YStart = ys;
	m_Buffer = TwoDArray<vector<ZBufferSample> >(xe-xs+1, ye-ys+1);
	for(int y = ys; y <= ye; y++)
		for(int x = xs; x <= xe; x++) {
			vector<ImageSample> samples;
			sampler->GetSamples(x, y, samples, rng);
			for(size_t i = 0; i < samples.size(); i++) {
				ZBufferSample zSamp(samples[i]);
				m_Buffer(x-xs,y-ys).push_back(zSamp);
			}
		}
}

void ZBuffer::Sample(int x, int y, const MicroPolygon &mp)
{
	vector<ZBufferSample> &samples = m_Buffer(x-m_XStart, y-m_YStart);
	for(size_t i = 0, size = samples.size(); i < size; i++) {		
		ZBufferSample &sample = samples[i];
		if(mp.bBox.pMin.z >= sample.depth)
			continue;
		Color color, opacity;
		float depth;		
		if(mp.Sample(sample.imageSample, &color, &opacity, &depth)) {            
			if(depth < sample.depth) {
				sample.color = color;
				sample.opacity = opacity;
				sample.depth = depth;
			}
		}
	}
}

void ZBuffer::Gather(int x, int y, shared_ptr<Display> display) {
	vector<ZBufferSample> &samples = m_Buffer(x-m_XStart, y-m_YStart);
	for(size_t i = 0; i < samples.size(); i++) {
		ZBufferSample &sample = samples[i];
		
        display->AddSample(
            sample.imageSample.imageX,
            sample.imageSample.imageY,
            sample.color,
            sample.depth != INFINITY ? 1.f : 0.f,
            sample.depth);
		
	}
}

bool ZBuffer::Cull(const AABB &bound) const {
    int xMin = Clamp<int>((int)floor(bound.pMin.x - (float)m_XStart), 0, m_Buffer.GetColNum()-1);
    int xMax = Clamp<int>((int) ceil(bound.pMax.x - (float)m_XStart), 0, m_Buffer.GetColNum()-1);
    int yMin = Clamp<int>((int)floor(bound.pMin.y - (float)m_YStart), 0, m_Buffer.GetRowNum()-1);
    int yMax = Clamp<int>((int) ceil(bound.pMax.y - (float)m_YStart), 0, m_Buffer.GetRowNum()-1);
    float z = bound.pMin.z;
    for(int y = yMin; y <= yMax; y++)
        for(int x = xMin; x <= xMax; x++) {
            const vector<ZBufferSample> &samples = m_Buffer(x, y);
            for(size_t i = 0; i < samples.size(); i++) {
                if(z <= samples[i].depth) {
                    return false;         
                }
            }
        }

    return true;
}

void ZBufferHider::ToBucketCoord(const AABB &box, int *bColMin, int *bColMax, int *bRowMin, int *bRowMax) {
	*bColMin = Clamp<int>((int)floor((box.pMin.x-float(m_CropWindow.xStart))/m_BucketWidth) , 0, m_Buckets.GetColNum()-1);
	*bColMax = Clamp<int>((int) ceil((box.pMax.x-float(m_CropWindow.xStart))/m_BucketWidth) , 0, m_Buckets.GetColNum()-1);
	*bRowMin = Clamp<int>((int)floor((box.pMin.y-float(m_CropWindow.yStart))/m_BucketHeight), 0, m_Buckets.GetRowNum()-1);
	*bRowMax = Clamp<int>((int) ceil((box.pMax.y-float(m_CropWindow.yStart))/m_BucketHeight), 0, m_Buckets.GetRowNum()-1);
}

void ZBufferHider::WorldBegin()
{
	m_CropWindow = m_Display->GetSampleExtent();    

	//create buckets
	int width = m_CropWindow.GetWidth();
	int height = m_CropWindow.GetHeight();
    int bucketColumns = (int)ceil((float)width / m_BucketWidth);
    int bucketRows = (int)ceil((float)height / m_BucketHeight);
    //printf("br:%d\n", bucketRows);
    m_Buckets = TwoDArray<Bucket>(bucketColumns, bucketRows);
	m_CurBucketCol = m_CurBucketRow = 0;
}

void ZBufferHider::Insert(shared_ptr<Primitive> primitive)
{
    AABB box = primitive->BoundingBox();
	box = (*m_CameraToRaster)(box);
	box.Expand(m_DOFParams.MaxCoC());
	int bColMin, bColMax, bRowMin, bRowMax;
	ToBucketCoord(box, &bColMin, &bColMax, &bRowMin, &bRowMax);
		
	int bCol = bColMin;
	int bRow = bRowMin;
	int bIdx = bRow*m_Buckets.GetColNum()+bCol;
	int curIdx = m_CurBucketRow*m_Buckets.GetColNum()+m_CurBucketCol;
	if(bIdx < curIdx) {
		int maxIdx = bRowMax*m_Buckets.GetColNum()+bColMax;
		if(maxIdx < curIdx) 
			return;		
		
		bCol = m_CurBucketCol;
		bRow = m_CurBucketRow;
	}

	m_Buckets(bCol, bRow).Insert(primitive);
}

bool ZBufferHider::BucketBegin()
{   
	if(m_CurBucketCol < m_Buckets.GetColNum() &&
	   m_CurBucketRow < m_Buckets.GetRowNum()) {

		m_CurBucketMinX = m_CropWindow.xStart+m_CurBucketCol*m_BucketWidth;
		m_CurBucketMaxX = min(m_CurBucketMinX+m_BucketWidth-1, m_CropWindow.xEnd-1);
		m_CurBucketMinY = m_CropWindow.yStart+m_CurBucketRow*m_BucketHeight;
		m_CurBucketMaxY = min(m_CurBucketMinY+m_BucketHeight-1, m_CropWindow.yEnd-1);        
		//create z buffer
		m_ZBuffer = ZBuffer(this, m_CurBucketMinX, m_CurBucketMaxX, m_CurBucketMinY, m_CurBucketMaxY, m_Sampler, m_RNG);	
		return true;
	}
    return false;
}

shared_ptr<Primitive> ZBufferHider::PopPrimitive()
{        
    if(m_OcclusionCulling) {        
        //m_Buckets(m_CurBucketCol, m_CurBucketRow).SortPrimitivesByZ();	    
        shared_ptr<Primitive> prim;
        AABB bound;
        //occlusion culling
        while(true) {
            prim = m_Buckets(m_CurBucketCol, m_CurBucketRow).PopPrimitive();
            if(prim.get() == NULL) break;

            bound = prim->BoundingBox();
            float z = bound.pMin.z;
            bound = (*m_CameraToRaster)(bound);
            bound.pMin.z = z;
            if(!m_ZBuffer.Cull(bound)) break;
            m_CulledPrimitives.push_back(prim);
        }
        return prim;
    } else {
        return m_Buckets(m_CurBucketCol, m_CurBucketRow).PopPrimitive();
    }    
}

void ZBufferHider::Hide(MicroPolygonGrid* grid)
{    
    vector<shared_ptr<MicroPolygon> > microPolygons;
    grid->BustColor(microPolygons, *m_CameraToRaster, m_DOFParams);
    for(size_t i = 0; i < microPolygons.size(); i++) {
        MicroPolygon &mp = *(microPolygons[i]);
        if(mp.bBox.pMin.x >= (float)m_Display->GetXResolution() ||
           mp.bBox.pMin.y >= (float)m_Display->GetYResolution() ||
           mp.bBox.pMax.x < 0.f ||
           mp.bBox.pMax.y < 0.f) //the micropolygon is out of screen
            continue;

		int bColMin, bColMax, bRowMin, bRowMax;
		ToBucketCoord(mp.bBox, &bColMin, &bColMax, &bRowMin, &bRowMax);
		for(int r = bRowMin; r <= bRowMax; r++)
			for(int c = bColMin; c <= bColMax; c++) {
				m_Buckets(c, r).Insert(microPolygons[i]);
			}
    }

    shared_ptr<MicroPolygon> mp;
	while((mp = m_Buckets(m_CurBucketCol, m_CurBucketRow).PopMicroPolygon()).get() != 0) {
        int xMin = Clamp<int>((int)floor(mp->bBox.pMin.x), m_CurBucketMinX, m_CurBucketMaxX);
        int xMax = Clamp<int>((int) ceil(mp->bBox.pMax.x), m_CurBucketMinX, m_CurBucketMaxX);
        int yMin = Clamp<int>((int)floor(mp->bBox.pMin.y), m_CurBucketMinY, m_CurBucketMaxY);
        int yMax = Clamp<int>((int) ceil(mp->bBox.pMax.y), m_CurBucketMinY, m_CurBucketMaxY);		
        for(int y = yMin; y <= yMax; y++)
            for(int x = xMin; x <= xMax; x++) {
				m_ZBuffer.Sample(x, y, *mp);
            }
	}
}

void ZBufferHider::BucketEnd()
{	
	shared_ptr<MicroPolygon> mp;
	while((mp = m_Buckets(m_CurBucketCol, m_CurBucketRow).PopMicroPolygon()).get() != 0) {
        int xMin = Clamp<int>((int)floor(mp->bBox.pMin.x), m_CurBucketMinX, m_CurBucketMaxX);
        int xMax = Clamp<int>((int) ceil(mp->bBox.pMax.x), m_CurBucketMinX, m_CurBucketMaxX);
        int yMin = Clamp<int>((int)floor(mp->bBox.pMin.y), m_CurBucketMinY, m_CurBucketMaxY);
        int yMax = Clamp<int>((int) ceil(mp->bBox.pMax.y), m_CurBucketMinY, m_CurBucketMaxY);		
        for(int y = yMin; y <= yMax; y++)
            for(int x = xMin; x <= xMax; x++) {
				m_ZBuffer.Sample(x, y, *mp);
            }
	}

	for(int y = m_CurBucketMinY; y <= m_CurBucketMaxY; y++)
		for(int x = m_CurBucketMinX; x <= m_CurBucketMaxX; x++) {
			m_ZBuffer.Gather(x, y, m_Display);
		}

    m_Display->UpdateScreen(m_CurBucketMinX, m_CurBucketMaxX, m_CurBucketMinY, m_CurBucketMaxY);

	m_CurBucketCol++;
	if(m_CurBucketCol >= m_Buckets.GetColNum()) {
		m_CurBucketCol = 0;
		m_CurBucketRow++;
	}

    while(!m_CulledPrimitives.empty()) {                
        Insert(m_CulledPrimitives.back());
        m_CulledPrimitives.pop_back();
    }
    
}

void ZBufferHider::WorldEnd() {
    m_Display->Done();
}

float ZBufferHider::GetShadingRate(const Primitive &prim) const {
	AABB box = prim.BoundingBox();
	float coc = min(m_DOFParams.CoC(box.pMin.z), m_DOFParams.CoC(box.pMax.z));
    return max(prim.GetShadingRate(), coc);
}

int ZBufferHider::MaxGridSize() const {
    return 256;
}

bool ZBufferHider::NeedShade() const {
    return m_Display->GetChannel() == Display::RGB ||
           m_Display->GetChannel() == Display::RGBA;
}

shared_ptr<Hider> CreateZBufferHider(
    shared_ptr<Transform> cameraToRaster, shared_ptr<Display> display, 
    shared_ptr<ImageSampler> sampler, bool rasterOrient, 
    const DOFParameters &dofParams, 
    const ParameterList &params)
{
    int bucketWidth = (int)params.FindFloat("bucketwidth", 32);
    int bucketHeight = (int)params.FindFloat("bucketheight", 32);
    bool occlusionCulling = params.FindFloat("occlusionculling", 1) != 0.f;
    shared_ptr<Hider> hider = 
        shared_ptr<Hider>(
            new ZBufferHider(cameraToRaster, sampler, display, rasterOrient,
                bucketWidth, bucketHeight, dofParams, occlusionCulling));
    return hider;
}


};
