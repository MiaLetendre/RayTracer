#include "RayTracer.h"
#include <iostream>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "../external/json.hpp"
#include "../external/simpleppm.h"
#include <cmath>
#include <limits>

using namespace std;
using json = nlohmann::json;
using namespace Eigen;


//to do 


//get ray and other ray stuff is called early on and needs this
class Ray;


RayTracer::~RayTracer() {
    //Doenst do anything, at elast for right now, but good to have
}


//Takes all the values from the json and assigns them to the class variables
RayTracer::RayTracer(const json& j) {
    if (j.contains("geometry") && j["geometry"].is_array()){
        
    for (auto& element : j["geometry"]) {
        const string type = element["type"];
            // Handle output settings
            Geometry g;
            g.typejson = true; //one of the flags to make sure that all needed valies are there
            g.visible = true; //default to visible unless specified otherwise
            if (element.contains("comment")) {
                g.comment = element.value("comment", "");
            }
            g.type = type;
            if (type == "sphere") {
                if (element.contains("radius")){
                g.radius = element.value("radius", 1.0f);
                g.radjson = true;
                }
                if (element.contains("centre")){
                auto centre_vec = element["centre"];
                g.centre = Eigen::Vector3f(centre_vec[0], centre_vec[1], centre_vec[2]);
                g.centrejson = true;
                }
            } else if (type == "rectangle") {
                if (element.contains("p1") && element.contains("p2") && element.contains("p3") && element.contains("p4")) {
                    auto p1_vec = element["p1"];
                    auto p2_vec = element["p2"];
                    auto p3_vec = element["p3"];
                    auto p4_vec = element["p4"];
                    g.p1 = Eigen::Vector3f(p1_vec[0], p1_vec[1], p1_vec[2]);
                    g.p2 = Eigen::Vector3f(p2_vec[0], p2_vec[1], p2_vec[2]);
                    g.p3 = Eigen::Vector3f(p3_vec[0], p3_vec[1], p3_vec[2]);
                    g.p4 = Eigen::Vector3f(p4_vec[0], p4_vec[1], p4_vec[2]);

                    //this next part is so that when rendering, it doesnt have to redo this stuff wxh times
                    g.rectjson = true;
                    g.edgea = g.p2-g.p1;
                    g.edgeb = g.p4-g.p1;
                    g.plane = g.edgea.cross(g.edgeb).normalized(); //this is the normal
                }
                
            }
            if (element.contains("ac")) {
                auto ac_vec = element["ac"];
                g.ac = Eigen::Vector3f(ac_vec[0], ac_vec[1], ac_vec[2]);
                g.acjson = true;
            }
            if (element.contains("dc")) {
                auto dc_vec = element["dc"];
                g.dc = Eigen::Vector3f(dc_vec[0], dc_vec[1], dc_vec[2]);
                g.dcjson = true;
            }
            if (element.contains("sc")) {
                auto sc_vec = element["sc"];
                g.sc = Eigen::Vector3f(sc_vec[0], sc_vec[1], sc_vec[2]);
                g.scjson = true;
            }
            if (element.contains("ka")) {
                g.ka = element.value("ka", 0.0f);
                g.kajson = true;
            }
            if (element.contains("kd")) {
                g.kd = element.value("kd", 0.0f);
                g.kdjson = true;
            }
            if (element.contains("ks")) {
                g.ks = element.value("ks", 0.0f);
                g.ksjson = true;
            }
            if (element.contains("pc")) {
                g.pc = element.value("pc", 1.0f);
                g.pcjson = true;
            }
            if(element.contains("visible")) {
                g.visible = element.value("visible", true);
            }
            if (element.contains("transform")) {
                auto transform_array = element["transform"];
                Eigen::Matrix4f transform_matrix;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        transform_matrix(i, j) = transform_array[i * 4 + j];
                    }
                }
                g.transform = transform_matrix;
            } else{
                g.transform = Eigen::Matrix4f::Identity();
            }
            //flags to check if has everything needed
            if (g.typejson == false  || g.ksjson == false || g.kdjson == false  || g.kajson == false  || g.pcjson == false  || g.acjson == false  || g.dcjson == false  || g.scjson == false  ||
                (g.type == "sphere" && (g.radjson == false || g.centrejson == false )) ||
                (g.type == "rectangle" && (g.rectjson == false))) {
                cerr << "Error: Missing required geometry parameters." << endl;
                continue; // Skip this geometry
            }
            


            geometries.push_back(g);
    }
}
        if (j.contains("light") && j["light"].is_array()){
            
        for (auto& element : j["light"]) {
        const string type = element["type"];
            //Let there be Light!
            Light L;
            L.typejsonlight = true;
            
            L.type = element["type"];
            if (element.contains("comment")) {
                L.comment = element.value("comment", "");
            }
            if (type == "area") {
                if (element.contains("p1") && element.contains("p2") && element.contains("p3") && element.contains("p4")) {
                    auto p1_vec = element["p1"];
                    auto p2_vec = element["p2"];
                    auto p3_vec = element["p3"];
                    auto p4_vec = element["p4"];
                    L.p1 = Eigen::Vector3f(p1_vec[0], p1_vec[1], p1_vec[2]);
                    L.p2 = Eigen::Vector3f(p2_vec[0], p2_vec[1], p2_vec[2]);
                    L.p3 = Eigen::Vector3f(p3_vec[0], p3_vec[1], p3_vec[2]);
                    L.p4 = Eigen::Vector3f(p4_vec[0], p4_vec[1], p4_vec[2]);
                    L.pointsjson = true;
                }
                
                if (element.contains("n")) {
                L.n = element.value("n", 1);
                }
                if (element.contains("usecenter")) {
                L.usecenter = element.value("usecenter", false);
                }
            } else if (type == "point") {
                if (element.contains("centre")){
                auto centre_vec = element["centre"];
                L.centre = Eigen::Vector3f(centre_vec[0], centre_vec[1], centre_vec[2]);
                L.centrejsonlight = true;
                }
            }
            if (element.contains("id")) {
                auto id_vec = element["id"];
                L.id = Eigen::Vector3f(id_vec[0], id_vec[1], id_vec[2]);
                L.idjson = true;
            }
            if (element.contains("is")) {
                auto is_vec = element["is"];
                L.is = Eigen::Vector3f(is_vec[0], is_vec[1], is_vec[2]);
                L.isjson = true;
            }
            if(element.contains("use")) {
                L.use = element.value("use", true);
            }
            if (element.contains("transform")) {
                auto transform_array = element["transform"];
                Eigen::Matrix4f transform_matrix;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        transform_matrix(i, j) = transform_array[i * 4 + j];
                    }
                }
                L.transform = transform_matrix;
            }else{
                L.transform = Eigen::Matrix4f::Identity();
            }

            //flags to check if has everything needed
            if (L.use == false || L.typejsonlight == false || L.idjson == false  || L.isjson == false  ||
                (L.type == "area" && (L.pointsjson == false )) ||
                (L.type == "point" && (L.centrejsonlight == false ))) {
                cout << L.use << L.typejsonlight << L.idjson << L.isjson << L.pointsjson << L.centrejsonlight;
                cerr << "Error: Missing required light parameters." << endl;
                continue; // Skip this light
            }
            //test to make sure json successfully parsed
            //cout << "Light type: " << L.type  << "Light id: " << L.id  << "Light is: " << L.is << endl;
            lights.push_back(L);
        }
    }

    if (j.contains("output") && j["output"].is_array()){
        auto& output_array = j["output"];
        for(auto& element : output_array) {
            // Handle output settings
            Output out;
            
            if (element.contains("filename")) {
                out.filename = element.value("filename", "output.ppm");
                out.outFileJson = true;
            }
            if (element.contains("size")) {
                auto size_vec = element["size"];
                out.size = vector<int>{size_vec[0], size_vec[1]};
                out.outSizeJson = true;
            }
            if (element.contains("lookat")) {
                auto lookat_vec = element["lookat"];
                out.lookat = Eigen::Vector3f(lookat_vec[0], lookat_vec[1], lookat_vec[2]);
                out.outLookJson = true;
            }

            if (element.contains("up")) {
                auto up_vec = element["up"];
                out.up = Eigen::Vector3f(up_vec[0], up_vec[1], up_vec[2]);
                out.outUpJson = true;
            }
            if (element.contains("fov")) {
                out.fov = element.value("fov", 45.0f);
                out.outFovJson = true; 
            }
            if (element.contains("size")) {
                out.size = vector<int>{element["size"][0], element["size"][1]};
                out.outSizeJson = true;

            }
           
            if (element.contains("centre")) {
                auto centre_vec = element["centre"];
                out.centre = Eigen::Vector3f(centre_vec[0], centre_vec[1], centre_vec[2]);
                out.outCentreJson = true;
            }
            if (element.contains("ai")){
                auto ai_vec = element["ai"];
                out.ai = Eigen::Vector3f(ai_vec[0], ai_vec[1], ai_vec[2]);
                out.outAiJson = true;
            }
            if(element.contains("bkc")){
                auto bkc_vec = element["bkc"];
                out.bkc = Eigen::Vector3f(bkc_vec[0], bkc_vec[1], bkc_vec[2]);
                out.outBkcJson = true;
            }
            if (element.contains("globalillum")){
                out.globalillum = element.value("globalillum", false);
            }
            if (element.contains("raysperpixel")) {
                auto raysperpixel_vec = element["raysperpixel"];
                out.raysperpixel = vector<float>();
                for (auto& val : raysperpixel_vec) {
                    out.raysperpixel.push_back(val);
                }
            }
            if (element.contains("maxbounces")) {
                out.maxbounces = element.value("maxbounces", 5.0f);
            }
            if (element.contains("probterminate")) {
                out.probterminate = element.value("probterminate", 0.1f);
            }
            if (element.contains("antialiasing")) {
                out.antialiasing = element.value("antialiasing", false);
            }
            if (element.contains("twosiderender")) {
                out.twosiderender = element.value("twosiderender", false);
            }

            //flags to check if has everything needed
            //flags are messing things, should message self if soemthing is off, but should have a default 
            //show the one it fails
            if(out.outFileJson == false || out.outSizeJson == false || out.outLookJson == false || 
                out.outUpJson == false || out.outFovJson == false || out.outSizeJson == false || 
                out.outCentreJson == false || out.outAiJson == false || out.outBkcJson == false) {
                cerr << "Error: Missing required output parameters." << endl;
                if (out.outFileJson == false) {
                    cerr << "Missing filename" << endl;
                }
                if (out.outSizeJson == false) {
                    cerr << "Missing size" << endl;
                }
                if (out.outLookJson == false) {
                    cerr << "Missing lookat" << endl;
                }
                if (out.outUpJson == false) {
                    cerr << "Missing up" << endl;
                }
                if (out.outFovJson == false) {
                    cerr << "Missing fov" << endl;
                }
                if (out.outSizeJson == false) {
                    cerr << "Missing size" << endl;
                }
                
                if (out.outCentreJson == false) {
                    cerr << "Missing centre" << endl;
                }
                if (out.outAiJson == false) {
                    cerr << "Missing ai" << endl;
                }
                if (out.outBkcJson == false) {
                    cerr << "Missing bkc" << endl;
                }

                continue; 
            }
            outputsettings.push_back(out);
        }
    }
    
}




void RayTracer::run() {
    // Run the ray tracing process
    // Prolly should just do the rendering here but I prefer to have it in a seperate method
    renderScene();

}

//go thorugh every pixel and run detect pixel 
void RayTracer::renderScene() {
    // Render the scene
    for (const auto& out : outputsettings) {
    //Getting the needed values to run 
    focallength = 1.0f; 
    aspectratio = static_cast<float>(out.size[0]) / out.size[1];
    imagewidth = out.size[0];
    imageheight = out.size[1];
    halfheight = tanf((out.fov * M_PI / 180.0f) / 2.0f); //done in radians
    halfwidth = aspectratio * halfheight;
    //cant use aspect ratio here becuause aspect is ideal and this is actual
    
    forward = out.lookat.normalized();
    right = forward.cross(out.up).normalized();
    up = right.cross(forward).normalized();
    

    //top left pixel
    imagecenter = out.centre + focallength * forward;
    viewportupperleft = imagecenter + (up *halfheight) - (right * halfwidth);

    //steps, down needs to be negative, because... its down
    stepdown = (-2.0f * halfheight / imageheight) * up;
    stepright = (2.0f * halfwidth / imagewidth) * right;

    vector<double> buffer(static_cast<size_t>(imagewidth) * imageheight * 3, 0.0f); 
    cout << "Rendering started" << endl;
    
    int samples = 1;
    //for antialiasing, need to do the following
    //need to add anb inner loop for each pixel
    //add a random offset to each sample, generate ray, average colors
    for (int j = 0; j < imageheight; j++){
        for (int i = 0 ; i < imagewidth; i++){  
            if (out.antialiasing && out.raysperpixel.size() > 0) {
                samples = 5;
            } 
            Eigen::Vector3f color(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < samples; s++) {
                float u;
                float v;
                    if (samples > 1){
                        u = i + static_cast<float>(rand()) / RAND_MAX; 
                        v = j + static_cast<float>(rand()) / RAND_MAX; 
                    }else{
                        u = i + 0.5f;
                        v = j + 0.5f;
                    }
                    Ray ray = getRay(u, v, out);
                    color += detectPixel(ray, out);
                }
            //for antialiasing, can add random offset to i and j here based on rays per pixel
            //clamp color values to 0 to 1
            if (samples > 1) {
                color /= static_cast<float>(samples);
            }
            color = color.cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f)).cwiseMax(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
            //ok so weird bug here didnt break anything and I dont know why
            //had a variable called "index" from code that was deleted, and this code still worked
            //not sure why it worked, or why nothing happened when I fixed it, but I guess whatever
            buffer[(j * imagewidth + i)*3 + 0] = color.x();
            buffer[(j * imagewidth + i)*3 + 1] = color.y();
            buffer[(j * imagewidth + i)*3 + 2] = color.z();
        }
    }
    save_ppm(out.filename, buffer, imagewidth, imageheight);

    cout << "Rendering completed! Image has been saved to " << out.filename << endl;
    }
}


Eigen::Vector3f RayTracer::detectPixel(Ray& ray, const Output& outputsettings) {
    // Detect pixel color based on ray intersection with scene geometry
    //no liogh calcs yet
    bool hit = false;
    Geometry hitGeometry; 
    Eigen::Vector3f Normal;
    Ray shadowRay(Eigen::Vector3f(0,0,0), Eigen::Vector3f(0,0,0)); 
    Ray shadowRayLoop(Eigen::Vector3f(0,0,0), Eigen::Vector3f(0,0,0));
    bool shadow = false;
    Eigen::Vector3f hitColor = outputsettings.bkc; //default to background color
    float closestT = numeric_limits<float>::max();
    for (const auto& geometry : geometries) {
        if (!geometry.visible) {
            continue;
        } 
        float t = getIntersection(ray, geometry);
        if (t > 0.00001f && t < closestT) { 
            closestT = t;
            hit = true;
            hitColor = geometry.dc;
            hitGeometry = geometry;
            if (geometry.type == "sphere") {
                Normal = (ray.at(t) - geometry.centre).normalized();
            } else if (geometry.type == "rectangle") {
                Normal = geometry.plane.normalized(); 
            }

        }
    }
    if (hit){
        
        Eigen::Vector3f finalColor = phong(ray, outputsettings, hitGeometry, ray.at(closestT), Normal); 
        
        return finalColor;
    } else {
        //return background color
        return outputsettings.bkc; 
    }


}
    

Eigen::Vector3f RayTracer::phong(const Ray& ray, const Output& outputsettings, const Geometry& geometry, const Eigen::Vector3f& hitpoint, const Eigen::Vector3f& normal){
    //formula for phong shading is I = Ia*ka + Id*kd*(L*N) + Is*ks*(R*V)^pc
    //Ia is ambient light, ka is ambient reflection coefficient, Id is diffuse light, 
    //kd is diffuse reflection coefficient, L is light direction, N is normal, Is is specular light
    // ks is specular reflection coefficient, R is reflection direction, V is view direction, pc is phong coefficient

    //from hit point to camera
    Eigen::Vector3f viewdir = (ray.origin - hitpoint).normalized();
    //Eigen::Vector3f viewdir = (outputsettings.centre - hitpoint).normalized(); 
    //from the hit point to the light
    vector<Eigen::Vector3f> lightDirs;
    vector<Eigen::Vector3f> areaLightDirs;

    //only need once so can do outside loop
    Eigen::Vector3f amb = outputsettings.ai.cwiseProduct(geometry.ac) * geometry.ka; 
    Eigen::Vector3f illumination = amb;

    for (const auto& light : lights) {
        //first get light directions: 
        if (!light.use) {
            continue;
        }
        Eigen::Vector3f lighttemp;
            if (light.type == "point") {
                lighttemp = (light.centre - hitpoint).normalized();
            } else if (light.type == "area") {
                //for area later
                //sampling the light 


                int samples = 64;
                Eigen::Vector3f areaIll(0.0f, 0.0f, 0.0f);
                for (int i = 0; i< 8; i++){
                    for (int j = 0; j < 8; j++){
                        //gets random points on area
                        float u = (i + static_cast<float>(rand()) / RAND_MAX) / 8.0f;
                        float v = (j + static_cast<float>(rand()) / RAND_MAX) / 8.0f;
                        Eigen::Vector3f point = light.p1 + u * (light.p2 - light.p1) + v * (light.p4 - light.p1);
                        
                        Eigen::Vector3f lightdirLoop = (point - hitpoint).normalized();

                        //chekcs in shadow
                        Ray shadowRayLoop(hitpoint + normal * 0.001f, (point - hitpoint).normalized());
                        bool inShadow = false;
                        for (const auto& geometry : geometries) {
                            if (!geometry.visible) {
                                continue;
                            }
                            float tempLoop = getIntersection(shadowRayLoop, geometry);
                            if (tempLoop > 0.00001f && tempLoop < (point - hitpoint).norm()) {
                                inShadow = true;
                                break; 
                            }
                        }
                        //if not in shado, nbeeds to calculate diffuse and specular 
                        if (!inShadow) {

                            float dot = lightdirLoop.dot(normal);
                            if (dot > 0) {
                                //diffuse
                                areaIll += geometry.kd * light.id.cwiseProduct(geometry.dc) * dot;
                                
                                //specular
                                Eigen::Vector3f reflectdir = (2 * dot * normal - lightdirLoop.normalized()).normalized();
                                
                                float specdot = reflectdir.dot(viewdir);
                                if (specdot > 0) {
                                    areaIll += geometry.ks * light.is.cwiseProduct(geometry.sc) * pow(specdot, geometry.pc);
                                }
                            } 
                        }
                    }
                }
                //average area light
                illumination += areaIll / static_cast<float>(samples);
                //the next part is really only for point
                continue;  
            }
        

            bool shadow = false;
            //offset the ray
            Ray shadowRay(hitpoint + normal * 0.001f, lighttemp); 
           for(const auto& geometry : geometries) {
            if (!geometry.visible) {
                continue;
            }
            float temp = getIntersection(shadowRay, geometry);
            if (temp > 0.00001f && temp < (light.centre - hitpoint).norm()) {
                shadow = true;
                break;
            }
           }

            if (shadow) {
                //skip diffuse and spec if in shadow
                continue; 
            }
            //only add to ill if light matters
            Eigen::Vector3f lightdir = lighttemp.normalized();

            if (lightdir.dot(normal) < 0){
                continue;
            }
            //next diffuse and dot
            Eigen::Vector3f diff = max(0.0f, lightdir.dot(normal)) * geometry.kd * geometry.dc.cwiseProduct(light.id);
            illumination += diff;

            //next get specular component and reflection directiosn

            float dot = lightdir.dot(normal);
            //specular component
            Eigen::Vector3f reflectdir = (2 * dot * normal - lightdir).normalized();
            float specdot = reflectdir.dot(viewdir);
            if (specdot > 0) {
                Eigen::Vector3f spec = geometry.ks * light.is.cwiseProduct(geometry.sc) * pow(specdot, geometry.pc);
                illumination += spec;
            }
        
    }

    //this is going ot be Ambient + the sum of all lights diffuse + specular
    

    return illumination; 
}


//originally just for detect pixel, needed these calculations for shadow
float RayTracer::getIntersection(const Ray& ray, const Geometry& geometry){
    if (geometry.type == "sphere"){
            //Phere center to ray origin vector
            Eigen::Vector3f spherectorayo = ray.origin - geometry.centre; 
        
            //a*t^2 + b*t + c = 0
            float a = ray.direction.dot(ray.direction); //should be 1
            float b = 2.0f * spherectorayo.dot(ray.direction);
            float c = spherectorayo.dot(spherectorayo) - geometry.radius * geometry.radius;
            //does the quadratic formula to find if there is a solution
            if ((b * b - 4 * a * c) < 0) {
                return -1.0f;
            }
            float t1 = (-b - sqrt(b * b - 4 * a * c)) / (2.0f*a);
            float t2 = (-b + sqrt(b * b - 4 * a * c)) / (2.0f*a);
            float t = t1;
            if (t < 0.00001f){
                t = t2; // If t1 is negative, try t2
            }
            if (t < 0.00001f) {
                return -1.0f; // Both t1 and t2 are negative, no valid intersection
            }
            return t;
        } else if (geometry.type == "rectangle"){
            //using P = O + t * D
            // also plane: (P-c)*N = 0
            //substituting become t = ((c-o) *n)/D*N

            
            float denominator = ray.direction.dot(geometry.plane);
            //makes stuff invisible if ray and normal face each other
            if (denominator > 0){
                return -1.0f;
            }
            if (abs(denominator) < 0.00001f){
                return -1.0f;
            }
            
            //t
            float t = (geometry.p1-ray.origin).dot(geometry.plane)/denominator;
            
            //is the point inside or out 
            Eigen::Vector3f hitpoint = ray.at(t);
            //assumes that origin of rect is p1
            //local is the vector pointing from corner to hit
            Eigen::Vector3f local = hitpoint - geometry.p1;
            float dota = local.dot(geometry.edgea);
            float dotb = local.dot(geometry.edgeb);

            if (t < 0.00001f){
                return -1.0f;
            }

            if (dota >= 0 && dota <= geometry.edgea.dot(geometry.edgea) && dotb >= 0 && dotb <= geometry.edgeb.dot(geometry.edgeb)) {
                return t;
            }
        }
    return -1.0f;                    
            
}

Ray RayTracer::getRay(float u, float v, const Output& outputsettings) {
    Eigen::Vector3f pixelposition = viewportupperleft + u * stepright + v * stepdown; 
    Eigen::Vector3f direction = (pixelposition - outputsettings.centre).normalized();
    return Ray(outputsettings.centre, direction);
}

