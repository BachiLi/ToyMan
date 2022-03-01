#ifndef __DOF_PARAMETERS_H__
#define __DOF_PARAMETERS_H__

class DOFParameters {
public:
    DOFParameters() {
        cocCoeff = 0.f; invFd = 0.f;
        maxCoC = 0.f;
    }

	DOFParameters(float fs, float fl, float fd) {
		cocCoeff = 0.5f*(fl/fs)*(fd*fl/(fd-fl));
		invFd = 1.f/fd;
		maxCoC = cocCoeff*invFd; //assume depth = INFINITY
	}	

	float CoC(float depth) const {		
		return cocCoeff*abs(1.f/depth-invFd);
	}

	///the maximum possible circle of confusion size
	float MaxCoC() const {
		return maxCoC;
	}

private:
	//float FocalLength, FStop, FocalDistance;

	//some precalculate stuffs
	float cocCoeff;
	float invFd;
	float maxCoC;

};

#endif //#ifndef __DOF_PARAMETERS_H__