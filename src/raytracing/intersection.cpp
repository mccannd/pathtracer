#include <raytracing/intersection.h>

Intersection::Intersection():
    point(glm::vec3(0)),
    normal(glm::vec3(0)),
    tangent(glm::vec3(0)),
    bitangent(glm::vec3(0)),
    t(-1),
    texture_color(glm::vec3(1.0f))
{
    object_hit = NULL;
}

