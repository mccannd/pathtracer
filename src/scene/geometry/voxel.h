#pragma once
#include <scene/geometry/geometry.h>
#include <scene/geometry/perlin.h>
#include <scene/materials/material.h>
#include <raytracing/intersection.h>
#include <scene/geometry/voxel.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>
#include <scene/transform.h>
#include <math.h>

class Intersection;
class Geometry;

class Voxel : public Drawable
{
public:
    Voxel();
    Voxel(glm::vec3 mini, glm::vec3 maxi);

//    Intersection GetIntersection(Ray r);

    QList<float> densities;

    glm::vec3 min; // minimum boundary
    glm::vec3 max; // maximim boundary
    int n; // number of voxels in x, y, z directions


    // Functions
    int pointToIndex(float x, float y, float z);
    float getDensity(float x, float y, float z);
    float getDensity(glm::mat4 invT, glm::vec3 p);

//    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point);
    virtual glm::vec3 ComputeNormal(const glm::vec3 &P);
//    virtual void ComputeArea();
//    virtual Intersection GetSamplePoint(const glm::vec3 &isx_normal);
    virtual glm::vec4 GetCubeNormal(const glm::vec4& P);

    void create();
    virtual GLenum drawMode();
};
