#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#include "TwoDArray.h"
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER

using namespace std::tr1;

namespace ToyMan {

class Hider;
class Primitive;
class Color;
class Display;

/**
 *  @brief Framework performs the main render job.
 *  Parser is responsible to insert the primitives into it,
 *  Framework delegates to Hider to either do rasterization
 *  or ray tracing
 */
class Framework {
public:
    Framework(shared_ptr<Hider> hider) : m_Hider(hider) {}
    virtual void WorldBegin() = 0;
    virtual void Insert(shared_ptr<Primitive> primitive) = 0;
    virtual void WorldEnd() = 0;

protected:
    shared_ptr<Hider> m_Hider;
};

};

#endif //#ifndef __FRAMEWORK_H__
