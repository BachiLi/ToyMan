#include "PointBasedColorBleeding.h"
#include "RadiosityPoint.h"
#include "../ParameterList.h"
#include "../MicroPolygonGrid.h"
#include "../TwoDArray.h"
#include "../AABB.h"
#include "../Plane.h"
#include "../RNG.h"
#include "../SphericalHarmonics.h"
#include "../Normal.h"
#include "../Point.h"
#include "../MipMap.h"
#include "../Util.h"

#include <string>
#include <fstream>
#include <array>
#include <map>

using namespace std;
using namespace std::tr1;

#define PBRT_L1_CACHE_LINE_SIZE 64

void *AllocAligned(size_t size);
template <typename T> T *AllocAligned(unsigned int count) {
    return (T *)AllocAligned(count * sizeof(T));
}

void FreeAligned(void *);
class MemoryArena {
public:
    // MemoryArena Public Methods
    MemoryArena(unsigned int bs = 32768) {
        blockSize = bs;
        curBlockPos = 0;
        currentBlock = AllocAligned<char>(blockSize);
    }
    ~MemoryArena() {
        FreeAligned(currentBlock);
        for (unsigned int i = 0; i < usedBlocks.size(); ++i)
            FreeAligned(usedBlocks[i]);
        for (unsigned int i = 0; i < availableBlocks.size(); ++i)
            FreeAligned(availableBlocks[i]);
    }
    void *Alloc(unsigned int sz) {
        // Round up _sz_ to minimum machine alignment
        sz = ((sz + 15) & (~15));
        if (curBlockPos + sz > blockSize) {
            // Get new block of memory for _MemoryArena_
            usedBlocks.push_back(currentBlock);
            if (availableBlocks.size() && sz <= blockSize) {
                currentBlock = availableBlocks.back();
                availableBlocks.pop_back();
            }
            else
                currentBlock = AllocAligned<char>(max(sz, blockSize));
            curBlockPos = 0;
        }
        void *ret = currentBlock + curBlockPos;
        curBlockPos += sz;
        return ret;
    }
    template<typename T> T *Alloc(unsigned int count = 1) {
        T *ret = (T *)Alloc(count * sizeof(T));
        for (unsigned int i = 0; i < count; ++i)
            new (&ret[i]) T();
        return ret;
    }
    void FreeAll() {
        curBlockPos = 0;
        while (usedBlocks.size()) {
    #ifndef NDEBUG
            memset(usedBlocks.back(), 0xfa, blockSize);
    #endif
            availableBlocks.push_back(usedBlocks.back());
            usedBlocks.pop_back();
        }
    }
private:
    // MemoryArena Private Data
    unsigned int curBlockPos, blockSize;
    char *currentBlock;
    vector<char *> usedBlocks, availableBlocks;
};

void *AllocAligned(size_t size) {
    return _aligned_malloc(size, PBRT_L1_CACHE_LINE_SIZE);
}


void FreeAligned(void *ptr) {
    if (!ptr) return;
    _aligned_free(ptr);
}


namespace ToyMan {

MemoryArena g_OctreeArena;

inline AABB OctreeChildBound(int child, const AABB &nodeBound,
                             const Point &pMid) {
    AABB childBound;
    childBound.pMin.x = (child & 4) ? pMid.x : nodeBound.pMin.x;
    childBound.pMax.x = (child & 4) ? nodeBound.pMax.x : pMid.x;
    childBound.pMin.y = (child & 2) ? pMid.y : nodeBound.pMin.y;
    childBound.pMax.y = (child & 2) ? nodeBound.pMax.y : pMid.y;
    childBound.pMin.z = (child & 1) ? pMid.z : nodeBound.pMin.z;
    childBound.pMax.z = (child & 1) ? nodeBound.pMax.z : pMid.z;
    return childBound;
}

struct RadPointOctreeNode {
    RadPointOctreeNode() {        
        isLeaf = true;
        for(size_t i = 0; i < rps.size(); i++)
            rps[i] = NULL;
    }

    ~RadPointOctreeNode() {
        if(!isLeaf) {            
            for(size_t i = 0; i < children.size(); i++)
                if(children[i])
                    delete children[i];
        }
    }

    void Insert(const AABB &nodeBound, 
        RadiosityPoint *rp) {
        Point pMid = .5f * (nodeBound.pMin + nodeBound.pMax);
        if(isLeaf) {
            for(size_t i = 0; i < rps.size(); i++) 
                if(!rps[i]) {
                    rps[i] = rp;
                    return;
                }
            
            isLeaf = false;
            array<RadiosityPoint*, 8> localRps = rps;
            children.assign(NULL);
            for(size_t i = 0; i < rps.size(); i++) {
                RadiosityPoint *rp = localRps[i];
                int child = (rp->P.x > pMid.x ? 4 : 0) +
                            (rp->P.y > pMid.y ? 2 : 0) +
                            (rp->P.z > pMid.z ? 1 : 0);
                if(!children[child])
                    children[child] = g_OctreeArena.Alloc<RadPointOctreeNode>();
                AABB childBound = OctreeChildBound(child, nodeBound, pMid);
                children[child]->Insert(childBound, rp);
            }
        }
        int child = (rp->P.x > pMid.x ? 4 : 0) +
                    (rp->P.y > pMid.y ? 2 : 0) +
                    (rp->P.z > pMid.z ? 1 : 0);
        if(!children[child])
            children[child] = g_OctreeArena.Alloc<RadPointOctreeNode>();
        AABB childBound = OctreeChildBound(child, nodeBound, pMid);
        children[child]->Insert(childBound, rp);       
    }

    int InitHierarchy() {
        /*cluster.P = Point();
        cluster.Ci = Color();
        cluster.N = Normal();
        cluster.area = 0.f;
        if(isLeaf) {                                    
            int rpsCount = 0;
            for(size_t i = 0; i < rps.size(); i++) {
                if(!rps[i]) break;
                cluster.P += rps[i]->P;
                cluster.Ci += rps[i]->Ci;
                cluster.N += rps[i]->N;
                cluster.area += rps[i]->area;
                rpsCount++;
            }
            
            if(rpsCount) {
                cluster.P /= (float)rpsCount;                
                cluster.N = Normalize(cluster.N);  
                cluster.Ci /= (float)rpsCount;
            }
        } else {
            int childrenCount = 0;
            for(size_t i = 0; i < children.size(); i++) {
                if(!children[i]) continue;
                children[i]->InitHierarchy();
                cluster.P += children[i]->cluster.P;
                cluster.Ci += children[i]->cluster.Ci;
                cluster.N += children[i]->cluster.N;
                cluster.area += children[i]->cluster.area;
                childrenCount++;
            }
            
            if(childrenCount) {
                cluster.P /= (float)childrenCount;                
                cluster.N = Normalize(cluster.N);   
                cluster.Ci /= (float)childrenCount;
            }
        }
        */
        
        shPower.assign(Color());
        shArea.assign(0.f);
        clusterPos = Point();
        //bound = AABB();
        if(isLeaf) {                                    
            int rpsCount = 0;
            for(size_t i = 0; i < rps.size(); i++) {
                if(!rps[i]) break;
                rps[i]->SHProject(*RNG::GetGlobalRNG(),
                    shPower, shArea); 
                clusterPos += rps[i]->P;
                bound = Union(bound, rps[i]->P);                
                rpsCount++;
            }            
            clusterPos /= (float)rpsCount;
            return rpsCount;
        } else {
            int childNum = 0;
            for(size_t i = 0; i < children.size(); i++) {
                if(!children[i]) continue;
                int iChildNum = children[i]->InitHierarchy();
                for(size_t j = 0; j < shPower.size(); j++) {
                    shPower[j] += children[i]->shPower[j];
                    shArea[j] += children[i]->shArea[j];
                }
                clusterPos += (float)iChildNum*children[i]->clusterPos;
                childNum += iChildNum;
                bound = Union(bound,children[i]->bound);
            }
            clusterPos /= (float)childNum;
            return childNum;
        }
        
    }

    bool isLeaf;    
    //RadiosityPoint cluster;
    array<Color, 9> shPower;
    array<float, 9> shArea;
    Point clusterPos;    
    union {
        array<RadPointOctreeNode*, 8> children;
        array<RadiosityPoint*, 8> rps;
    };
    AABB bound;

};

const int CUBE_WIDTH = 32;
const int CUBE_HEIGHT = 32;

inline Vector GetPixelDir(int face, int px, int py) {
    float sx = ((px+.5f)/CUBE_WIDTH)*2.f-1.f;
    float sy = ((py+.5f)/CUBE_HEIGHT)*2.f-1.f;
    switch(face) {
        case 0:
            return Vector(1.f, sy, -sx);
        case 1:
            return Vector(-1.f, sy, sx);
        case 2:
            return Vector(sx, 1.f, -sy);
        case 3:
            return Vector(sx, -1.f, sy);
        case 4:
            return Vector(sx, sy, 1.f);
        case 5:
            return Vector(-sx, sy, -1.f);
        default:            
            return Vector();
    }
    return Vector();
}

class CubeBuffer {    
public:
    CubeBuffer(const Point &origin, 
        const Normal &originNormal,
        shared_ptr<MipMap<Color> > envmap) {
        m_Origin = origin;
        m_OriginNormal = Normalize(originNormal);
        m_Plane = Plane(m_Origin, m_OriginNormal);

        if(!s_Initialized) {
            InitPixels(envmap);
            s_Initialized = true;
        }
    }

    void Rasterize(const RadiosityPoint &pt);
    void Rasterize(const Color &Ci, 
        const Vector &d, float projArea, float dist);
    Color Integral() const;
    bool UnderHorizon(const AABB &bound) const;
    Point GetOrigin() const {return m_Origin;}
    void PrintBuffer(ostream &out) const {
        out << m_Origin << endl;
        out << m_OriginNormal << endl;
        for(int i = 0; i < 6; i++) {
            for(int x = 0; x < CUBE_WIDTH; x++)
                for(int y = 0; y < CUBE_HEIGHT; y++)
                    out << m_Faces[i].pixels[x][y].color << endl;
            out << endl;
        }
    }
private:    
    struct PointBufferPixel {
        PointBufferPixel() { distance = INFINITY; }
        Color color;
        float distance;
    };

    struct CubeFace {
        inline void AddPoint(int x, int y, 
            const Color &c, float dis);

        PointBufferPixel pixels[CUBE_WIDTH][CUBE_HEIGHT];        
    };

    static void InitPixels(shared_ptr<MipMap<Color> > envMap);

    //assume project to z
    inline void Project(float x, float y, float z, 
        float halfProjWidth, CubeFace &face,
        const Color &c, float dist);

    Point m_Origin;
    Normal m_OriginNormal;
    Plane m_Plane; //for octree test
    //the order of six faces are: +x, -x, +y, -y, +z, -z
    CubeFace m_Faces[6];    

    static bool s_Initialized;
    static float s_Pixeldw[6][CUBE_WIDTH][CUBE_HEIGHT];
    static Vector s_PixelDir[6][CUBE_WIDTH][CUBE_HEIGHT];
    static Color s_EnvColor[6][CUBE_WIDTH][CUBE_HEIGHT];
};

bool CubeBuffer::s_Initialized = false;
float CubeBuffer::s_Pixeldw[6][CUBE_WIDTH][CUBE_HEIGHT];
Vector CubeBuffer::s_PixelDir[6][CUBE_WIDTH][CUBE_HEIGHT];
Color CubeBuffer::s_EnvColor[6][CUBE_WIDTH][CUBE_HEIGHT];

void CubeBuffer::InitPixels(shared_ptr<MipMap<Color> > envMap) {
    const Normal na[] = {Normal(1.f,0.f,0.f), Normal(-1.f, 0.f, 0.f),
                         Normal(0.f,1.f,0.f), Normal( 0.f,-1.f, 0.f),
                         Normal(0.f,0.f,1.f), Normal( 0.f, 0.f,-1.f)};   

    for(int f = 0; f < 6; f++)
        for(int px = 0; px < CUBE_WIDTH; px++)
            for(int py = 0; py < CUBE_HEIGHT; py++) {
                Vector pixelDir = GetPixelDir(f, px, py);                
                float pixelDist2 = pixelDir.LengthSquared();
                pixelDir = Normalize(pixelDir);
                const float pixelArea = (2.f/CUBE_WIDTH)*(2.f/CUBE_HEIGHT);
                float dw = pixelArea*Dot(na[f], pixelDir)/pixelDist2;                                
                s_Pixeldw[f][px][py] = dw;
                s_PixelDir[f][px][py] = pixelDir;
                float s = SphericalPhi(pixelDir) * (1.f/(2.f*(float)M_PI));
                float t = SphericalTheta(pixelDir) * (1.f/(float)M_PI);
                s_EnvColor[f][px][py] = envMap.get() != NULL ? envMap->Lookup(s,t) : Color();
            }
}

inline void CubeBuffer::CubeFace::AddPoint(int x, int y, 
    const Color &c, float dis) {
    PointBufferPixel &pixel = pixels[x][y];
    if(pixel.distance < dis) return;    
    pixel.color = c;
    pixel.distance = dis;    
}

inline void CubeBuffer::Project(
    float x, float y, float z, float halfProjWidth, 
    CubeFace &face, const Color &c, float dist) {    
    if(z <= 1e-6f)
        return;

    float invZ = 1.f/z;
    float sx0 = 0.5f*((x-halfProjWidth)*invZ+1.f);
    float sx1 = 0.5f*((x+halfProjWidth)*invZ+1.f);
    float sy0 = 0.5f*((y-halfProjWidth)*invZ+1.f);
    float sy1 = 0.5f*((y+halfProjWidth)*invZ+1.f);
    if(sx0 >= 1.f || sx1 < 0.f || sy0 >= 1.f || sy1 < 0.f)
        return;
    int minX = max(int(sx0*CUBE_WIDTH), 0);
    int maxX = min(int(sx1*CUBE_WIDTH), CUBE_WIDTH-1);
    int minY = max(int(sy0*CUBE_HEIGHT), 0);        
    int maxY = min(int(sy1*CUBE_HEIGHT), CUBE_HEIGHT-1);
    for(int x = minX; x <= maxX; x++)
        for(int y = minY; y <= maxY; y++)  {
            face.AddPoint(x,y,c,dist);
        }
}

void CubeBuffer::Rasterize(const RadiosityPoint &pt) {
    //determine which face the point belong
    Vector d = pt.P-m_Origin;
    //float t = Dot(d, m_OriginNormal);
    //if(t <= 0.f) //below horizon
    //    return;
    
    float invDistSquared = 1.f/d.LengthSquared();
    //if(pt.area*invDistSquared >= (float)M_PI) {
    //    return;
    //}
    float invDist = sqrt(invDistSquared);
    d = d*invDist;
    float cosTheta = abs(Dot(pt.N, -d));
    if(cosTheta <= 0.f) return;
    float area = (float)M_PI*pt.radius*pt.radius;
    float projArea = cosTheta*area*invDistSquared;
    float dist = 1.f/invDist;
    Rasterize(pt.Ci, d, projArea, dist);
}

void CubeBuffer::Rasterize(const Color &Ci, const Vector &d, float projArea, float dist) {
    if(projArea <= 0.f) {        
        return;
    }

    float t = Dot(d, m_OriginNormal);
    if(t <= 0.f)
        return;

    float halfProjWidth = sqrtf(projArea)/2.f;
    if(d.x > 0.f)
        Project(-d.z, d.y, d.x,halfProjWidth,m_Faces[0],Ci, dist);
    else
        Project( d.z, d.y,-d.x,halfProjWidth,m_Faces[1],Ci, dist);
    if(d.y > 0.f)
        Project( d.x,-d.z, d.y,halfProjWidth,m_Faces[2],Ci, dist);
    else
        Project( d.x, d.z,-d.y,halfProjWidth,m_Faces[3],Ci, dist);
    if(d.z > 0.f)
        Project( d.x, d.y, d.z,halfProjWidth,m_Faces[4],Ci, dist);
    else
        Project(-d.x, d.y,-d.z,halfProjWidth,m_Faces[5],Ci, dist);
    /*
    if(abs(d.x) > abs(d.y) && abs(d.x) > abs(d.z)) { //project to +x
        if(d.x > 0.f)
            Project(-d.z, d.y, d.x,halfProjWidth,m_Faces[0],Ci,dist);
        else
            Project( d.z, d.y,-d.x,halfProjWidth,m_Faces[1],Ci,dist);
    } else if(abs(d.y) > abs(d.x) && abs(d.y) > abs(d.z)) { //project to y
        if(d.y > 0.f)
            Project( d.x,-d.z, d.y,halfProjWidth,m_Faces[2],Ci, dist);
        else
            Project( d.x, d.z,-d.y,halfProjWidth,m_Faces[3],Ci, dist);
    } else { //project to z
        if(d.z > 0.f)
            Project( d.x, d.y, d.z,halfProjWidth,m_Faces[4],Ci, dist);
        else
            Project(-d.x, d.y,-d.z,halfProjWidth,m_Faces[5],Ci, dist);
    }
    */
}

Color CubeBuffer::Integral() const {   
    //int count = 0;
    Color result;
    //for each face
    for(int f = 0; f < 6; f++) {
        const CubeFace &face = m_Faces[f];        
        //for each pixel
        for(int px = 0; px < CUBE_WIDTH; px++)
            for(int py = 0; py < CUBE_HEIGHT; py++) {                
                const PointBufferPixel &pixel = face.pixels[px][py];                
                const Color &c = pixel.distance != INFINITY ? 
                    pixel.color : s_EnvColor[f][px][py];                
                //integral over area
                float dw = s_Pixeldw[f][px][py];
                float cosTheta = max(Dot(m_OriginNormal, s_PixelDir[f][px][py]),0.f);                
                float wt = (1.f/((float)M_PI))*cosTheta*dw;
                //result += wt*Clamp(c,0.f,1.f);
                result += wt*c;
                //count++;
            }
    }

    //float ao = max(1.f-(float)count/(3*CUBE_WIDTH*CUBE_HEIGHT),0.f);

    return result;
}

bool CubeBuffer::UnderHorizon(const AABB &bound) const {
    return m_Plane.GetSide(bound) == Plane::OUTSIDE;
}

RadPointOctree::RadPointOctree(
    const vector<RadiosityPoint> &points,
    float maxSolidAngle) {
    m_Points = points;
    m_Root = new RadPointOctreeNode();
    for(size_t i = 0; i < points.size(); i++) 
        m_Bound = Union(m_Bound, m_Points[i].P);
    m_Bound.Expand(0.01f);
    for(size_t i = 0; i < points.size(); i++) 
        m_Root->Insert(m_Bound, &m_Points[i]);
    m_Root->InitHierarchy();
    m_MaxSolidAngle = maxSolidAngle;
}

void RadPointOctree::Rasterize(CubeBuffer &cubeBuffer) {    
    Rasterize(cubeBuffer, m_Root, m_Bound);
}

void RadPointOctree::Rasterize(CubeBuffer &cubeBuffer, 
    RadPointOctreeNode *node, const AABB &bound) {
    if(cubeBuffer.UnderHorizon(bound)) 
        return;

    if(node->isLeaf) {        
        for(size_t i = 0; i < node->rps.size(); i++)
            if(node->rps[i]) {                                
                cubeBuffer.Rasterize(*(node->rps[i]));        
            }
    } else {                       
        Vector d = cubeBuffer.GetOrigin() - node->clusterPos;
        float distance2 = d.LengthSquared();
        float distance = sqrt(distance2);
        d = d/distance;
        array<float, 9> Ylm;
        SHEvaluate(d, 2, Ylm.data());
        float area = 0.f;
        for(size_t i = 0; i < Ylm.size(); i++)
            area += node->shArea[i]*Ylm[i];
        float solidAngle = area / distance2;                
        if(!bound.Inside(cubeBuffer.GetOrigin()) &&
           solidAngle >= 0.f && solidAngle < m_MaxSolidAngle) {
            Color power(0.f,0.f,0.f);
            for(size_t i = 0; i < Ylm.size(); i++) 
                power += node->shPower[i]*Ylm[i];
            cubeBuffer.Rasterize(power/area, -d, solidAngle, distance);
        } else {            
            Point pMid = .5f * (bound.pMin + bound.pMax);
            for(size_t i = 0; i < node->children.size(); i++)
                if(node->children[i]) {                                        
                    //AABB childBound = OctreeChildBound(i, bound, pMid);
                    Rasterize(cubeBuffer, node->children[i], node->children[i]->bound);
                }
        }        
    }    

}

Color RadPointOctree::Illuminate(const Point &p, const Normal &n) {
    return Illuminate(p, n, m_Root, m_Bound);
}

static inline float FormFactor(const Point &rP, const Normal &rN,
                               const Point &oP, const Normal &oN,
                               float dP) {
    Vector d = oP-rP;
    float t = Dot(d,rN);
    if(t < -dP) { 
        return 0.f;
    } else if(t < dP) {
        Point p0 = oP + dP*rN;
        Point p1 = oP -  t*rN;
        Point cP = 0.5f*(p0+p1);
        d = p0-cP;
        dP = d.Length();
        d = cP-rP;
        float cosTheta = Dot(d,oN);
        if(cosTheta >= 0.f)
            return 0.f;
        float invDist2 = 1.f/(d.LengthSquared()+1e-6f);
        return -cosTheta*Dot(d,rN)*dP*dP * invDist2 * invDist2;

        /*
        if(Dot(d,oN) >= 0.f)
            return 0.f;
        float invDist2 = 1.f/(d.LengthSquared()+1e-6f);
        float invDist = sqrt(invDist2);
        Vector nD = d*invDist;
        return -Dot(nD,oN)*Dot(nD,rN)*dP*dP * invDist2;
        */
    } else {        
        /*
        if(Dot(d, oN) >= 0.f)
            return 0.f;
        float invDist2 = 1.f/(d.LengthSquared()+1e-6f);
        float invDist = sqrt(invDist2);
        Vector nD = d*invDist;
        assert(Dot(nD, rN) >= 0.f);      
        return -Dot(nD,oN)*Dot(nD,rN)*dP*dP * invDist2;
        */

        float cosTheta = Dot(d, oN);
        if(cosTheta >= 0.f)
            return 0.f;
        float invDist2 = 1.f/(d.LengthSquared()+1e-6f);   
        return -cosTheta*t*dP*dP * invDist2 * invDist2;
    }
}

Color RadPointOctree::Illuminate(const Point &p, const Normal &n, RadPointOctreeNode *node, const AABB &bound) {
    Color result;
    if(node->isLeaf) {        
        for(size_t i = 0; i < node->rps.size(); i++) {
            if(!node->rps[i]) break;
            Color c = node->rps[i]->Ci*
                FormFactor(p,n,node->rps[i]->P,node->rps[i]->N,
                    node->rps[i]->radius);
            result += c;            
        }
    } else {       
        Plane plane(p, n);
        if(plane.GetSide(bound) == Plane::OUTSIDE)
            return result;        
        
        Vector d = p - node->clusterPos;
        float distance2 = d.LengthSquared()+1e-6f;
        float invDistance2 = 1.f/distance2;
        float invDist = sqrt(invDistance2);
        d *= invDist;

        array<float, 9> Ylm;
        SHEvaluate(d, 2, Ylm.data());                
        float area = 0.f;
        for(size_t i = 0; i < Ylm.size(); i++)
            area += node->shArea[i]*Ylm[i];
        float solidAngle = area * invDistance2;
        if(!bound.Inside(p) && 
            solidAngle >= 0.f &&
            solidAngle < m_MaxSolidAngle) {               
            Color power;
            for(size_t i = 0; i < Ylm.size(); i++) 
                  power += node->shPower[i]*Ylm[i]; 
            power = Clamp(power, 0.f, 1.f);            
            Color c = power*max(Dot(n,-d), 0.f)*invDistance2;
            result += c; 
        } else {            
            Point pMid = .5f * (bound.pMin + bound.pMax);
            for(size_t i = 0; i < node->children.size(); i++)
                if(node->children[i]) {                                        
                    //AABB childBound = OctreeChildBound(i, bound, pMid);
                    result += Illuminate(p,n,node->children[i],node->children[i]->bound);
                }
        }
    }
    return result;
}

void PointBasedColorBleeding::Shade(RNG &rng, MicroPolygonGrid *grid) {
    //setup grid symbols
    shared_ptr<PointGrid>  P = grid->FindPointGrid("P");
	shared_ptr<NormalGrid> N = grid->FindNormalGrid("N");
    shared_ptr<ColorGrid> Ci = grid->FindColorGrid("Ci");	
	shared_ptr<ColorGrid> Cs = grid->FindColorGrid("Cs");
    shared_ptr<VectorGrid> I = grid->FindVectorGrid("I");

    const int uDim = grid->GetUDimenstion();
	const int vDim = grid->GetVDimenstion();
    for(int i = 0; i < uDim*vDim; i++) {
        Point  p = (*P)(i);        
        Normal n = Faceforward(Normalize((*N)(i)), -(*I)(i));
        if(m_DoRasterize) {
            CubeBuffer buffer(p, n, m_EnvMap);
            m_Octree->Rasterize(buffer);
            (*Ci)(i) = (*Cs)(i)*((buffer.Integral()));
        } else {
            Color c = m_Octree->Illuminate(p,n)/((float)M_PI);        
            (*Ci)(i) = (*Cs)(i)*(c);        
        }
    }

    if(m_TexMap.get() != NULL) {
        shared_ptr<FloatGrid>  s = grid->FindFloatGrid("s");
        shared_ptr<FloatGrid>  t = grid->FindFloatGrid("t");    
        FloatGrid dsdu(uDim,vDim), dsdv(uDim,vDim), dtdu(uDim,vDim), dtdv(uDim,vDim);

        for(int u = 0; u < uDim; u++)
            for(int v = 0; v < vDim; v++) {
                int up = Clamp(u+1,0,uDim-1), vp = Clamp(v+1,0,vDim-1);
                int um = Clamp(u-1,0,uDim-1), vm = Clamp(v-1,0,vDim-1);
                dsdu(u,v) = ((*s)(up,v) - (*s)(um,v))/(float)(up-um);
                dsdv(u,v) = ((*s)(u,vp) - (*s)(u,vm))/(float)(vp-vm);
                dtdu(u,v) = ((*t)(up,v) - (*t)(um,v))/(float)(up-um);
                dtdv(u,v) = ((*t)(u,vp) - (*t)(u,vm))/(float)(vp-vm);
            } 

        for(int i = 0; i < uDim*vDim; i++) {            
            Color tex=m_TexMap->Lookup((*s)(i), (*t)(i), 2.f*max(max(abs(dsdu(i)), abs(dsdv(i))),
                max(abs(dtdu(i)), abs(dtdv(i)))));
            //std::cerr << tex << std::endl;
            (*Ci)(i) = (*Ci)(i)*tex;
        }
    }

    if(m_BakeFS.is_open()) {
        vector<RadiosityPoint> radPoints;
        shared_ptr<VectorGrid> dPdu = grid->FindVectorGrid("dPdu");    
        shared_ptr<VectorGrid> dPdv = grid->FindVectorGrid("dPdv");    
        for(int u = 0; u < uDim-1; u++)
            for(int v = 0; v < vDim-1; v++) {
                Point p = 0.25f*((*P)(u,v)+(*P)(u+1,v)+(*P)(u,v+1)+(*P)(u+1,v+1));
                Normal n = Normalize((*N)(u,v)+(*N)(u+1,v)+(*N)(u,v+1)+(*N)(u+1,v+1));
                Color ci = 0.25f*((*Ci)(u,v)+(*Ci)(u+1,v)+(*Ci)(u,v+1)+(*Ci)(u+1,v+1));
                Vector dpdu = 0.25f*((*dPdu)(u,v)+(*dPdu)(u+1,v)+(*dPdu)(u,v+1)+(*dPdu)(u+1,v+1));
                Vector dpdv = 0.25f*((*dPdv)(u,v)+(*dPdv)(u+1,v)+(*dPdv)(u,v+1)+(*dPdv)(u+1,v+1));
                float area = Cross(dpdu,dpdv).Length();
                radPoints.push_back(
                    RadiosityPoint(p, n, ci, area));
            }

    
        for(size_t i = 0; i < radPoints.size(); i++)
            m_BakeFS << radPoints[i] << endl;
    }
}

shared_ptr<SurfaceShader> CreatePointBasedColorBleeding(const ParameterList &params) {
    string filename = params.FindString("bakefile", "bake.txt");
    float maxSolidAngle = params.FindFloat("maxsolidangle", 0.05f);
    bool doRasterize = params.FindFloat("dorasterize", 0.f) != 0.f;
    string texmap = params.FindString("texmap", "");
    string envmap = params.FindString("envmap", "");
    string bakeOut = params.FindString("bakeout", "");

    static map<string, shared_ptr<RadPointOctree> > bakefileMap;
    shared_ptr<RadPointOctree> octree;
    if(bakefileMap[filename].get() == 0) {
        //fprintf(stderr, "new octree\n");
        fstream fs(filename.c_str(), fstream::in);
        vector<RadiosityPoint> points;    
        while(fs.good()) {
            RadiosityPoint pt;
            fs >> pt;
            points.push_back(pt);
        }
        fprintf(stderr, "num of points:%d\n", points.size());
            
        octree = shared_ptr<RadPointOctree>(new RadPointOctree(points, maxSolidAngle));
        bakefileMap[filename] = octree;
    } else {
        //fprintf(stderr, "old octree\n");
        octree = bakefileMap[filename];
    }
    
    shared_ptr<SurfaceShader> shader = 
        shared_ptr<SurfaceShader>(
        new PointBasedColorBleeding(octree, doRasterize, envmap, texmap, bakeOut));
    return shader;
}

} //namespace ToyMan