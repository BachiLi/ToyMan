#ifndef __MEDIAN_CUT_ENV_LIGHT_H__
#define __MEDIAN_CUT_ENV_LIGHT_H__

#include "../LightShader.h"

namespace ToyMan {

class Transform;
class ParameterList;

vector<shared_ptr<LightShader> > CreateMedianCutEnvLight(
    const Transform &light2cam, const ParameterList &params);

};

#endif //#ifndef __MEDIAN_CUT_ENV_LIGHT_H__