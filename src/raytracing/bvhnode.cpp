#pragma once
#include "bvhnode.h"

BVHNode::BVHNode()
{

}

/*Chloe's code, crashing because of null make bounds in my_gl? Run debugger to check
BVHNode::BVHNode(QList<Geometry *> &to_contain, unsigned short axis)
{
    contained = to_contain;
    //BVHNode* node = new BVHNode();
    BoundingBox bbox = BoundingBox(0,0,0,0,0,0);
    for(int i = 0; i < to_contain.size(); i++) {
        bbox = mergeBoxes(bbox, *(to_contain[i]->bounds));
    }
    if(to_contain.size() == 1) {
        //check if it is a mesh, if it is, recurse (CHECK LATER)
        Mesh* m = dynamic_cast<Mesh*>(to_contain[0]);
        // each mesh must contain its own tree based on triangles,
        // so it is not a leaf node
        if (m != NULL) {

        } else {
            //create leaf
            this->contained = to_contain;
            this->bounds = &bbox;
        }

    } else {
        BoundingBox centroidBounds = BoundingBox(0,0,0,0,0,0);
        for(int i = 0; i < to_contain.size(); i++) {
            centroidBounds = mergeBoxes(centroidBounds, to_contain[i]->bounds->midpoint);
        }
        int dim = centroidBounds.MaximumExtent();

        if(centroidBounds.pMax[dim] == centroidBounds.pMin[dim]) {

            //check if it is a mesh, if it is, recurse (CHECK LATER)
            Mesh* m = dynamic_cast<Mesh*>(to_contain[0]);
            // each mesh must contain its own tree based on triangles,
            // so it is not a leaf node
            if (m != NULL) {
                this->contained = QList<Geometry*>();
                this->bounds = &bbox;

                // make a new list out of triangles
                for (int i = 0; i < m->faces.size(); i++) {
                    contained.append((Geometry*) m->faces[i]);
                }
                // continue as normal
                sortByAxis(contained, axis, 0, contained.size() - 1); // sort the list

                QList<Geometry*> l = QList<Geometry*>();
                for (unsigned int i = 0; i < contained.size() / 2; i++) {
                    l.append(contained[i]);
                }
                QList<Geometry*> r = QList<Geometry*>();
                for (unsigned int i = contained.size() / 2;
                     i < contained.size(); i++) {
                    r.append(contained[i]);
                }

                lesser = new BVHNode(l, (axis + 1) % 3);
                greater = new BVHNode(r, (axis + 1) % 3);
            } else {
                //create leaf
                this->contained = to_contain;
                this->bounds = &bbox;
            }
        }
        //partition
        if(to_contain.size() <= 4) {
            //use SPLIT_EQUAL_COUNTS method, partition geometries into equally-sized subsets
            sortByAxis(contained, axis, 0, contained.size() - 1);

            QList<Geometry*> l = QList<Geometry*>();
            for (unsigned int i = 0; i < contained.size() / 2; i++) {
                l.append(contained[i]);
            }
            QList<Geometry*> r = QList<Geometry*>();
            for (unsigned int i = contained.size() / 2;
                 i < contained.size(); i++) {
                r.append(contained[i]);
            }

            lesser = new BVHNode(l, (axis + 1) % 3);
            greater = new BVHNode(r, (axis + 1) % 3);
        } else {
            //use surface area heuristic
            const int nBuckets = 12;
            struct BucketInfo {
                BucketInfo() {count = 0; }
                int count;
                BoundingBox* bounds;
            };
            BucketInfo buckets[nBuckets];
            //for each geometry, determine the bucket that its centroid lies in and update the buckets bounds to include the geoemtries bounds
            //Initialize BucketInfo for SAH partition buckets
            for(int i = 0; i < to_contain.size(); i++){
                BoundingBox curr = *(to_contain[i]->bounds);
                glm::vec3 P1 = glm::vec3(curr.x1, curr.y1, curr.z1);
                glm::vec3 P2 = glm::vec3(curr.x2, curr.y2, curr.z2);
                int b = nBuckets *
                        ((curr.midpoint[dim] - centroidBounds.pMin[dim]) /
                        (centroidBounds.pMax[dim] - centroidBounds.pMin[dim]));
                if(b == nBuckets) b = nBuckets - 1;
                buckets[b].count++;
                BoundingBox bb = mergeBoxes(*(buckets[b].bounds), curr);
                buckets[b].bounds = &bb;
            }

            //compute cost for splitting after each bucket
            float cost[nBuckets-1];
            for(int i = 0; i < nBuckets-1; i++) {
                BoundingBox b0 = BoundingBox(0,0,0,0,0,0);
                BoundingBox b1 = BoundingBox(0,0,0,0,0,0);
                int count0 = 0, count1 = 0;
                for(int j = 0; j <=i; j++) {
                    b0 = mergeBoxes(b0, *(buckets[j].bounds));
                    count0 += buckets[j].count;
                }
                for(int j = i+1; j < nBuckets; j++) {
                    b1 = mergeBoxes(b1, *(buckets[j].bounds));
                    count1 += buckets[j].count;
                }
                cost[i] = .125 + (count0*b0.surface_area + count1*b1.surface_area) / bbox.surface_area;
            }

            //Find bucket to split that minimizes SAH metric
            float minCost = cost[0];
            int minCostSplit = 0;
            for(int i = 1; i < nBuckets-1; i++) {
                if(cost[i] <minCost) {
                    minCost = cost[i];
                    minCostSplit = i;
                }
            }

            //either create leaf or split geometries at selected SAH bucket
            if(to_contain.size() > 1 || minCost < to_contain.size()) {
                this->contained = to_contain;
                this->bounds = &bbox;

                //partition
                QList<Geometry*> l = QList<Geometry*>();
                for (unsigned int i = 0; i < minCostSplit; i++) {
                    l.append(contained[i]);
                }
                QList<Geometry*> r = QList<Geometry*>();
                for (unsigned int i = minCostSplit;
                     i < contained.size(); i++) {
                    r.append(contained[i]);
                }

                lesser = new BVHNode(l, (axis + 1) % 3);
                greater = new BVHNode(r, (axis + 1) % 3);
            } else {
                //check if it is a mesh, if it is, recurse (CHECK LATER)
                Mesh* m = dynamic_cast<Mesh*>(to_contain[0]);
                // each mesh must contain its own tree based on triangles,
                // so it is not a leaf node
                if (m != NULL) {
                    this->contained = QList<Geometry*>();
                    this->bounds = &bbox;

                    // make a new list out of triangles
                    for (int i = 0; i < m->faces.size(); i++) {
                        contained.append((Geometry*) m->faces[i]);
                    }
                    // continue as normal
                    sortByAxis(contained, axis, 0, contained.size() - 1); // sort the list

                    QList<Geometry*> l = QList<Geometry*>();
                    for (unsigned int i = 0; i < contained.size() / 2; i++) {
                        l.append(contained[i]);
                    }
                    QList<Geometry*> r = QList<Geometry*>();
                    for (unsigned int i = contained.size() / 2;
                         i < contained.size(); i++) {
                        r.append(contained[i]);
                    }

                    lesser = new BVHNode(l, (axis + 1) % 3);
                    greater = new BVHNode(r, (axis + 1) % 3);
                } else {
                    //create leaf
                    this->contained = to_contain;
                    this->bounds = &bbox;
                }
            }
        }
    }
}*/

/*
 * Dan's code
 * BVHNode::BVHNode(QList<Geometry *> &to_contain, unsigned short int axis)
{
    contained = to_contain;

    //std::cout<<"Creating a new BVHNode\n";

    if (contained.size() > 1) {
        // this node will contains multiple pieces of geometry
        // find the median of the QList based on the axis
        // sort objects and divide evenly
        sortByAxis(contained, axis, 0, contained.size() - 1); // sort the list


        //sum surface area of all geometry
        float totalSA = 0;
        for (Geometry* g : contained) {
            if (g->area < 0) g->ComputeArea();
            totalSA += g->area;
        }

        QList<Geometry*> l = QList<Geometry*>();
        QList<Geometry*> r = QList<Geometry*>();
        float bestProportion = 1; // for cases where surface area split isnt very even

        // attempt every partition of this total list
        for (int i = 0; i < contained.size(); i++) {
            QList<Geometry*> ltemp = QList<Geometry*>();
            QList<Geometry*> rtemp = QList<Geometry*>();

            // partition the list
            for (int jl = 0; jl < i; jl++) { ltemp.push_back(contained[jl]); }
            for (int jr = i; jr < contained.size(); jr++) { rtemp.push_back(contained[jr]); }

            // pick the smaller of the two lists
            QList<Geometry*> smaller = ltemp;
            if (rtemp.size() < ltemp.size()) smaller = rtemp;

            // find the proportion of the smaller list to the bigger list
            float partitionSA = 0;
            for (int ind = 0; ind < smaller.size(); ind++) partitionSA += smaller[ind]->area;

            // if the proportion is close enough to one half, break the iteration, good enough
            float proportion = partitionSA / totalSA;
            if (proportion > 0.47 && proportion < 0.53) {
                l = ltemp; // make the temp lists permanent
                l.clear();
                for (int i = 0; i < ltemp.size(); i++) l.append(ltemp[i]);
                r = rtemp;
                r.clear();
                for (int i = 0; i < rtemp.size(); i++) r.append(rtemp[i]);
                break;
            } else {
                // otherwise keep track of the best partition so it can be used afterward
                float ap = fabs(proportion - 0.5);
                if (ap < bestProportion) {
                    bestProportion = ap;
                    l = ltemp;
                    l.clear();
                    for (int i = 0; i < ltemp.size(); i++) l.append(ltemp[i]);
                    r = rtemp;
                    r.clear();
                    for (int i = 0; i < rtemp.size(); i++) r.append(rtemp[i]);
                }
            }
        }

//        for (unsigned int i = 0; i < contained.size() / 2; i++) {
//            l.append(contained[i]);
//        }

//        for (unsigned int i = contained.size() / 2;
//             i < contained.size(); i++) {
//            r.append(contained[i]);
//        }

        lesser = new BVHNode(l, (axis + 1) % 3);
        greater = new BVHNode(r, (axis + 1) % 3);

    } else {
        Mesh* m = dynamic_cast<Mesh*>(to_contain[0]);
        // each mesh must contain its own tree based on triangles,
        // so it is not a leaf node
        if (m != NULL) {
            contained = QList<Geometry*>();

            // make a new list out of triangles
            for (int i = 0; i < m->faces.size(); i++) {
                contained.append((Geometry*) m->faces[i]);
            }

            // continue as normal
            sortByAxis(contained, axis, 0, contained.size() - 1); // sort the list

            QList<Geometry*> l = QList<Geometry*>();
            for (unsigned int i = 0; i < contained.size() / 2; i++) {
                l.append(contained[i]);
            }
            QList<Geometry*> r = QList<Geometry*>();
            for (unsigned int i = contained.size() / 2;
                 i < contained.size(); i++) {
                r.append(contained[i]);
            }

            lesser = new BVHNode(l, (axis + 1) % 3);
            greater = new BVHNode(r, (axis + 1) % 3);
        }
    }
}
*/

// recursive constructor will create an entire tree of nodes
// this means that this should be created from the top down
BVHNode::BVHNode(QList<Geometry *> &to_contain, unsigned short int axis)
{
    contained = to_contain;

    //std::cout<<"Creating a new BVHNode\n";

    if (contained.size() > 1) {
        // this node will contains multiple pieces of geometry
        // find the median of the QList based on the axis
        // sort objects and divide evenly
        sortByAxis(contained, axis, 0, contained.size() - 1); // sort the list

        QList<Geometry*> l = QList<Geometry*>();
        for (unsigned int i = 0; i < contained.size() / 2; i++) {
            l.append(contained[i]);
        }
        QList<Geometry*> r = QList<Geometry*>();
        for (unsigned int i = contained.size() / 2;
             i < contained.size(); i++) {
            r.append(contained[i]);
        }

        lesser = new BVHNode(l, (axis + 1) % 3);
        greater = new BVHNode(r, (axis + 1) % 3);

    } else {
        Mesh* m = dynamic_cast<Mesh*>(to_contain[0]);
        // each mesh must contain its own tree based on triangles,
        // so it is not a leaf node
        if (m != NULL) {
            contained = QList<Geometry*>();

            // make a new list out of triangles
            for (int i = 0; i < m->faces.size(); i++) {
                contained.append((Geometry*) m->faces[i]);
            }

            // continue as normal
            sortByAxis(contained, axis, 0, contained.size() - 1); // sort the list

            QList<Geometry*> l = QList<Geometry*>();
            for (unsigned int i = 0; i < contained.size() / 2; i++) {
                l.append(contained[i]);
            }
            QList<Geometry*> r = QList<Geometry*>();
            for (unsigned int i = contained.size() / 2;
                 i < contained.size(); i++) {
                r.append(contained[i]);
            }

            lesser = new BVHNode(l, (axis + 1) % 3);
            greater = new BVHNode(r, (axis + 1) % 3);
        }
    }


}

void BVHNode::MakeBounds()
{

    if (this->isLeaf()) {
        // if this is a leaf, give it the bounds of its geometry
        if (contained[0]->bounds == NULL) contained[0]->MakeBoundingBox();

        bounds = contained[0]->bounds;
    } else {
        // create the bounds of children if they do not already exist
        if (lesser->bounds == NULL) {
            lesser->MakeBounds();
        }

        if (greater->bounds == NULL) {
            greater->MakeBounds();
        }

        if (contained.size() > 1) {
            // merge the children's bounding boxes
            bounds = mergeBoxes(lesser->bounds, greater->bounds);
        } else {
            // edge case: mesh is not a leaf and should use mesh bounds,
            // HOWEVER it needed to build the bounds of child nodes
            if (contained[0]->bounds == NULL) contained[0]->MakeBoundingBox();
            bounds = contained[0]->bounds;
        }
    }
}

// sorts a QList of geometry by centroid position along an axis
void BVHNode::sortByAxis(QList<Geometry *> &objs, unsigned short axis,
                         int left, int right)
{

    // quicksort algorithm
    int i = left;
    int j = right;
    Geometry *g = objs[(left + right) / 2];

    if (g->bounds == NULL) g->MakeBoundingBox();
    glm::vec3 pivotVec = objs[(left + right) / 2]->bounds->midpoint;
    float pivot;



    while (i <= j) {
        if (axis % 3 == 0) {
            pivot = pivotVec[0];
            while (objs[i]->bounds->midpoint[0] < pivot)
                i++;
            while (objs[j]->bounds->midpoint[0] > pivot)
                j--;
        } else if (axis % 3 == 1) {
            pivot = pivotVec[1];
            while (objs[i]->bounds->midpoint[1] < pivot)
                i++;
            while (objs[j]->bounds->midpoint[1] > pivot)
                j--;
        } else {
            pivot = pivotVec[2];
            while (objs[i]->bounds->midpoint[2] < pivot)
                i++;
            while (objs[j]->bounds->midpoint[2] > pivot)
                j--;
        }


        if (i <= j) {
            Geometry* t = objs[i];
            objs[i] = objs[j];
            objs[j] = t;
            i++;
            j--;
        }
    }

    if (left < j) sortByAxis(objs, axis, left, j);
    if (i < right) sortByAxis(objs, axis, i, right);
}

// leaf node, must have contain exactly one object AND have no children
// the latter avoids marking a node with a triangle mesh as a leaf, since
// each mesh has its own tree divided among triangles
bool BVHNode::isLeaf()
{
    return contained.size() == 1 && greater == NULL && lesser == NULL;
}

bool BVHNode::intersectsBounds(Ray r)
{


    if (bounds == NULL) return false;

    // shortcut: origin is inside bounds (which are in world space)
    if (r.origin.x >= bounds->x1 && r.origin.x <= bounds->x2 ||
            r.origin.y >= bounds->y1 && r.origin.y <= bounds->y2 ||
            r.origin.z >= bounds->z1 && r.origin.z <= bounds->z2) {
        return true;
    }

    float t_near = -99999;
    float t_far = 99999;

    // calculate t intersection with x slab
    float t0 = (bounds->x1 - r.origin.x) / r.direction.x;
    float t1 = (bounds->x2 - r.origin.x) / r.direction.x;

    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 > t_near) t_near = t0;
    if (t1 < t_far) t_far = t1;

    // calculate t intersection with y slab
    t0 = (bounds->y1 - r.origin.y) / r.direction.y;
    t1 = (bounds->y2 - r.origin.y) / r.direction.y;

    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 > t_near) t_near = t0;
    if (t1 < t_far) t_far = t1;

    // calculate t intersection with z slab
    t0 = (bounds->z1 - r.origin.z) / r.direction.z;
    t1 = (bounds->z2 - r.origin.z) / r.direction.z;

    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    if (t0 > t_near) t_near = t0;
    if (t1 < t_far) t_far = t1;


    return t_near <= t_far;

}

BVHNode::~BVHNode()
{
    if (contained.size() > 1 && bounds != NULL) delete bounds;
    if (lesser != NULL) delete lesser;
    if (greater != NULL) delete greater;
}

