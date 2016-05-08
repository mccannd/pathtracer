#pragma once
#include <QList>
#include <raytracing/ray.h>
#include <scene/scene.h>
#include <raytracing/bvhnode.h>

class Scene;
class Ray;


class IntersectionEngine
{
public:
    IntersectionEngine();
    Scene *scene;

    Intersection GetIntersection(Ray r);
    Intersection GetIntersectionSkipThis(Ray r, QList<Geometry*> skip);
    QList<Intersection> GetAllIntersections(Ray r);
    BVHNode *root = NULL;
    Intersection IntersectionByBounds(Ray r, BVHNode *n);
    bool IntersectBounds(Ray r, BVHNode* n);
};
