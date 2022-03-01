#ifndef __LIGHT_SHADER_H__
#define __LIGHT_SHADER_H__

#include "Point.h"
#include "Vector.h"
#include "Color.h"
#include "RNG.h"

#include <vector>
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER

using namespace std;

namespace ToyMan {

class MicroPolygonGrid;

class LightShader {
public:
	virtual bool IsAmbient() const = 0;	
    virtual bool Shade(RNG &rng, const Point &Ps, const Normal &Ns, float As, //illuminance params
                       Vector *L, Color *Cl) = 0; //outputs
protected:
    //assume axis is normalized
    bool Illuminate(const Point &Ps, const Normal &Ns, float As, //illuminance params
        const Point &position, const Vector &axis, float angle,  //illuminate params
        Vector *L); //outputs
	bool Solar(const Point &Ps, const Normal &Ns, float As, //illuminance params
        const Vector &axis, float angle,                    //solar params
        Vector *L); //outputs


};

typedef shared_ptr<LightShader> LightShaderSharedPtr;
typedef vector<LightShaderSharedPtr> LightList;

};

#endif //#ifndef __LIGHT_SHADER_H__