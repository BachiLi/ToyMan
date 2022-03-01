#ifndef __MATTE_H__
#define __MATTE_H__

#include "../SurfaceShader.h"
#include "../Color.h"

namespace ToyMan {

class ParameterList;

class Matte : public SurfaceShader {
public:
	Matte(float Ka=1.f, float Kd=1.f)
        : m_Ka(Ka), m_Kd(Kd) {}
	virtual void Shade(RNG &rng, MicroPolygonGrid *grid);
private:
	float m_Ka, m_Kd;
};

shared_ptr<SurfaceShader> CreateMatte(const ParameterList &params);

};

#endif //#ifndef __MATTE_H__