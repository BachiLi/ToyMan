#include "RadiosityPoint.h"
#include "../SphericalHarmonics.h"
#include "../RNG.h"
#include "../Util.h"
#include "../CommonDef.h"

#include <climits>

using namespace std;

namespace ToyMan {

inline static Vector UniformSampleSphere(float u1, float u2) {
    float z = 1.f - 2.f * u1;
    float r = sqrtf(max(0.f, 1.f - z*z));
    float phi = 2.f * (float)M_PI * u2;
    float x = r * cosf(phi);
    float y = r * sinf(phi);
    return Vector(x, y, z);
}


void RadiosityPoint::SHProject(RNG &rng, 
    array<Color, 9> &shPower,
    array<float, 9> &shArea,
    unsigned int nSamples) {
    array<float, 9> Ylm;
    unsigned int ns = RoundUpPow2(nSamples);
    unsigned int scramble1D = rng.NextUInt(0, UINT_MAX);
    unsigned int scramble2D[2] = {rng.NextUInt(0, UINT_MAX), 
                                  rng.NextUInt(0, UINT_MAX)};    
    float invNs = 1.f/(float)ns;    
    float cosTheta = 0.f;
    array<Color, 9> tmpPower;
    tmpPower.assign(Color());
    for(unsigned int i = 0; i < ns; i++) {
        float u[2];
        Sample02(i, scramble2D, u);
        Vector d = Normalize(UniformSampleSphere(u[0], u[1]));
        SHEvaluate(d, 2, Ylm.data());
        float area = (float)M_PI*radius*radius;
        for(size_t j = 0; j < Ylm.size(); j++) {
            tmpPower[j] += 
                Ci*area*max(Dot(d, N), 0.f)*Ylm[j]*4.f*(float)M_PI*invNs;
            //shArea[j] +=
            //    area*max(Dot(d, N), 0.f)*Ylm[j]*4.f*(float)M_PI*invNs;
            shArea[j] +=
                area*abs(Dot(d, N))*Ylm[j]*4.f*(float)M_PI*invNs;
        }
    }
    SHReduceRinging(tmpPower.data(), 2);
    for(size_t i = 0; i < tmpPower.size(); i++)
        shPower[i] += tmpPower[i];

}

} //namespace ToyMan