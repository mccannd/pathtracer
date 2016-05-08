#pragma once
#include <la.h>
#include <raytracing/ray.h>
#include <raytracing/intersection.h>
#include <raytracing/intersectionengine.h>
#include <scene/scene.h>

class Scene;

//The Integrator class recursively evaluates the path a ray takes throughout a scene
//and computes the color a ray becomes as it bounces.
//It samples the materials, probability density functions, and BRDFs of the surfaces the ray hits
//to do this.

class Integrator
{
public:
    Integrator();
    Integrator(Scene *s);
    glm::vec3 TraceRay(Ray r, unsigned int depth);
   // glm::vec3 TraceRayBVH(Ray r, unsigned int depth);
    glm::vec3 DirectLightingIntegratorTraceRay(Ray &r, unsigned int depth, float &current_brdf_PDF, glm::vec3 &current_BxDFCol_Dot);
    glm::vec3 DirectLightingIntegratorTraceRayWithClouds(Ray &r, unsigned int depth, float &current_brdf_PDF, glm::vec3 &current_BxDFCol_Dot);
    glm::vec3 CloudVisibility(Intersection &isx, Intersection sample_isx, Ray r, float &opacity, QList<Geometry*> &skip);
    void SetDepth(unsigned int depth);

    Scene* scene;
    IntersectionEngine* intersection_engine;

protected:
    unsigned int max_depth;//Default value is 5.
};
