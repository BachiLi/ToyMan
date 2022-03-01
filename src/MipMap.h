#ifndef __MIP_MAP_H__
#define __MIP_MAP_H__

#include "Color.h"
#include "TwoDArray.h"
#include <FreeImagePlus.h>
#include <string>
#include <vector>
#ifdef _MSC_VER
    #include <memory>
#else
    #include <tr1/memory>
#endif //#ifdef _MSC_VER
#include <map>

using namespace std;
using namespace std::tr1;

namespace ToyMan {

template<typename T>
class MipMap {
public:
    MipMap() {m_ImagePyramid.push_back(TwoDArray<T>(1,1));}
    MipMap(const string &filename);
    T Lookup(float s, float t, float width=0.f) const;
    static shared_ptr<MipMap<T> > GetMipMap(const string &filename);
    int GetWidth() const {return m_ImagePyramid.front().GetColNum();}
    int GetHeight() const {return m_ImagePyramid.front().GetRowNum();}

private:
    inline const T& Texel(int level, int s, int t) const;
    inline T Triangle(int level, float s, float t) const;

    vector<TwoDArray<T> > m_ImagePyramid;
    static map<string, shared_ptr<MipMap<T> > > m_MipMaps;
};


static inline void Convert(fipImage &image)
{
    image.convertToRGBF();
}

static inline void Copy(fipImage &image, TwoDArray<Color> *ary)
{
    *ary = TwoDArray<Color>(image.getWidth(), image.getHeight());
    FIRGBF *pixels = (FIRGBF*)image.accessPixels();
    FIRGBF *pixelPtr = pixels;
    for(int i = 0; i < (int)(image.getWidth()*image.getHeight()); i++) {
        Color &color = (*ary)(i);
        color.x = pixelPtr->red;
        color.y = pixelPtr->green;
        color.z = pixelPtr->blue;
        pixelPtr++;
    }
}

static inline void Convert(fipImage &image, TwoDArray<float>)
{
    image.convertToType(FIT_FLOAT);
}

static inline void Copy(fipImage &image, TwoDArray<float> *ary)
{
    *ary = TwoDArray<float>(image.getWidth(), image.getHeight());
    float *pixels = (float*)image.accessPixels();
    float *pixelPtr = pixels;
    for(int i = 0; i < (int)(image.getWidth()*image.getHeight()); i++) {
        float &v = (*ary)(i);
        v = *pixelPtr;
        pixelPtr++;
    }
}

template<typename T>
MipMap<T>::MipMap(const string &filename)
{
    fipImage image;
    if(!image.load(filename.c_str())) {
        m_ImagePyramid.push_back(TwoDArray<T>(1,1));
        return;
    }
    TwoDArray<T> ary;
    Convert(image);
    Copy(image, &ary);
    m_ImagePyramid.push_back(ary);
    int nLevels = 1 + (int)Log2(float(max(image.getWidth(), image.getHeight())));
    for(int i = 1; i < nLevels; i++) {
        image.rescale(image.getWidth()/2, image.getHeight()/2, FILTER_LANCZOS3);
        Copy(image, &ary);
        m_ImagePyramid.push_back(ary);
    }
}

template<typename T>
T MipMap<T>::Lookup(float s, float t, float width) const 
{
    const int nLevels = m_ImagePyramid.size();
    float level = nLevels - 1 + Log2(max(width, 1e-8f));
    if(level < 0.f) {
        return Triangle(0, s, t);
    } else if(level >= nLevels -1) {
        return Texel(nLevels-1, 0, 0);
    } else {
        int iLevel = (int)level;
        float delta = level - (float)iLevel;
        return (1.f-delta) * Triangle(iLevel, s, t) +
                    delta  * Triangle(iLevel+1, s, t);
    }
}

template<typename T>
inline const T &MipMap<T>::Texel(int level, int s, int t) const
{
    const TwoDArray<T> &l = m_ImagePyramid[level];
    s = Mod(s, l.GetColNum());
    t = Mod(t, l.GetRowNum());
    return l(s,t);
}

template<typename T>
inline T MipMap<T>::Triangle(int level, float s, float t) const
{
    const int nLevel = m_ImagePyramid.size();
    level = Clamp<int>(level, 0, nLevel-1);
    s = s * m_ImagePyramid[level].GetColNum() - 0.5f;
    t = t * m_ImagePyramid[level].GetRowNum() - 0.5f;
    int s0 = int(s), t0 = int(t);
    float ds = s-s0, dt = t-t0;
    return (1.f-ds) * (1.f-dt) * Texel(level, s0  , t0  ) +
           (1.f-ds) * dt       * Texel(level, s0  , t0+1) +
           ds       * (1.f-dt) * Texel(level, s0+1, t0  ) +
           ds       * dt       * Texel(level, s0+1, t0+1);
}

template<typename T>
shared_ptr<MipMap<T> > MipMap<T>::GetMipMap(const string &filename)
{
    if(m_MipMaps.find(filename) != m_MipMaps.end())
        return m_MipMaps[filename];
    shared_ptr<MipMap<T> > newMipMap = 
        shared_ptr<MipMap<T> >(new MipMap(filename));
    m_MipMaps[filename] = newMipMap;
    return newMipMap;
}

};

#endif // #ifndef __MIP_MAP_H__
