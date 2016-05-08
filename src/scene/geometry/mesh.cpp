#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>


//void Triangle::SetVoxel() {
//    glm::vec3 min = glm::vec3(
//                        std::min(std::min(points[0][0], points[1][0]), points[2][0]),
//                        std::min(std::min(points[0][1], points[1][1]), points[2][1]),
//                        std::min(std::min(points[0][2], points[1][2]), points[2][2]));
//    glm::vec3 max = glm::vec3(
//                        std::max(std::max(points[0][0], points[1][0]), points[2][0]),
//                        std::max(std::max(points[0][1], points[1][1]), points[2][1]),
//                        std::max(std::max(points[0][2], points[1][2]), points[2][2]));
////    vox = new Voxel(
////                glm::vec3(meshtransform * glm::vec4(min, 1)),
////                glm::vec3(meshtransform * glm::vec4(max, 1)), this);
//    vox = new Voxel(min, max);
//    vox->create();
//}

//void Mesh::SetVoxel() {
//    QList<BoundingBox*> bbt;
//    glm::vec3 min = glm::vec3((float) INFINITY);
//    glm::vec3 max = glm::vec3((float) -INFINITY);

//    for (Geometry *t : faces) {
//        ((Triangle*) t)->meshtransform = transform.T();
//        t->SetBoundingBox();
//        bbt.append(t->boundingbox);
//    }
//    // Create Own abb-> Tree
//    BVHNode* triangle_root = BVH(&bbt, 0);
//    triangle_root->bb->iammesh = true;
//    boundingbox = triangle_root->bb;
//    boundingbox->tri_root = triangle_root;
//}

void Mesh::MakeBoundingBox()
{

    faces[0]->transform = this->transform;
    if (faces[0]->bounds == NULL) faces[0]->MakeBoundingBox();

    float xmin = faces[0]->bounds->x1;
    float xmax = faces[0]->bounds->x2;
    float ymin = faces[0]->bounds->y1;
    float ymax = faces[0]->bounds->y2;
    float zmin = faces[0]->bounds->z1;
    float zmax = faces[0]->bounds->z2;

    // iterate through all triangles and create an enclosing bounding box
    for (Triangle *t : faces) {
        t->transform = this->transform;
        if (t->bounds == NULL) {
           t->MakeBoundingBox();
        }

        //t->bounds->setBounds(this->transform.T());

        xmin = std::min(xmin, t->bounds->x1);
        xmax = std::max(xmax, t->bounds->x2);
        ymin = std::min(ymin, t->bounds->y1);
        ymax = std::max(ymax, t->bounds->y2);
        zmin = std::min(zmin, t->bounds->z1);
        zmax = std::max(zmax, t->bounds->z2);

    }
    if (bounds == NULL) {
        bounds = new BoundingBox(xmin, xmax, ymin, ymax, zmin, zmax);
    } else {
        bounds->setBounds(xmin, xmax, ymin, ymax, zmin, zmax);
    }
}

void Triangle::MakeBoundingBox()
{
    glm::mat4 m = this->transform.T();
    //m = glm::mat4(1.0f);

    glm::vec4 a = m * glm::vec4(points[0], 1);
    glm::vec4 b = m * glm::vec4(points[1], 1);
    glm::vec4 c = m * glm::vec4(points[2], 1);

    float x1 = std::min(std::min(a[0], b[0]), c[0]);
    float y1 = std::min(std::min(a[1], b[1]), c[1]);
    float z1 = std::min(std::min(a[2], b[2]), c[2]);

    float x2 = std::max(std::max(a[0], b[0]), c[0]);
    float y2 = std::max(std::max(a[1], b[1]), c[1]);
    float z2 = std::max(std::max(a[2], b[2]), c[2]);

    bounds = new BoundingBox(x1, x2, y1, y2, z1, z2);
}


Intersection Triangle::GetSamplePoint(const glm::vec3 &isx_normal) {
    float a = qrand() / (float) RAND_MAX;
    float b = qrand() / (float) RAND_MAX;

    if (a + b > 1.0f) {
        // Reflect over the axis created by point1 to point2
      a /= 2.0f;
      b /= 2.0f;
    }

    // points[0] is the "origin" of the triangle
    glm::vec3 v1 = points[1] - points[0];
    glm::vec3 v2 = points[2] - points[0];

    glm::vec3 P = points[0] + a * v1 + b * v2;
    Intersection result;
    result.point = P;

    //2. Barycentric test
    float S = 0.5f * glm::length(glm::cross(points[0] - points[1], points[0] - points[2]));
    float s1 = 0.5f * glm::length(glm::cross(P - points[1], P - points[2]))/S;
    float s2 = 0.5f * glm::length(glm::cross(P - points[2], P - points[0]))/S;
    float s3 = 0.5f * glm::length(glm::cross(P - points[0], P - points[1]))/S;
    float sum = s1 + s2 + s3;

    if(s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && fequal(sum, 1.0f)){
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        result.object_hit = this;

        // Compute the tangent and bitangent of the triangle
        glm::vec3 dpos1 = points[1] - points[0];
        glm::vec3 dpos2 = points[2] - points[1];

        glm::vec2 uv0 = GetUVCoordinates(points[0]);
        glm::vec2 uv1 = GetUVCoordinates(points[1]);
        glm::vec2 uv2 = GetUVCoordinates(points[2]);

        glm::vec2 duv1 = uv1 - uv0;
        glm::vec2 duv2 = uv2 - uv0;

        result.tangent = (duv2[1] * dpos1 - duv1[1] * dpos2)
                            / (duv2[1] * duv1[0] - duv1[1] * duv2[0]);

        if (fequal(duv2[1], 0.0f)) {
            result.bitangent = (dpos1 - duv1[0] * result.tangent) / duv1[1];
        } else {
            result.bitangent = (dpos2 - duv2[0] * result.tangent) / duv2[1];
        }
    }
    return result;
}

Intersection Mesh::GetSamplePoint(const glm::vec3 &isx_normal) {
    int t = qrand() % faces.size();
    Geometry* rand_triangle = faces.at(t);
    Intersection result = rand_triangle->GetSamplePoint(isx_normal);

    result.point = glm::vec3(transform.T() * glm::vec4(result.point, 1));
    result.normal = glm::vec3(transform.T() * glm::vec4(result.normal, 1));
    result.tangent = glm::vec3(transform.T() * glm::vec4(result.tangent, 1));
    result.bitangent = glm::vec3(transform.T() * glm::vec4(result.bitangent, 1));

    return result;
}

//void Triangle::ComputeArea()
//{
//    //Extra credit to implement this
//    float a = glm::distance(points[0], points[1]);
//    float b = glm::distance(points[1], points[2]);
//    float c = glm::distance(points[2], points[0]);
//    float s = (a + b + c) / 2; // semiperimeter

//    // Heron's formula for area
//    area = sqrt(s * (s - a) * (s - b) * (s - c));
//}

//void Mesh::ComputeArea()
//{
//    area = 0;
//    for(Geometry *tri : faces) {
//        tri->ComputeArea();
//        area += tri->area;
//    }
//}

void Triangle::ComputeArea()
{
    glm::mat4 m = this->transform.T();

    //Extra credit to implement this
    glm::vec3 p0 = glm::vec3(m * glm::vec4(points[0], 1));
    glm::vec3 p1 = glm::vec3(m * glm::vec4(points[1], 1));
    glm::vec3 p2 = glm::vec3(m * glm::vec4(points[2], 1));


    float a = glm::distance(p0, p1);
    float b = glm::distance(p1, p2);
    float c = glm::distance(p2, p0);
    float s = (a + b + c) / 2; // semiperimeter

    // Heron's formula for area
    area = sqrt(s * (s - a) * (s - b) * (s - c));
}

void Mesh::ComputeArea()
{
    area = 0;
    for(Geometry *tri : faces) {
        tri->transform = this->transform;

        tri->ComputeArea();
        area += tri->area;
    }
}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

float Area(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &P)
{
    float A = Area(points[0], points[1], points[2]);
    float A0 = Area(points[1], points[2], P);
    float A1 = Area(points[0], points[2], P);
    float A2 = Area(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}

glm::vec3 Triangle::ComputeNormal(const glm::vec3 &P)
{}
glm::vec3 Mesh::ComputeNormal(const glm::vec3 &P)
{}

//HAVE THEM IMPLEMENT THIS
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r){
    //1. Ray-plane intersection
    Intersection result;
    float t =  glm::dot(plane_normal, (points[0] - r.origin)) / glm::dot(plane_normal, r.direction);

    glm::vec3 P = r.origin + t * r.direction;
    //2. Barycentric test
    float S = 0.5f * glm::length(glm::cross(points[0] - points[1], points[0] - points[2]));
    float s1 = 0.5f * glm::length(glm::cross(P - points[1], P - points[2]))/S;
    float s2 = 0.5f * glm::length(glm::cross(P - points[2], P - points[0]))/S;
    float s3 = 0.5f * glm::length(glm::cross(P - points[0], P - points[1]))/S;
    float sum = s1 + s2 + s3;

    if(s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && fequal(sum, 1.0f) && t >= 0){
        result.t = t;
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        result.object_hit = this;

        //Store the tangent and bitangent
        glm::vec3 P1 = points[0];
        glm::vec3 P2 = points[1];
        glm::vec3 P3 = points[2];

        glm::vec3 normal_compare = glm::normalize(glm::cross(P2-P1, P3-P1));

        result.normal = normal_compare;//glm::normalize(P);
        result.tangent = glm::normalize(P2-P1);
        result.bitangent = glm::cross(result.tangent, result.normal);

        tri_bitangent = result.bitangent;
        tri_tangent = result.tangent;

    }
    return result;
}

Intersection Mesh::GetIntersection(Ray r){
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection closest;
    for(int i = 0; i < faces.size(); i++){
        Intersection isx = faces[i]->GetIntersection(r_loc);
        if(isx.object_hit != NULL && isx.t > 0 && (isx.t < closest.t || closest.t < 0)){
            closest = isx;
        }
    }
    if(closest.object_hit != NULL)
    {
        Triangle* tri = (Triangle*)closest.object_hit;
        glm::vec4 P = glm::vec4(closest.t * r_loc.direction + r_loc.origin, 1);
        closest.point = glm::vec3(transform.T() * P);
        closest.normal = glm::normalize(glm::vec3(transform.invTransT() * tri->GetNormal(P)));
        closest.object_hit = this;
        closest.t = glm::distance(closest.point, r.origin);//The t used for the closest triangle test was in object space

        closest.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(tri->tri_tangent,0)));
        closest.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(tri->tri_bitangent,0)));
    }
    return closest;
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}

glm::vec2 Mesh::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2();
}


glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point)
{
    float A = Area(points[0], points[1], points[2]);
    float A0 = Area(points[1], points[2], point);
    float A1 = Area(points[0], points[2], point);
    float A2 = Area(points[0], points[1], point);
    return uvs[0] * A0/A + uvs[1] * A1/A + uvs[2] * A2/A;
}

void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
        //TODO: .mtl file loading
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}
