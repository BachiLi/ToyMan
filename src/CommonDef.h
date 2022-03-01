#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#include <limits>

using namespace std;

#ifndef INFINITY
#define INFINITY numeric_limits<float>::infinity()
#endif //#ifndef INFINITY

#ifndef isnan
#define isnan(x) _isnan(x)
#endif

#ifndef isinf
#define isinf(x) !_finite(x)
#endif

#ifndef INV_FOURPI
#define INV_FOURPI 0.07957747154594766788f
#endif

#endif //#ifndef __COMMON_DEF_H__
