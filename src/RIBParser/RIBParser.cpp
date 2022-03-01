#include "RIBParser.h"
#include "RIBLexer.h"
#include "RIBToken.h"

#include "../Util.h"
#include "../ShadowMap.h"
#include "../RayTracer.h"

//lights
#include "../LightShader/DistantLight.h"
#include "../LightShader/AmbientLight.h"
#include "../LightShader/ShadowDistantLight.h"
#include "../LightShader/PointLight.h"
#include "../LightShader/ShadowPointLight.h"
#include "../LightShader/MedianCutEnvLight.h"

//surfaces
#include "../SurfaceShader/Matte.h"
#include "../SurfaceShader/PaintedPlastic.h"
#include "../SurfaceShader/Plastic.h"
#include "../SurfaceShader/BakeRadiosity.h"
#include "../SurfaceShader/PointBasedColorBleeding.h"

//primitives
#include "../Primitive/Sphere.h"
#include "../Primitive/BilinearPatch.h"
#include "../Primitive/TriangleMesh.h"

//hiders
#include "../ZBufferHider.h"

//frameworks
#include "../Framework.h"
#include "../Framework/ReyesFramework.h"

//samplers
#include "../StratifiedSampler.h"

//displays
#include "../Display/FileDisplay.h"
#include "../Display/FrameBufferDisplay.h"

//filters
#include "../PixelFilter/BoxFilter.h"
#include "../PixelFilter/GaussianFilter.h"

#include <string>
#include <map>
#include <functional>
#include <typeinfo>

using namespace std;
using namespace std::tr1;

namespace ToyMan {
namespace RIBParser {

function<void (RIBParser*)> RIBParser::ParseMap(const string &keyword) {
    static map<string, function<void (RIBParser*)> > s_ParseMap;
    static bool s_Initialized = false;
    if(!s_Initialized) {
        s_ParseMap["AttributeBegin"]       = &ParseAttributeBegin;
        s_ParseMap["AttributeEnd"]         = &ParseAttributeEnd;
        s_ParseMap["Clipping"]             = &ParseClipping;
        s_ParseMap["Color"]                = &ParseColor;
        s_ParseMap["ConcatTransform"]      = &ParseConcatTransform;
        s_ParseMap["DepthOfField"]         = &ParseDepthOfField;
        s_ParseMap["Display"]              = &ParseDisplay;        
        s_ParseMap["Format"]               = &ParseFormat;                
        s_ParseMap["FrameAspectRatio"]     = &ParseFrameAspectRatio;
        s_ParseMap["FrameBegin"]           = &ParseFrameBegin;
        s_ParseMap["FrameEnd"]             = &ParseFrameEnd;
        s_ParseMap["Hider"]                = &ParseHider;
        s_ParseMap["Identity"]             = &ParseIdentity;
        s_ParseMap["LightSource"]          = &ParseLightSource;
        s_ParseMap["LookAt"]               = &ParseLookAt;
        s_ParseMap["MakeShadow"]           = &ParseMakeShadow;
        s_ParseMap["Opacity"]              = &ParseOpacity;
        s_ParseMap["Option"]               = &ParseOption;
        s_ParseMap["Patch"]                = &ParsePatch;
        s_ParseMap["PixelFilter"]          = &ParsePixelFilter;
        s_ParseMap["PixelSamples"]         = &ParsePixelSamples;
        s_ParseMap["Polygon"]              = &ParsePolygon;
        s_ParseMap["Projection"]           = &ParseProjection;
        s_ParseMap["Rotate"]               = &ParseRotate;
        s_ParseMap["Scale"]                = &ParseScale;
        s_ParseMap["ShadingRate"]          = &ParseShadingRate;
        s_ParseMap["ShadingInterpolation"] = &ParseShadingInterpolation;
        s_ParseMap["Sides"]                = &ParseSides;
        s_ParseMap["Sphere"]               = &ParseSphere;
        s_ParseMap["Surface"]              = &ParseSurface;
        s_ParseMap["Transform"]            = &ParseTransform;  
        s_ParseMap["TransformBegin"]       = &ParseTransformBegin;        
        s_ParseMap["TransformEnd"]         = &ParseTransformEnd;        
        s_ParseMap["Translate"]            = &ParseTranslate;        
        s_ParseMap["WorldBegin"]           = &ParseWorldBegin;
        s_ParseMap["WorldEnd"]             = &ParseWorldEnd;
        s_Initialized = true;
    }
    if(s_ParseMap.find(keyword) == s_ParseMap.end())         
        throw SyntaxErrorException();
    return s_ParseMap[keyword];
}

RIBParser::RIBParser(RIBLexer &lexer) 
    : m_Lexer(lexer), m_LookAHead(lexer.NextToken()) {
    //setup graphics state stack
    m_ModeStack.push(BASE);
    m_OptionsStack.push(Options());
    m_AttributesStack.push(Attributes());
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    m_TransformStack.push(transform);
    m_Timer.Start();
}

void RIBParser::Move() {
    m_LookAHead = m_Lexer.NextToken();
}

void RIBParser::Match(const RIBToken::Tag &tag) {
    if(m_LookAHead.tag == tag) {
        Move();
        return;
    }
    throw SyntaxErrorException();
}

bool RIBParser::IsMode(Mode mode) {
    return (m_ModeStack.top() & mode) != 0;
}

void RIBParser::Parse() {
    while(m_LookAHead.tag != RIBToken::END_OF_FILE) {
        if(m_LookAHead.tag != RIBToken::KEYWORD) {
            throw SyntaxErrorException();
        }       

        function<void (RIBParser*)> f = ParseMap(m_LookAHead.str);
        Move(); //eat the keyword       
        f(this);
    }
}

inline ParameterList RIBParser::ParseParameterList() {
    ParameterList result;
    while(m_LookAHead.tag == RIBToken::STRING) {        
        string id = m_LookAHead.str;
        Move();
        Match(RIBToken::ARRAY_BEGIN);        
        if(m_LookAHead.tag == RIBToken::NUMBER) {            
            vector<float> floats;
            while(m_LookAHead.tag != RIBToken::ARRAY_END) {               
                floats.push_back(ParseNumber());
            }
            //TODO: check duplication
            result.numberList[id] = floats;
        } else if(m_LookAHead.tag == RIBToken::STRING) {
            vector<string> strings;
            while(m_LookAHead.tag != RIBToken::ARRAY_END) {
                strings.push_back(ParseString());
            }
            //TODO: check duplication
            result.stringList[id] = strings;
        } else {            
            throw SyntaxErrorException();
        }
        Match(RIBToken::ARRAY_END);
    }
    return result;
}

inline vector<float> RIBParser::ParseFloatArray() {
    vector<float> result;
    Match(RIBToken::ARRAY_BEGIN);
    while(m_LookAHead.tag != RIBToken::ARRAY_END)  {
        result.push_back(ParseNumber());
    }
    Match(RIBToken::ARRAY_END);
    return result;
}

inline vector<string> RIBParser::ParseStringArray() {
    vector<string> result;
    Match(RIBToken::ARRAY_BEGIN);
    while(m_LookAHead.tag != RIBToken::ARRAY_END)  {        
        result.push_back(ParseString());        
    }
    Match(RIBToken::ARRAY_END);
    return result;
}

inline float RIBParser::ParseNumber() {
    if(m_LookAHead.tag != RIBToken::NUMBER) {
        throw SyntaxErrorException();
    }
    float num = m_LookAHead.num;
    Move();
    return num;
}

inline string RIBParser::ParseString() {
    if(m_LookAHead.tag != RIBToken::STRING)
        throw SyntaxErrorException();
    string str = m_LookAHead.str;
    Move();
    return str;
}

inline Matrix4x4 RIBParser::ParseMatrix() {
    vector<float> m = ParseFloatArray();
    if(m.size() != 16)
        throw SyntaxErrorException();
    Matrix4x4 mat(
        m[0], m[1], m[2], m[3],
        m[4], m[5], m[6], m[7],
        m[8], m[9],m[10],m[11],
       m[12],m[13],m[14],m[15]);
    return mat;
}

void RIBParser::ParseAttributeBegin() {
    m_ModeStack.push(ATTRIBUTE);
    m_AttributesStack.push(m_AttributesStack.top());
}

void RIBParser::ParseAttributeEnd() {
    m_ModeStack.pop();
    m_AttributesStack.pop();
}

void RIBParser::ParseClipping() {
    m_OptionsStack.top().zNear = ParseNumber();
    m_OptionsStack.top().zFar  = ParseNumber();
}

void RIBParser::ParseColor() {
    vector<float> color = ParseFloatArray();    
    if(color.size() != 3)
        throw SyntaxErrorException();
    m_AttributesStack.top().color
        = Color(color[0],color[1],color[2]);
}

void RIBParser::ParseConcatTransform() {
    Transform t = ParseMatrix();
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    *transform = t * 
        (*m_AttributesStack.top().objectToCamera);
    m_AttributesStack.top().objectToCamera = transform;      
}

void RIBParser::ParseDepthOfField() {
    float fs = ParseNumber();
    float fl = ParseNumber();
    float fd = ParseNumber();
    m_OptionsStack.top().dofParameters
        = DOFParameters(fs, fl, fd);
}

void RIBParser::ParseDisplay() {
    m_OptionsStack.top().displayName = ParseString();
    m_OptionsStack.top().displayType = ParseString();
    string channelStr = ParseString();
    Display::Channel channel = Display::RGB;
    if(channelStr == "rgb") {
        channel = Display::RGB;
    } else if(channelStr == "rgba") {
        channel = Display::RGBA;
    } else if(channelStr == "z") {
        channel = Display::Z;
    } else {
        //TODO: send warning
    }
    m_OptionsStack.top().channel = channel;
    m_OptionsStack.top().dispParameters = ParseParameterList();
}

void RIBParser::ParseFormat() {
    m_OptionsStack.top().xRes = (int)ParseNumber();
    m_OptionsStack.top().yRes = (int)ParseNumber();
    m_OptionsStack.top().pixelAspectRatio = ParseNumber();
}

void RIBParser::ParseFrameAspectRatio() {
    m_OptionsStack.top().frameAspectRatio = ParseNumber();
}

void RIBParser::ParseFrameBegin() {
    if(m_ModeStack.top() != BASE) 
        throw SyntaxErrorException();
        
    m_ModeStack.push(FRAME);
    m_AttributesStack.push(m_AttributesStack.top());
    m_OptionsStack.push(m_OptionsStack.top());    
    m_OptionsStack.top().frameNumber = (int)ParseNumber();

    //create ray tracer
    m_RayTracer = shared_ptr<RayTracer>(new RayTracer());
}

void RIBParser::ParseFrameEnd() {
    if(m_ModeStack.top() != FRAME)
        throw SyntaxErrorException();
    m_ModeStack.pop();
    m_AttributesStack.pop();
    m_OptionsStack.pop();
}

void RIBParser::ParseHider() {
    m_OptionsStack.top().hiderType = ParseString();
    m_OptionsStack.top().hiderParameters = ParseParameterList();
}

void RIBParser::ParseIdentity() {
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    m_AttributesStack.top().objectToCamera = transform;
}

void RIBParser::ParseLightSource() {
    string name = ParseString();
    int seqNum = (int)ParseNumber();
    ParameterList params = ParseParameterList();
    vector<shared_ptr<LightShader>> lights = CreateLightShader(
        (*m_AttributesStack.top().objectToCamera), name, seqNum, params);
    m_AttributesStack.top().lightSources->insert(
        m_AttributesStack.top().lightSources->end(),
        lights.begin(), lights.end());
    //if(light.get() != NULL)
    //    m_AttributesStack.top().lightSources->push_back(light);
}

void RIBParser::ParseLookAt() {
    float px = ParseNumber();
    float py = ParseNumber();
    float pz = ParseNumber();
    float lx = ParseNumber();
    float ly = ParseNumber();
    float lz = ParseNumber();
    float ux = ParseNumber();
    float uy = ParseNumber();
    float uz = ParseNumber();
    
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    *transform = LookAt(Point(px, py, pz), Point(lx, ly, lz), Vector(ux,uy,uz)) * 
        (*m_AttributesStack.top().objectToCamera);
    m_AttributesStack.top().objectToCamera = transform;
}

void RIBParser::ParseMakeShadow() {
    string picturename = ParseString();
    string texturename = ParseString();
    ParameterList params = ParseParameterList();
    ShadowMap::MakeShadow(
        picturename, texturename, 
        (*m_AttributesStack.top().objectToCamera),
        CreateProjection(
        m_OptionsStack.top().projectionType,
        m_OptionsStack.top().projParameters));
}

void RIBParser::ParseOpacity() {
    vector<float> opacity = ParseFloatArray();    
    if(opacity.size() != 3)
        throw SyntaxErrorException();
    m_AttributesStack.top().opacity
        = Color(opacity[0],opacity[1],opacity[2]);
}

void RIBParser::ParseOption() {    
    string name = ParseString();
    if(name == "dice") {
        ParameterList params = ParseParameterList();
        m_OptionsStack.top().rasterOrient =
            params.FindFloat("rasterorient", 
            m_OptionsStack.top().rasterOrient ? 1.f : 0.f) != 0.f;
    } else if(name == "cull") {
        ParameterList params = ParseParameterList();
        m_OptionsStack.top().backFaceCulling =
            params.FindFloat("backfacing",
            m_OptionsStack.top().backFaceCulling ? 1.f : 0.f) != 0.f;
    } else {
        //TODO: issue warning
    }
}

void RIBParser::ParsePatch() {
    if(IsMode((Mode)(BASE | FRAME)))
        throw SyntaxErrorException();

    string type = ParseString();
    if(type == "bilinear") {
        ParameterList params = ParseParameterList();
        vector<Point> points = params.FindPointList("P");
        if(points.size() == 4)  {
            shared_ptr<Primitive> prim
                = shared_ptr<Primitive>(new BilinearPatch(
                shared_ptr<Attributes>(new Attributes(m_AttributesStack.top())), 
                points[0], points[1],
                points[2], points[3]));
            assert(m_Framework.get() != NULL);
            m_Framework->Insert(prim);            
            m_RayTracer->Insert(prim);
        } else {
            //TODO: issue warning
        }
    } else {
        //TODO: issue warning
    }
}

void RIBParser::ParsePolygon() {
    if(IsMode((Mode)(BASE | FRAME)))
        throw SyntaxErrorException();

    string type = ParseString();
    if(type == "trianglemesh") {
        ParameterList params = ParseParameterList();
        vector<Point> points = params.FindPointList("P");
        vector<float> fIndices = params.FindFloatList("indices");
        vector<int> indices(fIndices.size());
        for(size_t i = 0; i < fIndices.size(); i++)
            indices[i] = (int)fIndices[i];
        vector<float> st = params.FindFloatList("st");
        shared_ptr<Primitive> prim
            = shared_ptr<Primitive>(new TriangleMesh(
                shared_ptr<Attributes>(new Attributes(m_AttributesStack.top())),
                indices.size()/3, points.size(), &indices[0], &points[0],
                NULL, st.size() > 0 ? &st[0] : NULL));                
        m_Framework->Insert(prim);        
        m_RayTracer->Insert(prim);        
    }
}

void RIBParser::ParsePixelFilter() {
    m_OptionsStack.top().filterType = ParseString();
    m_OptionsStack.top().xWidth = ParseNumber();
    m_OptionsStack.top().yWidth = ParseNumber();
}

void RIBParser::ParsePixelSamples() {
    m_OptionsStack.top().xSamples = (int)ParseNumber();
    m_OptionsStack.top().ySamples = (int)ParseNumber();
}

void RIBParser::ParseProjection() {
    m_OptionsStack.top().projectionType = ParseString();
    m_OptionsStack.top().projParameters = ParseParameterList();
}

void RIBParser::ParseRotate() {
    float angle = ParseNumber();
    float dx = ParseNumber();
    float dy = ParseNumber();
    float dz = ParseNumber();
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    *transform = Rotate(angle, Vector(dx,dy,dz)) * 
        (*m_AttributesStack.top().objectToCamera);
    m_AttributesStack.top().objectToCamera = transform;        
}

void RIBParser::ParseScale() {
    float x = ParseNumber();
    float y = ParseNumber();
    float z = ParseNumber();
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    *transform = Scale(x,y,z) * 
        (*m_AttributesStack.top().objectToCamera);
    m_AttributesStack.top().objectToCamera = transform;        
}

void RIBParser::ParseShadingRate() {
    m_AttributesStack.top().shadingRate = ParseNumber();
}

void RIBParser::ParseShadingInterpolation() {
    string shadingInterpolation = ParseString();
    if(shadingInterpolation == "constant") {
        m_AttributesStack.top().shadingInterpolation = CONSTANT;
    } else if(shadingInterpolation == "smooth") {
        m_AttributesStack.top().shadingInterpolation = SMOOTH;
    } else {
        //TODO: issue warning
    }
}

void RIBParser::ParseSides() {
    int sides = (int)ParseNumber();
    if(sides == 1) {
        m_AttributesStack.top().sides = ONE;
    } else if(sides == 2) {
        m_AttributesStack.top().sides = TWO;
    } else {
        //TODO: issue warning
    }
}

void RIBParser::ParseSphere() {
    if(IsMode((Mode)(BASE | FRAME)))
        throw SyntaxErrorException();

    float radius = ParseNumber();
    float zmin = ParseNumber();
    float zmax = ParseNumber();
    float thetamax = Radians(ParseNumber());
    ParameterList params = ParseParameterList();
    shared_ptr<Primitive> prim
        = shared_ptr<Primitive>(new Sphere(
        shared_ptr<Attributes>(new Attributes(m_AttributesStack.top())), 
        radius, zmin,
        zmax, thetamax));
    assert(m_Framework.get() != NULL);
    m_Framework->Insert(prim);
    m_RayTracer->Insert(prim);
}

void RIBParser::ParseSurface() {
    if(IsMode((Mode)(BASE | FRAME)))
        throw SyntaxErrorException();

    string shaderName = ParseString();
    ParameterList params = ParseParameterList();
    shared_ptr<SurfaceShader> surface =
        CreateSurfaceShader(shaderName, params);
    if(surface.get() != NULL)
        m_AttributesStack.top().surfaceShader = surface;    
}

void RIBParser::ParseTransform() {
    Transform t = ParseMatrix();
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    *transform = t;
    m_AttributesStack.top().objectToCamera = transform; 
}

void RIBParser::ParseTransformBegin() {
    m_TransformStack.push(m_AttributesStack.top().objectToCamera);
    m_ModeStack.push(TRANSFORM);
}

void RIBParser::ParseTransformEnd() {    
    m_TransformStack.pop();
    m_AttributesStack.top().objectToCamera
        = m_TransformStack.top();
    m_ModeStack.pop();
}

void RIBParser::ParseTranslate() {
    float dx = ParseNumber();
    float dy = ParseNumber();
    float dz = ParseNumber();    
    shared_ptr<Transform> transform
        = shared_ptr<Transform>(new Transform());
    *transform = Translate(Vector(dx,dy,dz)) * 
        (*m_AttributesStack.top().objectToCamera);
    m_AttributesStack.top().objectToCamera = transform; 
}

void RIBParser::ParseWorldBegin() {
    m_ModeStack.push(WORLD);
    m_AttributesStack.push(m_AttributesStack.top());
    m_OptionsStack.push(m_OptionsStack.top());
    
    //create renderer
    m_Framework = CreateFramework(
        m_OptionsStack.top().frameworkType, m_OptionsStack.top().fwParameters);
    m_Framework->WorldBegin();
}

void RIBParser::ParseWorldEnd() {
    m_Framework->WorldEnd();
    m_ModeStack.pop();
    m_AttributesStack.pop();
    m_OptionsStack.pop();    
    cout << "render time: " << m_Timer.Time() << "s" << endl;
    m_Timer.Stop();
}

vector<shared_ptr<LightShader> > RIBParser::CreateLightShader(
    const Transform &light2cam, const string &name, int seqnum, const ParameterList &params) {
    //TODO: make sequence number useful
    vector<shared_ptr<LightShader> > shaders;
    if(name == "ambientlight") {
        shaders.push_back(CreateAmbientLight(params));
        return shaders;
    } else if(name == "distantlight") {
        shaders.push_back(CreateDistantLight(light2cam, params));
        return shaders;
    } else if(name == "shadowdistantlight") {
        shaders.push_back(CreateShadowDistantLight(light2cam, 
            params, m_AttributesStack.top().objectToCamera->GetInverseMatrix()));
        return shaders;
    } else if(name == "pointlight") {
        shaders.push_back(CreatePointLight(light2cam, params));
        return shaders;    
    } else if(name == "shadowpointlight") {
        shaders.push_back(CreateShadowPointLight(light2cam, m_RayTracer, params));
        return shaders;
    } else if(name == "mediancutenvlight") {        
        return CreateMedianCutEnvLight(light2cam, params);
    } else {
        //TODO: issue warning
        //shared_ptr<LightShader> empty;
        return shaders;
    }
}

shared_ptr<SurfaceShader> RIBParser::CreateSurfaceShader(
    const string &name, const ParameterList &params) {

    if(name == "matte") {
        return CreateMatte(params);
    } else if(name == "paintedplastic") {
        return CreatePaintedPlastic(params);
    } else if(name == "plastic") {
        return CreatePlastic(params);
    } else if(name == "bakeradiosity") {
        return CreateBakeRadiosity(params);
    } else if(name == "pointbasedcolorbleeding") {
        return CreatePointBasedColorBleeding(params);
    } else {
        //TODO: issue warning
        shared_ptr<SurfaceShader> empty;
        return empty;
    }
}

shared_ptr<Framework> RIBParser::CreateFramework( const string &name, 
    const ParameterList &params) {
    if(name == "reyes") {
        shared_ptr<Hider> hider = CreateHider(
            m_OptionsStack.top().hiderType, m_OptionsStack.top().hiderParameters);        
        shared_ptr<Framework> framework = CreateReyesFramework(hider,
            m_OptionsStack.top().viewingFrustum, m_OptionsStack.top().backFaceCulling);
        return framework;
    } else {
        //TODO: issue warning
        return CreateFramework("reyes", ParameterList());
    }
}

shared_ptr<Hider> RIBParser::CreateHider(const string &name,
    const ParameterList &params) {
    if(name == "hidden") {
        Transform cameraToScreen = CreateProjection(
            m_OptionsStack.top().projectionType, m_OptionsStack.top().projParameters);

        int xRes = m_OptionsStack.top().xRes;
        int yRes = m_OptionsStack.top().yRes;
        float aspectRatio = float(xRes)/float(yRes);	    
        float screen[4];
        if(aspectRatio > 1.f) {
            screen[0] = -aspectRatio; screen[1] = aspectRatio;
            screen[2] = -1.f        ; screen[3] = 1.f;
        } else {
            screen[0] = -1.f             ; screen[1] = 1.f;
            screen[2] = -1.f/aspectRatio ; screen[3] = 1.f/aspectRatio;
        }
        Transform screenToRaster = Scale((float)xRes, (float)yRes, 1.f) *
                                   Scale(1.f/(screen[1]-screen[0]), 1.f/(screen[3]-screen[2]), 1.f) *
                                   Translate(Vector(-screen[0],-screen[2],0.f));

        shared_ptr<Transform> cameraToRaster = shared_ptr<Transform>(new Transform());
        *cameraToRaster = screenToRaster * cameraToScreen;

        shared_ptr<Display> display = CreateDisplay(
            m_OptionsStack.top().displayName, m_OptionsStack.top().displayType,
            m_OptionsStack.top().channel, m_OptionsStack.top().dispParameters);

        shared_ptr<ImageSampler> sampler = CreateImageSampler(m_OptionsStack.top().samplerType, 
            m_OptionsStack.top().xSamples, m_OptionsStack.top().ySamples, 
            m_OptionsStack.top().shutterOpen, m_OptionsStack.top().shutterClose,
            m_OptionsStack.top().smplParameters);
        return CreateZBufferHider(
            cameraToRaster, display, sampler, 
            m_OptionsStack.top().rasterOrient, 
            m_OptionsStack.top().dofParameters, params);
    } else {
        //TODO: issue warning
        return CreateHider("hidden", params);
    }
}

shared_ptr<Display> RIBParser::CreateDisplay(const string &name,
    const string &type, const Display::Channel &channel,
    const ParameterList &params) {
    shared_ptr<PixelFilter> filter
        = CreatePixelFilter(m_OptionsStack.top().filterType,
                       m_OptionsStack.top().xWidth,
                       m_OptionsStack.top().yWidth);
    if(type == "file") {
        shared_ptr<Display> display
            = CreateFileDisplay(name, 
              m_OptionsStack.top().xRes,
              m_OptionsStack.top().yRes,
              m_OptionsStack.top().cropWindow,
              channel, filter,
              params);
        return display;
    } else if(type == "framebuffer") {
        shared_ptr<Display> display
            = CreateFrameBufferDisplay(name, 
              m_OptionsStack.top().xRes,
              m_OptionsStack.top().yRes,
              m_OptionsStack.top().cropWindow,
              channel, filter,
              params);
        return display;
    } else {
        //TODO: issue warning
        return CreateDisplay("", "framebuffer", Display::RGBA, ParameterList());        
    }
}

shared_ptr<PixelFilter> RIBParser::CreatePixelFilter(const string &name,
    float xWidth, float yWidth) {
    if(name == "box") {
        shared_ptr<PixelFilter> filter =
            CreateBoxFilter(xWidth/2.f, yWidth/2.f);
        return filter;
    } else if(name == "gaussian") {        
        shared_ptr<PixelFilter> filter =
            CreateGaussianFilter(xWidth/2.f, yWidth/2.f);
        return filter;
    } else {
        //TODO: issue warning
        return CreatePixelFilter("gaussian", 2.f, 2.f);
    }
}

Transform RIBParser::CreateProjection(const string &name, 
    const ParameterList &params) {
    if(name == "perspective") {
        float fov = params.FindFloat("fov", 90.f);
        float zNear = m_OptionsStack.top().zNear;
        float zFar = m_OptionsStack.top().zFar;
        m_OptionsStack.top().viewingFrustum.AddPerspective(
            zNear, zFar, 
            (float)m_OptionsStack.top().xRes/(float)m_OptionsStack.top().yRes,
            fov);
        return Perspective(fov, zNear, zFar);
    } else {
        //TODO: issue warning
        return CreateProjection("perspective", ParameterList());
    }
}

shared_ptr<ImageSampler> RIBParser::CreateImageSampler(const string &name,
    int xSample, int ySample, float sOpen, float sClose,
    const ParameterList &params) {    

    if(name == "stratified") {
        return CreateStratifiedSampler(xSample, ySample, sOpen, sClose, params);
    } else {
        //TODO: issue warning
        return CreateImageSampler("stratified", 
            m_OptionsStack.top().xSamples, m_OptionsStack.top().ySamples, 
            m_OptionsStack.top().shutterOpen, m_OptionsStack.top().shutterClose,
            ParameterList());
    }    
}

} //namespace RIBParser
} //namespace ToyMan
