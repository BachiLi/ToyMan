#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Color.h"
#include "Grid.h"
#include "SurfaceShader.h"
#include <vector>
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER

using namespace std::tr1;

namespace ToyMan {

class Point;
class Vector;
class Normal;
class AABB;
class Framework;
class MicroPolygonGrid;
class Transform;
class Hider;
class Ray;

/** @brief Describes a perspective mapping from (u,v) to (s,t)
 *  the mapping is encoded as a perspective transform of the form:
 *  s = (a0+a1u+a2v)/(1+c1u+c2v)
 *  t = (b0+b1u+b2v)/(1+c1u+c2v)
 *  user needs to input 4 pairs of uv&st relations
 */
struct TextureCoordinates {
    TextureCoordinates(float s1=0.f,float t1=0.f, float s2=1.f, float t2=0.f, float s3=0.f, float t3=1.f, float s4=1.f, float t4=1.f) {
        a0 = s1;
        a1 = -(s1*(s3*(t4-t2)+s4*(t2-t3))+s2*(s3*(t1-t4)+s4*(t3-t1))) /(s3*(t4-t2)+s2*(t3-t4)+s4*(t2-t3));
        a2 = -(s2*(s3*(t4-t1)+s1*(t3-t4))+s1*s4*(t2-t3)+s3*s4*(t1-t2)) /(s3*(t4-t2)+s2*(t3-t4)+s4*(t2-t3));
        b0 = t1;
        b1 = -(s3*(t1*t4-t2*t4)+s1*t2*(t4-t3)+s2*t1*(t3-t4)+s4*(t2*t3-t1*t3)) /(s3*(t4-t2)+s2*(t3-t4)+s4*(t2-t3));
        b2 = (s1*(t3*t4-t2*t3)+s2*(t1*t4-t3*t4)+s3*(t1*t2-t1*t4)+s4*t2*(t3-t1)) /(s3*(t4-t2)+s2*(t3-t4)+s4*(t2-t3));
        c1 = -(s1*(t4-t3)+s2*(t3-t4)+s4*(t2-t1)+s3*(t1-t2)) /(s3*(t4-t2)+s2*(t3-t4)+s4*(t2-t3)); 
        c2 = (s1*(t4-t2)+s3*(t2-t4)+s4*(t3-t1)+s2*(t1-t3)) /(s3*(t4-t2)+s2*(t3-t4)+s4*(t2-t3));        
    }

    float MapS(float u, float v) const {
        return (a0+a1*u+a2*v)/(1.f+c1*u+c2*v);
    }

    float MapT(float u, float v) const {
        return (b0+b1*u+b2*v)/(1.f+c1*u+c2*v);
    }

    float a0,a1,a2,b0,b1,b2,c1,c2;
};

enum ShadingInterpolation {
    CONSTANT,
    SMOOTH
};

enum Sides {
    ONE,
    TWO
};

struct Attributes {    
    Attributes();

    shared_ptr<Transform> objectToCamera;
    shared_ptr<Transform> movementTransform;
    Color color;
    Color opacity;
    TextureCoordinates texCoords;
    shared_ptr<LightList> lightSources;    
    ShadingInterpolation shadingInterpolation;
    float shadingRate;
    shared_ptr<SurfaceShader> surfaceShader;
    Sides sides;
};

class Primitive {
public:
    //Primitive(const Attributes &attr)
    //    : m_Attributes(shared_ptr<Attributes>(new Attributes(attr))) {}
    Primitive(shared_ptr<Attributes> attr)
        : m_Attributes(attr) {}
    //get a bounding box in camera space
    virtual AABB BoundingBox() const = 0;
    virtual bool Splittable() const = 0;
    //split the primitive, and add it the the vector caller provided
    virtual void Split(vector<shared_ptr<Primitive> > &splitted, bool uSplit, bool vSplit) const = 0;
    //if the primitive is dicable, the micropolygon's dimension is returned
    //otherwise return the axis that has to split
    virtual bool Diceable(int *uDim, int *vDim, const Hider &hider, bool *uSplit, bool *vSplit) const = 0;
    //special dice procedure, only dice the points
    //the pointGrid should be allocated before calling this function
    virtual void Dice(shared_ptr<PointGrid> pointGrid) const;
	//general dice procedure, add the default variables into grid
	virtual void Dice(MicroPolygonGrid *grid) const;
    virtual bool Intersect(const Ray &ray, MicroPolygonGrid *grid) const {return false;}
    virtual bool IntersectP(const Ray &ray) const {return false;}

    inline const shared_ptr<const Transform> GetObjectToCamera() const {return m_Attributes->objectToCamera;}    
	inline shared_ptr<Transform> GetMovementTransform() const {return m_Attributes->movementTransform;}
	inline shared_ptr<SurfaceShader> GetSurfaceShader() const {return m_Attributes->surfaceShader;}
    inline ShadingInterpolation GetShadingInterpolation() const {return m_Attributes->shadingInterpolation;}
    inline float GetShadingRate() const {return m_Attributes->shadingRate;}
    inline shared_ptr<LightList> GetLightSources() const {return m_Attributes->lightSources;}

protected:
    //note that the point returned should be in Camera space
    virtual Point EvalPoint(float uGrid, float vGrid) const = 0;
    virtual Vector EvaldPdu(float uGrid, float vGrid) const;
    virtual Vector EvaldPdv(float uGrid, float vGrid) const;
	virtual Normal EvalNormal(float uGrid, float vGrid) const;
	virtual Color EvalCs(float uGrid, float vGrid) const {return m_Attributes->color;}
    virtual float EvalU(float uGrid, float vGrid) const {return uGrid;}
    virtual float EvalV(float uGrid, float vGrid) const {return vGrid;}
    virtual float EvalS(float uGrid, float vGrid) const {
        float u = EvalU(uGrid, vGrid), v = EvalV(uGrid, vGrid);
        return m_Attributes->texCoords.MapS(u,v);
    }
    virtual float EvalT(float uGrid, float vGrid) const {
        float u = EvalU(uGrid, vGrid), v = EvalV(uGrid, vGrid);
        return m_Attributes->texCoords.MapT(u,v);
    }

    shared_ptr<Attributes> m_Attributes;
};

};

#endif //#ifndef __PRIMITIVE_H__
