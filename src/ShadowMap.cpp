#include "ShadowMap.h"
#include "Util.h"
#include "RNG.h"
#include <FreeImagePlus.h>
#include <fstream>

using namespace std;

namespace ToyMan {

ShadowMap::ShadowMap(const string &filename, const Transform &cameraToWorld)
{
    fstream fs(filename.c_str(), fstream::in);
    int width, height;
    fs >> width >> height;
    Matrix4x4 m;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            fs >> m.m[i][j];
    Transform worldToLight(m);
    m_CameraToLight = worldToLight*cameraToWorld;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            fs >> m.m[i][j];
    m_LightToRaster = Transform(m);
    
    m_DepthMap = TwoDArray<float>(width, height);    
    for(int i = 0; i < width*height; i++) 
        fs >> m_DepthMap(i);
    
    fs.close();
}

void ShadowMap::MakeShadow(const string &from, const string &to,
                           const Transform &worldToLight,
                           const Transform &lightToRaster)
{
    fstream fs(to.c_str(), fstream::out);
    fipImage image;
    if(!image.load(from.c_str()))
        throw ShadowMapDepthFileLoadException();
    if(image.getImageType() != FIT_FLOAT)
        throw ShadowMapDepthFileLoadException();

    fs << image.getWidth() << endl;
    fs << image.getHeight() << endl;
    Matrix4x4 m = worldToLight.GetMatrix();
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            fs << m.m[i][j] << endl;
    m = lightToRaster.GetMatrix();
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            fs << m.m[i][j] << endl;

    float *pixels = (float*)image.accessPixels();
    for(unsigned int i = 0; i < image.getWidth()*image.getHeight(); i++) 
        fs << pixels[i] << endl;
            
    fs.close();
}

//see Bill Reeves et. al, Rendering Antialiased Shadows with Depth Maps
//http://graphics.pixar.com/library/ShadowMaps/paper.pdf
float ShadowMap::Lookup(RNG &rng, const Point &Ps, float blur, int samples, float bias)
{        
    Point lPs = m_CameraToLight(Ps);    
    float z = lPs.z;
    if(z < 0.f)
        return 0.f;
    Point rPs = m_LightToRaster(lPs);

    float sRes = m_DepthMap.GetColNum()*blur;
    float tRes = m_DepthMap.GetRowNum()*blur;
    float s = rPs.x, t = rPs.y;

    if(s < 0.f || 
       s >= (float)m_DepthMap.GetColNum() ||
       t < 0.f ||
       t >= (float)m_DepthMap.GetRowNum())
        return 0.f;

    int ns,nt;
    if(sRes*tRes*4.f < samples) {
        ns = Clamp<int>((int)(sRes*2.f+0.5f), 1, samples);
        nt = Clamp<int>((int)(tRes*2.f+0.5f), 1, samples);
    } else {
        nt = Clamp<int>((int)(sqrt(tRes*samples/sRes)+0.5f), 1, samples);
        ns = Clamp<int>((int)(samples/nt+0.5f), 1, samples);
    }

    float ds = 2.f*sRes/ns, dt = 2.f*tRes/nt;
    float js = ds*.5f, jt = dt*.5f;
    float sMin = s-sRes+js, tMin = t-tRes + jt;
    
    int inShadow = 0;
    s = sMin;
    for(int i = 0; i < ns; i++) {
        t = tMin;
        for(int j = 0; j < nt; j++) {
            int iu = Clamp<int>((int)(s + rng.NextFloat()*js), 0, m_DepthMap.GetColNum()-1);
            int iv = Clamp<int>((int)(t + rng.NextFloat()*jt), 0, m_DepthMap.GetRowNum()-1);
            float depth = m_DepthMap(iu,iv);
            if(depth != 0.f && z > depth+bias) {
                inShadow++;
            }
            t += dt;
        }
        s += ds;
    }
    
    return ((float)(inShadow))/(ns*nt);
}

};