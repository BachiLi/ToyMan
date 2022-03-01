#ifndef __PARAMETRICS_H__
#define __PARAMETRICS_H__

#include "Primitive.h"
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER

using namespace std;

namespace ToyMan {

class Parametrics : public Primitive
{
public:
    Parametrics(shared_ptr<Attributes> attr, float umin=0.f, float umax=1.f, float vmin=0.f, float vmax = 1.f)
        : Primitive(attr), m_UMin(umin), m_UMax(umax), m_VMin(vmin), m_VMax(vmax) {}

    virtual AABB BoundingBox() const = 0;
    virtual bool Splittable() const;
    virtual void Split(vector<shared_ptr<Primitive> > &splitted, bool uSplit, bool vSplit) const;
    virtual bool Diceable(int *uDim, int *vDim, const Hider &hider, bool *uSplit, bool *vSplit) const;
protected:
    virtual shared_ptr<Parametrics> Clone() const = 0;
    virtual Point F(float u, float v) const = 0;
	virtual Vector dFdu(float u, float v) const;
	virtual Vector dFdv(float u, float v) const;
    virtual Point EvalPoint(float uGrid, float vGrid) const;
    virtual Vector EvaldPdu(float uGrid, float vGrid) const;
    virtual Vector EvaldPdv(float uGrid, float vGrid) const;
    virtual float EvalU(float uGrid, float vGrid) const;
    virtual float EvalV(float uGrid, float vGrid) const;

    float m_UMin, m_UMax, m_VMin, m_VMax;
};

};

#endif // #ifndef __PARAMETRIC_H__
