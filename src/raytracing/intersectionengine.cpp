#include "intersectionengine.h"

IntersectionEngine::IntersectionEngine()
{
    scene = NULL;
}

Intersection IntersectionEngine::GetIntersectionSkipThis(Ray r, QList<Geometry*> skip)
{
    Intersection nearest;
    for(Geometry* g : scene->objects)
    {
//        if (!skip.contains(g)) {
            Intersection isx = g->GetIntersection(r);
            if((isx.t < nearest.t && isx.object_hit != NULL) || nearest.t < 0)
            {
                nearest = isx;
            }
//        }
    }
    return nearest;
}

// depth first intersection test using bounding volume hierarchy
Intersection IntersectionEngine::IntersectionByBounds(Ray r, BVHNode *n)
{
    if (n == NULL) return Intersection();
    if (n->bounds == NULL) n->MakeBounds();

    if (n->isLeaf()) {
        // return the intersection of just this object in the leaf
        Geometry *g = n->contained[0];

        //check if it's a triangle, if so, need to transform!
        Triangle* tri = dynamic_cast<Triangle*>(g);
        Intersection i;
        Ray tr;
        if(tri != NULL){
            tr = r.GetTransformedCopy(g->transform.invT());
            i = g->GetIntersection(tr);
        } else {
            i = g->GetIntersection(r);
        }

        if (i.object_hit != NULL) {
            if(tri != NULL) {
                glm::vec4 P = glm::vec4(i.t * tr.direction + tr.origin, 1);
                i.point = glm::vec3(tri->transform.T() * P);
                i.normal = glm::normalize(glm::vec3(tri->transform.invTransT() * glm::vec4(i.normal,0)));
                i.t = glm::distance(i.point, r.origin);//The t used for the closest triangle test was in object space

                i.tangent = glm::normalize(glm::vec3(tri->transform.T() * glm::vec4(tri->tri_tangent,0)));
                i.bitangent = glm::normalize(glm::vec3(tri->transform.T() * glm::vec4(tri->tri_bitangent,0)));
            }
            return i;

        } else return Intersection();
    } else {
        // find the closest intersection from all children
        Intersection i1 = Intersection();
        Intersection i2 = Intersection();

        // see if this ray intersects either child,
        // then recurse on it if it does
        if (n->lesser->bounds == NULL) n->lesser->MakeBounds();

        if (n->greater->bounds == NULL) n->greater->MakeBounds();

        if (n->lesser->intersectsBounds(r)) {
            i1 = IntersectionByBounds(r, n->lesser);
        }

        if (n->greater->intersectsBounds(r)) {
            i2 = IntersectionByBounds(r, n->greater);
        }



        // if only one child intersects, return it, if neither, return default
        if (i1.object_hit != NULL && i2.object_hit == NULL) return i1;
        else if (i1.object_hit == NULL && i2.object_hit != NULL) return i2;
        else if (i1.object_hit == NULL && i2.object_hit == NULL) return Intersection();
        else {
            // two intersections: return the closest
            if (i1.t < i2.t) return i1;
            else return i2;
        }
    }
}

Intersection IntersectionEngine::GetIntersection(Ray r)
{
    Intersection nearest;
    for(Geometry* g : scene->objects)
    {
        Intersection isx = g->GetIntersection(r);
        if((isx.t < nearest.t && isx.object_hit != NULL) || nearest.t < 0)
        {
            nearest = isx;
        }
    }
    return nearest;
}

bool IntersectionComp(const Intersection &lhs, const Intersection &rhs)
{
    return lhs.t < rhs.t;
}

QList<Intersection> IntersectionEngine::GetAllIntersections(Ray r)
{
    QList<Intersection> result;
    for(Geometry* g : scene->objects)
    {
        Intersection isx = g->GetIntersection(r);
        if(isx.t > 0)
        {
            result.append(isx);
        }
    }
    std::sort(result.begin(), result.end(), IntersectionComp);
    return result;
}
