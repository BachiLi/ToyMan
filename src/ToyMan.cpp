#include "ZBufferHider.h"
#include "Transform.h"
#include "Framework/ReyesFramework.h"
#include "Primitive/BilinearPatch.h"
#include "Primitive/Sphere.h"
#include "Display/FileDisplay.h"
#include "Display/FrameBufferDisplay.h"
#include "Util.h"
#include "Vector.h"
#include "ViewingFrustum.h"
#include "StratifiedSampler.h"
#include "MicroPolygon.h"
#include "DOFParameters.h"
#include "SurfaceShader/Matte.h"
#include "SurfaceShader/Plastic.h"
#include "SurfaceShader/PaintedPlastic.h"
#include "LightShader/DistantLight.h"
#include "LightShader/ShadowDistantLight.h"
#include "ShadowMap.h"
#include "Primitive.h"
#include "RIBParser/RIBLexer.h"
#include "RIBParser/RIBParser.h"
#include "PixelFilter.h"
#include "PixelFilter/BoxFilter.h"
#include "PixelFilter/GaussianFilter.h"
#include "SphericalHarmonics.h"
#include <iostream>
#include <memory>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <array>
#include <ctime>

using namespace std;
using namespace ToyMan;
using namespace std::tr1;

int main(int argc, char **argv)
{
    if(argc <= 1)
        return 0;
    fstream fs(argv[1], fstream::in);
    
    RIBParser::RIBLexer lexer(fs);    
    try {
        RIBParser::RIBParser parser(lexer);
        parser.Parse();
    } catch(exception &ex) {
        cout << "line" << lexer.line << endl;
        cout << ex.what() << endl;         
        system("pause");
    }    

    return 0;

/*
    int xRes = 512, yRes = 512;
	float zNear = 0.01f, zFar = 1000.f, fov = 90.f;
    Transform cameraToScreen = Perspective(fov, zNear, zFar);	
    float aspectRatio = float(xRes)/float(yRes);
	ViewingFrustum vf;
    vf.AddPerspective(zNear, zFar, aspectRatio, fov);
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
	float crop[4] = {0.f,1.f,0.f,1.f};
    shared_ptr<PixelFilter> filter =
        shared_ptr<PixelFilter>(new GaussianFilter(1.f, 1.f));
	//Display *display = new FileDisplay(xRes, yRes, crop, Display::RGB, "sphere.png");
    shared_ptr<Display> display = 
        shared_ptr<Display>(new FrameBufferDisplay(xRes, yRes, crop, Display::Z, "sphere", filter));
	shared_ptr<ImageSampler> sampler = 
        shared_ptr<ImageSampler>(new StratifiedSampler(1, 1, 0.f, 1.f));
	float focalLength = aspectRatio/tan(fov/2.f);	
	float lensDiameter = zNear;
	DOFParameters dParams;
    shared_ptr<Hider> hider = 
        shared_ptr<Hider>(new ZBufferHider(cameraToRaster, sampler, display, 16, 16, dParams));
    Framework *framework = new ReyesFramework(hider, vf);
	Transform worldToCamera;
    Transform cameraToWorld = worldToCamera.GetInverseMatrix();

    shared_ptr<LightList> lightList = shared_ptr<LightList>(new LightList());
    lightList->push_back(LightShaderSharedPtr(
        new DistantLight(1.f, Color(1.f,1.f,1.f))));

    framework->WorldBegin();
	Transform objectToWorld;
	shared_ptr<Primitive> prim;
    Attributes attr;

	objectToWorld = Translate(Vector(0.f, 0.f, 2.f));
    attr.objectToCamera = shared_ptr<Transform>(new Transform());
    *(attr.objectToCamera) = worldToCamera * objectToWorld;
	//attr.surfaceShader = shared_ptr<SurfaceShader>(new Matte(0.f, 1.f));
    attr.color = Color(1.f, 0.6f, 0.f);
    attr.lightSources = lightList;
    prim = shared_ptr<Primitive>(new Sphere(attr, 1.f, -1.f, 1.f, Radians(360.f)));
	framework->Insert(prim);

    framework->WorldEnd();
    return 0;
*/
/*
    int xRes = 512, yRes = 512;
	float zNear = 0.01f, zFar = 1000.f, fov = 90.f;
    Transform cameraToScreen = Perspective(fov, zNear, zFar);	
    float aspectRatio = float(xRes)/float(yRes);
	ViewingFrustum vf;
    vf.AddPerspective(zNear, zFar, aspectRatio, fov);
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
	float crop[4] = {0.f,1.f,0.f,1.f};
    shared_ptr<PixelFilter> filter =
        shared_ptr<PixelFilter>(new GaussianFilter(1.f, 1.f));
	//Display *display = new FileDisplay(xRes, yRes, crop, Display::RGB, "sphere.png");
    shared_ptr<Display> display = 
        shared_ptr<Display>(new FrameBufferDisplay(xRes, yRes, crop, Display::RGB, "sphere", filter));
	shared_ptr<ImageSampler> sampler = 
        shared_ptr<ImageSampler>(new StratifiedSampler(2, 2, 0.f, 1.f));
	float focalLength = aspectRatio/tan(fov/2.f);	
	float lensDiameter = zNear;
	DOFParameters dParams(focalLength/lensDiameter, focalLength, zFar);
    shared_ptr<Hider> hider = 
        shared_ptr<Hider>(new ZBufferHider(cameraToRaster, sampler, display, 16, 16, dParams));
    Framework *framework = new ReyesFramework(hider, vf);	   

	//Transform worldToCamera = LookAt(Point(-10.f,10.f,-10.f), Point(0.f,0.f,0.f), Vector(1.f, 0.f, -1.f));
	Transform worldToCamera = Rotate(-10.f, Vector(1.f, 0.f, 0.f));
	worldToCamera = Translate(Vector(0.f, 0.f, 10.f)) * worldToCamera;
    Transform cameraToWorld = worldToCamera.GetInverseMatrix();

    shared_ptr<LightList> lightList = shared_ptr<LightList>(new LightList());
    lightList->push_back(LightShaderSharedPtr(
        new ShadowDistantLight(1.f, Color(1.f,1.f,1.f), Point(-10.f,10.f,-10.f), Point(0.f,0.f,0.f), 
        ShadowMap("sphere.txt", cameraToWorld), 0.01f, 16, 0.5f)));

    framework->WorldBegin();
	Transform objectToWorld;
	//shared_ptr<Transform> objectToCamera, movement;
	shared_ptr<Primitive> prim;
	//shared_ptr<SurfaceShader> surfaceShader;
    Attributes attr;

	objectToWorld = Translate(Vector(-1.5f, 0.f, -2.f));
    attr.objectToCamera = shared_ptr<Transform>(new Transform());
    *(attr.objectToCamera) = worldToCamera * objectToWorld;
    attr.movementTransform = shared_ptr<Transform>(new Transform());
	*(attr.movementTransform) = Translate(Vector(0.f, 0.f, 0.f));
	//surfaceShader = shared_ptr<SurfaceShader>(new Matte(lightList, 0.f, 1.f));
    //surfaceShader = shared_ptr<SurfaceShader>(new PaintedPlastic(lightList, 0.f, 1.f, 0.5f, 0.1f, Color(1.f,1.f,1.f), "face3.png"));
    attr.surfaceShader = shared_ptr<SurfaceShader>(new Plastic(0.f, 1.f));
    attr.color = Color(1.f, 0.6f, 0.f);
    attr.lightSources = lightList;
    prim = shared_ptr<Primitive>(new Sphere(attr, 2.f, -2.f, 2.f, Radians(360.f)));
	framework->Insert(prim);

	objectToWorld = Translate(Vector(3.f, 0.f, 20.f));	
	attr.objectToCamera = shared_ptr<Transform>(new Transform());
	*(attr.objectToCamera) = worldToCamera * objectToWorld;
    attr.movementTransform = shared_ptr<Transform>(new Transform());
	attr.surfaceShader = shared_ptr<SurfaceShader>(new Matte(0.f, 1.f));
	attr.color = Color(0.6f, 1.0f, 0.f);
    attr.lightSources = lightList;
    prim = shared_ptr<Primitive>(new Sphere(attr, 3.f, -3.f, 3.f, Radians(360.f)));
	framework->Insert(prim);

	objectToWorld = Translate(Vector(0.f, -3.f, 0.f));
	attr.objectToCamera = shared_ptr<Transform>(new Transform());
	*(attr.objectToCamera) = worldToCamera * objectToWorld;
    attr.movementTransform = shared_ptr<Transform>(new Transform());
	//surfaceShader = shared_ptr<SurfaceShader>(new Matte(lightList, 0.f, 1.f));
    attr.surfaceShader = shared_ptr<SurfaceShader>(new PaintedPlastic(0.f, 1.f, 0.5f, 0.1f, Color(1.f,1.f,1.f), "grass.jpg"));
	attr.color = Color(0.5f, 0.5f, 0.5f);
    attr.texCoords = TextureCoordinates(0.f,0.f,10.f,0.f,0.f,10.f,10.f,10.f);
    attr.lightSources = lightList;
    prim = shared_ptr<Primitive>(new BilinearPatch(attr, Point(-30,0,-30),Point(30,0,-30),Point(-30,0,30),Point(30,0,30)));	
	framework->Insert(prim);

    framework->WorldEnd();

    //ShadowMap::MakeShadow("sphere.tif", "sphere.txt", worldToCamera, *cameraToRaster);

    delete framework;	
    return 0;
*/
}
