#include "StratifiedSampler.h"
#include "Util.h"
#include "ParameterList.h"

namespace ToyMan {

inline static void StratifiedSample1D(vector<float> &samp, int nSamples, RNG &rng)
{
	float invTot = 1.f / (float)nSamples;
	samp.reserve(nSamples);
	for(int i = 0; i < nSamples; i++) {
		samp.push_back( ((float)i+rng.NextFloat())*invTot );
	}
}

inline static void StratifiedSample2D(vector<float> &samp, int nx, int ny, RNG &rng)
{
	float dx = 1.f/nx, dy = 1.f/ny;
	samp.reserve(2*nx*ny);
	int idx = 0;
	for(int y = 0; y < ny; y++)
		for(int x = 0; x < nx; x++) {			
			samp.push_back( ((float)x+rng.NextFloat())*dx );
			samp.push_back( ((float)y+rng.NextFloat())*dy );
		}
}

void StratifiedSampler::GetSamples(int x, int y, vector<ImageSample> &samples, RNG &rng)
{
	int nSamples = m_XSamplesPerPixel*m_YSamplesPerPixel;
	vector<float> imageSamples;
	vector<float> lensSamples;
	vector<float> timeSamples;
	StratifiedSample2D(imageSamples, m_XSamplesPerPixel, m_YSamplesPerPixel, rng);
	StratifiedSample2D(lensSamples, m_XSamplesPerPixel, m_YSamplesPerPixel, rng);
	StratifiedSample1D(timeSamples, nSamples, rng);
	Shuffle<float>(lensSamples, nSamples, 2, rng);
	Shuffle<float>(timeSamples, nSamples, 1, rng);

	samples.reserve(nSamples);
	for(int i = 0; i < nSamples; i++) {
		ImageSample sample;
		sample.imageX = x+imageSamples[2*i];
		sample.imageY = y+imageSamples[2*i+1];
        ConcentricSampleDisk(lensSamples[2*i], lensSamples[2*i+1], &(sample.lensU), &(sample.lensV));
		sample.time = Lerp(timeSamples[i], m_ShutterOpen, m_ShutterClose);
		samples.push_back(sample);
	}
}

shared_ptr<ImageSampler> CreateStratifiedSampler(
    int xSample, int ySample, float sOpen, float sClose, const ParameterList &params) {
    shared_ptr<ImageSampler> sampler
        = shared_ptr<ImageSampler>(new StratifiedSampler(xSample, ySample, sOpen, sClose));
    return sampler;
}

};