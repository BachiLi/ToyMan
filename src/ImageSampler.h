#ifndef __IMAGE_SAMPLER_H__
#define __IMAGE_SAMPLER_H__

#include "CropWindow.h"
#include "RNG.h"

#include <vector>

using namespace std;

namespace ToyMan {

struct ImageSample {
	float imageX, imageY;
	float lensU, lensV;
	float time;
};

class ImageSampler {
public:
	ImageSampler(int xSample, int ySample, float sOpen, float sClose)
		: m_XSamplesPerPixel(xSample), m_YSamplesPerPixel(ySample), 
		  m_ShutterOpen(sOpen), m_ShutterClose(sClose) {}

	virtual void GetSamples(int x, int y, vector<ImageSample> &samples, RNG &rng) = 0;

protected:
	int m_XSamplesPerPixel, m_YSamplesPerPixel;
	float m_ShutterOpen, m_ShutterClose;
};

}

#endif //#ifndef __IMAGE_SAMPLER_H__