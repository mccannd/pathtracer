#include <scene/geometry/geometry.h>


//Returns the solid-angle weighted probability density function given a point we're trying to illuminate and
// isx is the light point
// ray = wj from the geometry to the light
float Geometry::RayPDF(const Intersection &isx, const Ray &ray)
{
    //The isx passed in was tested ONLY against us (no other scene objects), so we test if NULL
    //rather than if != this.
    if(isx.object_hit == NULL)
    {
        return 0;
    }

    // Angle between two vectors
    // glm::dot(a, b) = ||A||*||B||*cos(theta) , A & B are unit vectors
    glm::vec3 light_to_geo = glm::normalize(ray.origin - isx.point);
    float cos_theta = glm::dot(light_to_geo, isx.normal);

    float r2 = glm::distance2(isx.point, ray.origin);
    return r2 / (cos_theta * isx.object_hit->area);
}
