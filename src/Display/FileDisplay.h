#ifndef __FILE_DISPLAY_H__
#define __FILE_DISPLAY_H__

#include "../Display.h"
#include <string>
#include <FreeImagePlus.h>
#include <memory>

using namespace std;

namespace ToyMan {

class ParameterList;

class FileDisplay : public Display {
public:
    FileDisplay(int xRes, int yRes, const float crop[4], 
        const Channel &channel, const string &filename,
        shared_ptr<PixelFilter> filter);

    void UpdateScreen(int xs, int xe, int ys, int ye) {}
    void Done();
private:
    string m_Filename;
};

shared_ptr<Display> CreateFileDisplay(const string &name, int xRes, int yRes, 
    const float crop[4], const Display::Channel &channel, shared_ptr<PixelFilter> filter, 
    const ParameterList &params);

};

#endif //#ifndef __FILE_DISPLAY_H__
