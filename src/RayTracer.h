#pragma once
#include <string>
#include <vector>
#include "../external/json.hpp"
#include "../external/simpleppm.h"
#include <Eigen/Core>
#include <Eigen/Dense>
//tunign with  ./raytracer ../assets/test_scene1.json
using namespace std;
using json = nlohmann::json;
//forward declaration of ray class
class Ray;
class RayTracer {
public:
    
    RayTracer(const json& j); //Parameterized Constructor
    ~RayTracer(); //Destructor
    /*i dont think I need these but jsut in case
    RayTracer(); //Default Constructor
    RayTracer(const RayTracer& other); //Copy COnstructor
    RayTracer& operator=(const RayTracer& other); //Assignment Operator
    friend std::ostream& operator<<(std::ostream& os, const RayTracer& rt); //Output Stream Operator (if needed)
    */
    void run();
    //getters and setters)
    
private:

    struct Geometry{
        string comment;
        string type; //the type of object
        bool typejson = false;

        //need tofigure
        float radius = -1; //radius of sphere
        bool radjson = false;
        Eigen::Vector3f centre = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //centre of object
        bool centrejson = false;
        Eigen::Vector3f p1 = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //rectangle corners
        Eigen::Vector3f p2, p3, p4; //rectangle corners
        bool rectjson = false;
        Eigen::Vector3f edgea, edgeb;
        Eigen::Vector3f plane; //normal, plane makes more sense to me
        Eigen::Vector3f ac = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //ambient color RGB
        bool acjson = false;
        Eigen::Vector3f dc = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //diffuse color RGB
        bool dcjson = false;
        Eigen::Vector3f sc = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //specular color RGB
        bool scjson = false;
        //floats between 0-1
        float ka = -1; //ambient reflection coefficient RGB
        bool kajson = false;
        float kd = -1; //diffuse reflection coefficient RGB
        bool kdjson = false;
        float ks = -1; //specular reflection coefficient RGB
        bool ksjson = false;
        float pc = -1; //phong coefficient, less than one
        bool pcjson = false;

        bool visible = true;
        Eigen::Matrix4f transform = Eigen::Matrix4f::Identity(); //transformation matrix

    };
    struct Light{
        string comment;
        string type; //the type of object 
        bool typejsonlight = false;
        Eigen::Vector3f p1 = Eigen::Vector3f(-1.0f, 0.0f, 0.0f);
        Eigen::Vector3f p2, p3, p4; //rectangle corners
        bool pointsjson = false;
        Eigen::Vector3f id {-1.0f, 1.0f, 1.0f}; //diffuse light RGB
        bool idjson = false;
        Eigen::Vector3f is {-1.0f, 1.0f, 1.0f}; //specular light RGB
        bool isjson = false;
        Eigen::Vector3f centre = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //centre of object
        bool centrejsonlight = false;

        int n; //statified sampling using grid of nxn?
        bool usecenter; //Debug feature for light from centre
        bool use = true; //whether to use this light or not
        Eigen::Matrix4f transform = Eigen::Matrix4f::Identity(); //transformation matrix

    };

    struct Output{
        //this is the camera and the worldspace
        string filename; //ppm file for output
        bool outFileJson = false;
        vector<int> size {-1,-1}; //resolution
        bool outSizeJson = false;
        Eigen::Vector3f lookat = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //not a point but a vector
        bool outLookJson = false;
        
        Eigen::Vector3f up = Eigen::Vector3f(-1.0f, 1.0f, 0.0f);
        bool outUpJson = false;
        float fov = -1;
        bool outFovJson = false;
        int width = -1;
        bool outWidthJson = false;
        int height = -1;
        bool outHeightJson = false;
        Eigen::Vector3f centre = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //centre of camera
        bool outCentreJson = false;
        Eigen::Vector3f ai = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //ambient light RGB
        bool outAiJson = false;
        Eigen::Vector3f bkc = Eigen::Vector3f(-1.0f, 0.0f, 0.0f); //background color RGB
        bool outBkcJson = false;
        
        bool globalillum; //use global illumination
        vector<float> raysperpixel; //Starts at one value, can be changed       
        float maxbounces; //max times ray can bounce
        float probterminate; //0-1, prob that ray terminates
        bool twosiderender; //rectangle property

        //none of the tests have this one, which is kinda weird ot me 
        bool antialiasing; //Determines use of antialiasing, if rays per pizel is specified then use this, other wise make own decision    

    };     

    //other variabel;s and methods

    vector<Geometry> geometries; //all geometries in scene
    vector<Light> lights; //all lights in scene
    vector<Output> outputsettings; //output settings
    float focallength; //focal length of camera
    float aspectratio; //aspect ratio of output image
    float imagewidth; //width of output image
    float imageheight; //height of output image
    float halfheight; //half height of output image
    float halfwidth; //half width of output image
    float viewportheight; //height of viewport
    Eigen::Vector3f viewportupperleft; //upper left corner of viewport
    Eigen::Vector3f forward; //horizontal vector for camera
    Eigen::Vector3f right; //vertical vector for camera
    Eigen::Vector3f up; //up vector for camera
    Eigen::Vector3f stepdown; //step down vector for camera
    Eigen::Vector3f stepright; //step right vector for camera
    Eigen::Vector3f imagecenter; //center of image plane
    
  
    void renderScene(); //renders the scene
    //added depth for reflectiosn but didnt work
    Eigen::Vector3f detectPixel(Ray& ray, const Output& outputsettings); //detects pixel
    Eigen::Vector3f phong(const Ray& ray, const Output& outputsettings, const Geometry& geometry, const Eigen::Vector3f& hitpoint, const Eigen::Vector3f& normal); //phong shading
    float getIntersection(const Ray& ray, const Geometry& geometry); 
    Ray getRay(float u, float v, const Output& outputsettings); //gets ray
};

class Ray {
public:
    Eigen::Vector3f origin;
    Eigen::Vector3f direction;

    Ray(const Eigen::Vector3f& orig, const Eigen::Vector3f& dir) : origin(orig), direction(dir.normalized()) {}

    Eigen::Vector3f at(float t) const {
        return origin + t * direction;
    }
    Eigen::Vector3f rayColor(const Ray& ray) const {
        //color of da ray
        Eigen::Vector3f unit_direction = ray.direction.normalized();
        float t = 0.5f * (unit_direction.y() + 1.0f);

        
        return (1.0f - t) * Eigen::Vector3f(1.0f, 1.0f, 1.0f) + t * Eigen::Vector3f(0.5f, 0.7f, 1.0f); 
    }
};