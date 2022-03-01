#ifndef __POINT_BASED_COLOR_BLEEDING_H__
#define __POINT_BASED_COLOR_BLEEDING_H__

#include "../SurfaceShader.h"
#include "../AABB.h"
#include "../MipMap.h"
#include "RadiosityPoint.h"

#include <vector>
#include <memory>
#include <fstream>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

class ParameterList;
struct RadPointOctreeNode;
class CubeBuffer;

class RadPointOctree {
public:
    RadPointOctree(const vector<RadiosityPoint> &points, float maxSolidAngle);
    void Rasterize(CubeBuffer &cubeBuffer);    
    Color Illuminate(const Point &p, const Normal &n);
private:
    void Rasterize(CubeBuffer &cubeBuffer, RadPointOctreeNode *node, const AABB &bound);
    Color Illuminate(const Point &p, const Normal &n, RadPointOctreeNode *node, const AABB &bound);
    vector<RadiosityPoint> m_Points;
    RadPointOctreeNode *m_Root;
    AABB m_Bound;
    float m_MaxSolidAngle;
};

class PointBasedColorBleeding : public SurfaceShader {
public:
    PointBasedColorBleeding(shared_ptr<RadPointOctree> octree, bool doRasterize, 
        const string &envmap, const string &texmap, const string &bakefile)
        : m_Octree(octree), m_DoRasterize(doRasterize) {
        if(envmap != "") {
            m_EnvMap = MipMap<Color>::GetMipMap(envmap);
        }
        if(bakefile != "") {            
            m_BakeFS = fstream(bakefile.c_str(), fstream::app);
        }
        if(texmap != "") {
            m_TexMap = MipMap<Color>::GetMipMap(texmap);
        }
    }

    virtual void Shade(RNG &rng, MicroPolygonGrid *grid);

private:
     shared_ptr<RadPointOctree> m_Octree;    
     bool m_DoRasterize;
     shared_ptr<MipMap<Color> > m_EnvMap;
     shared_ptr<MipMap<Color> > m_TexMap;
     fstream m_BakeFS;
};


shared_ptr<SurfaceShader> CreatePointBasedColorBleeding(const ParameterList &params);

} //namespace ToyMan

#endif //#ifndef __POINT_BASED_COLOR_BLEEDING_H__