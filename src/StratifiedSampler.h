#pragma once
#include "ImageSampler.h"

namespace ToyMan {

class ParameterList;

class StratifiedSampler : public ImageSampler
{
public:
	StratifiedSampler(int xSample, int ySample, float sOpen, float sClose)
		: ImageSampler(xSample, ySample, sOpen, sClose) {}

	void GetSamples(int x, int y, vector<ImageSample> &samples, RNG &rng);

};

shared_ptr<ImageSampler> CreateStratifiedSampler(
    int xSample, int ySample, float sOpen, float sClose, const ParameterList &params);

};