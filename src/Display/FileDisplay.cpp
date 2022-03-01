#include "FileDisplay.h"
#include "../Color.h"
#include "../Util.h"
#include "../ParameterList.h"

namespace ToyMan {

FileDisplay::FileDisplay(int xRes, int yRes, const float crop[4], 
    const Channel &channel, const string& filename, shared_ptr<PixelFilter> filter)
    : Display(xRes, yRes, crop, channel, filter), m_Filename(filename)
{}

void FileDisplay::Done()
{
    fipImage image;
	FREE_IMAGE_TYPE type;
	switch(m_Channel) {
		case RGB: {
			image = fipImage(FIT_RGBF, m_CropWindow.GetWidth(), m_CropWindow.GetHeight(), 32);
            FIRGBF *pixels = (FIRGBF*)image.accessPixels();
            for(unsigned int i = 0; i < image.getWidth()*image.getHeight(); i++) {
                Color c = m_Pixels(i).color/m_Pixels(i).weight;
                pixels[i].red = c.x;
                pixels[i].green = c.y;
                pixels[i].blue = c.z;
            }
			break;
        } case RGBA: {
			image = fipImage(FIT_RGBAF, m_CropWindow.GetWidth(), m_CropWindow.GetHeight(), 32);
            FIRGBAF *pixels = (FIRGBAF*)image.accessPixels();
            for(unsigned int i = 0; i < image.getWidth()*image.getHeight(); i++) {
                float invWeight = 1.f/m_Pixels(i).weight;
                Color c = m_Pixels(i).color*invWeight;
                pixels[i].red = c.x;
                pixels[i].green = c.y;
                pixels[i].blue = c.z;
                pixels[i].alpha = m_Pixels(i).alpha*invWeight;
            }
			break;
        } case Z: {
            image = fipImage(FIT_FLOAT, m_CropWindow.GetWidth(), m_CropWindow.GetHeight(), 32);
            float *pixels = (float*)image.accessPixels();
            for(unsigned int i = 0; i < image.getWidth()*image.getHeight(); i++) {
                pixels[i] = m_Pixels(i).depth != INFINITY ? m_Pixels(i).depth : 0.f;
            }
			type = FIT_FLOAT;
            break;
        }
	}	

    if(m_Channel == RGB || m_Channel == RGBA) {
        string ext = GetFileExtension(m_Filename);
        if(ext != "exr" && ext != "hdr") {//don't need tone mapping for these two files
            image.toneMapping(FITMO_REINHARD05);
        }
    }
    
    image.save(m_Filename.c_str(), 0);
}

shared_ptr<Display> CreateFileDisplay(const string &name,
    int xRes, int yRes, const float crop[4], 
    const Display::Channel &channel, shared_ptr<PixelFilter> filter,
    const ParameterList &params) {

    shared_ptr<Display> display = 
        shared_ptr<Display>(new FileDisplay(xRes, yRes, crop, channel, name, filter));
    return display;
}

};
