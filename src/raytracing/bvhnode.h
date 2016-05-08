#pragma once

#include <QList>
#include <scene/geometry/mesh.h>
#include <scene/geometry/boundingbox.h>
#include <raytracing/intersection.h>

class Intersection;

class BVHNode
{
public:
    BVHNode();
    BVHNode(QList<Geometry*> &to_contain, unsigned short int axis);

    BVHNode* greater = NULL;
    BVHNode* lesser = NULL;
    QList<Geometry*> contained;

    void sortByAxis(QList<Geometry*> &objs, unsigned short int axis,
                    int left, int right);

    void MakeBounds();
    bool isLeaf();
    bool intersectsBounds(Ray r);
    BoundingBox* bounds = NULL;

    ~BVHNode();
};

