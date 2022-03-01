#include "MipMap.h"

namespace ToyMan {

template<typename T>
map<string, shared_ptr<MipMap<T> > > MipMap<T>::m_MipMaps;

template<>
map<string, shared_ptr<MipMap<Color> > > MipMap<Color>::m_MipMaps;
template<>
map<string, shared_ptr<MipMap<float> > > MipMap<float>::m_MipMaps;


};