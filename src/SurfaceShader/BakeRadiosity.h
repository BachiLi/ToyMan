#ifndef __BAKE_RADIOSITY_H__
#define __BAKE_RADIOSITY_H__

#include "../SurfaceShader.h"
#include <memory>
#include <string>
#include <fstream>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

class ParameterList;

class BakeRadiosity : public SurfaceShader {
public:
	BakeRadiosity(const string &filename, float Ka=1.f, float Kd=1.f)
        : m_FStream(filename.c_str(), fstream::app), m_Ka(Ka), m_Kd(Kd) {}
	virtual void Shade(RNG &rng, MicroPolygonGrid *grid);
private:
    //string m_Filename;
    fstream m_FStream;
	float m_Ka, m_Kd;    
};

shared_ptr<SurfaceShader> CreateBakeRadiosity(const ParameterList &params);

} //namespace ToyMan

#endif //#ifndef __BAKE_RADIOSITY_H__