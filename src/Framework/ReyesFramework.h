#ifndef __REYES_FRAMEWORK_H__
#define __REYES_FRAMEWORK_H__

#include "../Framework.h"
#include "../TwoDArray.h"
#include "../RNG.h"
#include "../ViewingFrustum.h"
#include <memory>

using namespace std;

namespace ToyMan {

class Hider;
class Display;

class ReyesFramework : public Framework {
public:
    ReyesFramework(shared_ptr<Hider> hider, const ViewingFrustum &vf, bool backFaceCulling)
		: Framework(hider), m_ViewingFrustum(vf), m_BackFaceCulling(backFaceCulling) {}
    virtual void WorldBegin();
    virtual void Insert(shared_ptr<Primitive> primitive);
    virtual void WorldEnd();
private:
	void internalInsert(shared_ptr<Primitive> primitive, int depth);
	ViewingFrustum m_ViewingFrustum;
    RNG m_RNG;
    bool m_BackFaceCulling;
};

shared_ptr<Framework> CreateReyesFramework(
    shared_ptr<Hider> hider, const ViewingFrustum &vf, bool backFaceCulling);

};

#endif //#ifndef __REYES_FRAMEWORK_H__
