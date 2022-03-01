#include "MedianCutEnvLight.h"
#include "DistantLight.h"
#include "../ParameterList.h"
#include "../Transform.h"
#include "../Util.h"
#include "../MipMap.h"
#include <cmath>
#include <algorithm>

namespace ToyMan {

class MedianCutEnvLight {
public:
    MedianCutEnvLight(const Transform &light2cam, const string &texturename="", 
        bool minimizeVariance=true, int nSamples=64, float intensity=1.f);

    vector<shared_ptr<LightShader> > GetLights() {
        return m_Lights;
    }
private:
    void RecursivePartition(
        float *img, int width, int height, int xs, int ys, int xe, int ye, int depth, 
        bool minimizeVariance, const Transform &light2cam);

    vector<shared_ptr<LightShader> > m_Lights;
    MipMap<Color> m_RadianceMap;
    int m_NSamples;
    float m_Intensity;
};

MedianCutEnvLight::MedianCutEnvLight(const Transform &light2cam, 
    const string &texturename, bool minimizeVariance, int nSamples, 
    float intensity) {    
    m_NSamples = nSamples;
    m_Intensity = intensity;
    m_RadianceMap = MipMap<Color>(texturename);
    int w = m_RadianceMap.GetWidth();
    int h = m_RadianceMap.GetHeight();
    float filter = 1.f/max(w,h);
    float *img = new float[w*h];
    for(int v = 0; v < h; v++) {
        float vp = (float)v/(float)h;
        float sinTheta = std::sinf((float)M_PI*(float)(v+.5f)/(float)h);
        for(int u = 0; u < w; u++) {
            float up = (float)u/(float)w;
            img[u+v*w] = m_RadianceMap.Lookup(up, vp, filter).Y()*sinTheta;
        }
    }

    //printf("sampling radiance map\n");
    RecursivePartition(img, w, h, 0, 0, w, h, (int)Log2((float)nSamples), minimizeVariance, light2cam);
    //printf("end samping radiance map\n");
    delete[] img;
}

static float CalRegionVariance(float *img, int width, int height, int xs, int ys, int xe, int ye) {
	if(xs == xe || ys == ye)
		return 0.f;	

	float massX = 0.f, massY = 0.f;
	float totalEnergy = 0.f;	
	for(int y = ys; y < ye; y++) {
		for(int x = xs; x < xe; x++) {
			float u = float(x)/float(width);
			float v = float(y)/float(height);
			massX += u*img[x+y*width];
			massY += v*img[x+y*width];
			totalEnergy += img[x+y*width];
		}
	}

	float centroidX = massX/totalEnergy;
	float centroidY = massY/totalEnergy;

	float var = 0.f;
	for(int y = ys; y < ye; y++) {
		for(int x = xs; x < xe; x++) {
			float u = float(x)/float(width);
			float v = float(y)/float(height);
			var += img[x+y*width]*(u-centroidX)*(u-centroidX)*(v-centroidY)*(v-centroidY);
		}
	}
	
	return var;
}


void MedianCutEnvLight::RecursivePartition(
    float *img, int width, int height, int xs, int ys, int xe, int ye, int depth,
    bool minimizeVariance, const Transform &light2cam) {
    //printf("%d %d %d %d %d\n", xs, ys, xe, ye, depth);

	if(xs == xe || ys == ye)
		return;

	float centerX = (xe+xs)*0.5f;
	float centerY = (ye+ys)*0.5f;	

	if(depth == 0) {
		float filter = 1.f / max(width, height);
		float massX = 0.f, massY = 0.f;
		float totalEnergy = 0.f;		
        Color radiance;
		const float dTheta = (float)M_PI/height;
		const float dPhi = 2.f*(float)M_PI/width;		
		for(int y = ys; y < ye; y++) {
			float sinTheta = sinf((float)M_PI * float(y+.5f)/float(height));
			const float dA = sinTheta*dTheta*dPhi;
			for(int x = xs; x < xe; x++) {
				float u = float(x)/float(width);
				float v = float(y)/float(height);
				
				radiance += m_RadianceMap.Lookup(u, v, filter)*dA;
				massX += u*img[x+y*width];
				massY += v*img[x+y*width];
				totalEnergy += img[x+y*width];
			}
		}

		float centroidX = massX/totalEnergy;
		float centroidY = massY/totalEnergy;


		float theta = centroidY * (float)M_PI, phi = centroidX * 2.f * (float)M_PI;
		float costheta = cosf(theta), sintheta = sinf(theta);
		float sinphi = sinf(phi), cosphi = cosf(phi);
		Vector dir(sintheta * cosphi, sintheta * sinphi,
								  costheta);
        dir = light2cam(dir);
		m_Lights.push_back(shared_ptr<LightShader>(new 
            DistantLight(m_Intensity, radiance, 
            Point(), Point(dir.x,dir.y,dir.z))));
		return;
	}

	if(minimizeVariance) {
		float minMaxVariance = INFINITY;
		int dim = -1, cut = -1;
		
		for(int x = xs; x < xe; x++) {
			float reg1var = CalRegionVariance(img, width, height, xs, ys, x, ye);
			float reg2var = CalRegionVariance(img, width, height, x, ys, xe, ye);
			float maxVar = max(reg1var,reg2var);
			if(maxVar < minMaxVariance) {
				dim = 0;
				cut = x;
				minMaxVariance = maxVar;
			}
		}

		for(int y = ys; y < ye; y++) {
			float reg1var = CalRegionVariance(img, width, height, xs, ys, xe, y);
			float reg2var = CalRegionVariance(img, width, height, xs, y, xe, ye);
			float maxVar = max(reg1var,reg2var);
			if(maxVar < minMaxVariance) {
				dim = 1;
				cut = y;
				minMaxVariance = maxVar;
			}
		}
		
		if(dim == 0) {
			RecursivePartition(img, width, height, xs, ys, cut, ye, depth-1, 
                minimizeVariance, light2cam);
			RecursivePartition(img, width, height, cut, ys, xe, ye, depth-1, 
                minimizeVariance, light2cam);	
		} else { //dim == 1
			RecursivePartition(img, width, height, xs, ys, xe, cut, depth-1, 
                minimizeVariance, light2cam);
			RecursivePartition(img, width, height, xs, cut, xe, ye, depth-1, 
                minimizeVariance, light2cam);
		}

	} else {
		//determine the longest dimension	
		float sinTheta = sinf((float)M_PI * (centerY+.5f)/float(height));
		int longestDim = float(xe-xs) > float(ye-ys)*sinTheta ? 0 : 1; // 0 -> x axis, 1 -> y axis	

		if(longestDim == 0) {
			//cal total energy in img
			float* lineEnergy = new float[xe-xs];
			float totalEnergy = 0.f;
			for(int x = xs; x < xe; x++) {
				lineEnergy[x-xs] = 0.f;
				for(int y = ys; y < ye; y++) {
					totalEnergy += img[x+y*width];					
					lineEnergy[x-xs] += img[x+y*width];
				}
			}

			int minX = xs;
			float minDiff = totalEnergy;
			float accumEnergy = 0.f;
			for(int x = xs; x < xe; x++) {
				accumEnergy += lineEnergy[x-xs];
				float diff = fabs(accumEnergy - (totalEnergy - accumEnergy));
				if(diff < minDiff) {
					minX = x+1;
					minDiff = diff;
				}
			}
			delete[] lineEnergy;

			RecursivePartition(img, width, height, xs, ys, minX, ye, depth-1, 
                minimizeVariance, light2cam);
			RecursivePartition(img, width, height, minX, ys, xe, ye, depth-1, 
                minimizeVariance, light2cam);	
		} else {
			//cal total energy in img
			float* lineEnergy = new float[ye-ys];
			float totalEnergy = 0.f;
			for(int y = ys; y < ye; y++) {
				lineEnergy[y-ys] = 0.f;
				for(int x = xs; x < xe; x++) {
					totalEnergy += img[x+y*width];					
					lineEnergy[y-ys] += img[x+y*width];
				}
			}

			int minY = ys;
			float minDiff = totalEnergy;
			float accumEnergy = 0.f;
			for(int y = ys; y < ye; y++) {
				accumEnergy += lineEnergy[y-ys];
				float diff = fabs(accumEnergy - (totalEnergy - accumEnergy));
				if(diff < minDiff) {
					minY = y+1;
					minDiff = diff;
				}
			}
			delete[] lineEnergy;
		
			RecursivePartition(img, width, height, xs, ys, xe, minY, depth-1, 
                minimizeVariance, light2cam);
			RecursivePartition(img, width, height, xs, minY, xe, ye, depth-1, 
                minimizeVariance, light2cam);
		}
	}
}

vector<shared_ptr<LightShader> > CreateMedianCutEnvLight(
    const Transform &light2cam, const ParameterList &params) {
    string texturename = params.FindString("texturename", "");
    bool mv = params.FindFloat("minimizevariance", 1.f) != 0.f;
    int ns = (int)params.FindFloat("nsamples", 64.f);
    float intensity = params.FindFloat("intensity", 1.f);
    MedianCutEnvLight mcel(light2cam, texturename, mv, ns, intensity);    
    return mcel.GetLights();
}

} //namespace ToyMan