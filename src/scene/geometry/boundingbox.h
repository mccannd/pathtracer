#pragma once

#include <openGL/drawable.h>

class BoundingBox : public Drawable
{
public:
    BoundingBox(float xmin, float xmax,
                float ymin, float ymax,
                float zmin, float zmax);
    void create();
    float x1, x2, y1, y2, z1, z2;
    glm::vec3 midpoint;
    glm::vec3 pMax;
    glm::vec3 pMin;
    void setBounds(float xmin, float xmax,
                   float ymin, float ymax,
                   float zmin, float zmax);
    void setBounds(glm::mat4 t);
    virtual GLenum drawMode();
    float surface_area;

    int MaximumExtent() const;

};


BoundingBox* mergeBoxes(BoundingBox *a, BoundingBox *b);
BoundingBox* mergeBoxes(BoundingBox *a, glm::vec3 &midpoint);
BoundingBox mergeBoxes(BoundingBox& a, glm::vec3 &midpoint);
BoundingBox mergeBoxes(BoundingBox& a, BoundingBox &b);

