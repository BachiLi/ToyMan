#ifndef __RIBPARSER_RIBPARSER_H__
#define __RIBPARSER_RIBPARSER_H__

#include "RIBToken.h"
#include "../DOFParameters.h"
#include "../Display.h"
#include "../Primitive.h"
#include "../Transform.h"
#include "../LightShader.h"
#include "../ParameterList.h"
#include "../ViewingFrustum.h"
#include "../Timer.h"
#include <stack>
#include <string>
#include <map>
#include <vector>
#ifdef _MSC_VER
	#include <memory>
#else
	#include <tr1/memory>
#endif 
#include <exception>
#include <functional>

using namespace std;
using namespace std::tr1;

namespace ToyMan {    

class Hider;
class Framework;
class RayTracer;
class ImageSampler;

namespace RIBParser {

class SyntaxErrorException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "Syntax error.";
    }
};

class RIBLexer;

class RIBParser {
public:
    RIBParser(RIBLexer &lexer);
    void Parse();

private:
    void Move();
    void Match(const RIBToken::Tag &tag);


    inline ParameterList ParseParameterList();
    inline vector<float> ParseFloatArray();
    inline vector<string> ParseStringArray();
    inline float ParseNumber();
    inline string ParseString();
    inline Matrix4x4 ParseMatrix();

    void ParseAttributeBegin();
    void ParseAttributeEnd();
    void ParseClipping();
    void ParseColor();
    void ParseConcatTransform();
    void ParseDepthOfField();
    void ParseDisplay();
    void ParseFormat();
    void ParseFrameAspectRatio();
    void ParseFrameBegin();
    void ParseFrameEnd();
    void ParseHider();
    void ParseIdentity();
    void ParseLightSource();
    void ParseLookAt();
    void ParseMakeShadow();
    void ParseOpacity();
    void ParseOption();
    void ParsePatch();
    void ParsePixelFilter();
    void ParsePixelSamples();
    void ParsePolygon();
    void ParseProjection();
    void ParseRotate();
    void ParseScale();
    void ParseShadingRate();
    void ParseShadingInterpolation();
    void ParseSides();
    void ParseSphere();
    void ParseSurface();
    void ParseTransform();
    void ParseTransformBegin();
    void ParseTransformEnd();
    void ParseTranslate();
    void ParseWorldBegin();
    void ParseWorldEnd();

    vector<shared_ptr<LightShader> > CreateLightShader(
        const Transform &light2cam, const string &name, int seqnum, 
        const ParameterList &params);
    shared_ptr<SurfaceShader> CreateSurfaceShader(const string &name,
        const ParameterList &params);
    shared_ptr<Framework> CreateFramework(const string &name, 
        const ParameterList &params);
    shared_ptr<Hider> CreateHider(const string &name,
        const ParameterList &params);
    shared_ptr<Display> CreateDisplay(const string &name,
        const string &type, const Display::Channel &Channel,
        const ParameterList &params);
    shared_ptr<PixelFilter> CreatePixelFilter(const string &name,
        float xWidth, float yWidth);
    shared_ptr<ImageSampler> CreateImageSampler(const string &name,
        int xSample, int ySample, float sOpen, float sClose,
        const ParameterList &params);
    Transform CreateProjection(const string &name, 
        const ParameterList &params);

    static function<void (RIBParser*)> ParseMap(const string &keyword);

    enum Mode {
        BASE      = 0x1 << 0,
        FRAME     = 0x1 << 1,
        WORLD     = 0x1 << 2,
        ATTRIBUTE = 0x1 << 3,
        TRANSFORM = 0x1 << 4,
        SOLID     = 0x1 << 5,
        OBJECT    = 0x1 << 6,
        MOTION    = 0x1 << 7
    };
    
    bool IsMode(Mode mode);

    struct Options {
        Options() {
            frameNumber = -1;
            zNear = 0.01f; zFar = 1000.f;
            cropWindow[0] = 0.f; cropWindow[1] = 1.f;
            cropWindow[2] = 0.f; cropWindow[3] = 1.f;
            shutterOpen = 0.f; shutterClose = 1.f;
            xRes = 512; yRes = 512; pixelAspectRatio = 1.f;
            frameAspectRatio = 1.f;
            xSamples = ySamples = 2;
            projectionType = "Perspective";
            hiderType = "hidden";
            filterType = "gaussian";
            xWidth = yWidth = 2.f;
            frameworkType = "reyes";
            samplerType = "stratified";
            rasterOrient = true;
            backFaceCulling = true;
        }

        int frameNumber;                 //RiFrameBegin
        
        float zNear, zFar;               //RiClipping
        
        float cropWindow[4];             //RiCropWindow
        
        DOFParameters dofParameters;     //RiDepthOfField
        
        string displayName;
        string displayType;
        Display::Channel channel;    
        ParameterList dispParameters;    //RiDisplay

        float shutterOpen, shutterClose; //RiShutter
        
        int xRes, yRes;
        float pixelAspectRatio;          //RiFormat
        
        float frameAspectRatio;          //RiFrameAspectRatio
        
        int xSamples, ySamples;          //RiPixelSamples
        
        string projectionType;        
        ParameterList projParameters;    //RiProjection

        string hiderType;
        ParameterList hiderParameters;   //RiHider

        ViewingFrustum viewingFrustum;   //RiClippingPlane

        string filterType;
        float xWidth, yWidth;            //RipixelFilter

        string frameworkType;
        ParameterList fwParameters;      //custom options

        string samplerType;
        ParameterList smplParameters;    //custom options

        bool rasterOrient;               //custom options    

        bool backFaceCulling;            //custom options    
    };

    stack<Mode> m_ModeStack;
    stack<Options> m_OptionsStack;
    stack<Attributes> m_AttributesStack;
    stack<shared_ptr<Transform> > m_TransformStack;

    shared_ptr<Framework> m_Framework;
    shared_ptr<RayTracer> m_RayTracer;

    RIBLexer &m_Lexer;
    RIBToken m_LookAHead;

    Timer m_Timer;
};

} //namespace ToyMan
} //namespace RIBParser

#endif //#ifndef __RIBPARSER_RIBPARSER_H__