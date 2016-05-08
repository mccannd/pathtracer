#include "cube.h"
#include <scene/geometry/voxel.h>
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;


void Cube::SetVoxel() {
    glm::vec3 min = glm::vec3(transform.T() * glm::vec4(-0.5, -0.5 , -0.5, 0.5));
    glm::vec3 max = glm::vec3(transform.T() * glm::vec4(0.5, 0.5, 0.5, 0.5));

    vox = new Voxel(min, max);
    vox->create();
}


void Cube::MakeBoundingBox()
{
    glm::mat4 m = this->transform.T();


    glm::vec4 p1 = m * glm::vec4(0.5, 0.5, 0.5, 1);
    glm::vec4 p2 = m * glm::vec4(-0.5, 0.5, 0.5, 1);
    glm::vec4 p3 = m * glm::vec4(0.5, -0.5, 0.5, 1);
    glm::vec4 p4 = m * glm::vec4(0.5, 0.5, -0.5, 1);
    glm::vec4 p5 = m * glm::vec4(-0.5, -0.5, 0.5, 1);
    glm::vec4 p6 = m * glm::vec4(0.5, -0.5, -0.5, 1);
    glm::vec4 p7 = m * glm::vec4(-0.5, 0.5, -0.5, 1);
    glm::vec4 p8 = m * glm::vec4(-0.5, -0.5, -0.5, 1);

    float x1 = std::min(p1[0], p2[0]);
    x1 = std::min(x1, p3[0]);
    x1 = std::min(x1, p4[0]);
    x1 = std::min(x1, p5[0]);
    x1 = std::min(x1, p6[0]);
    x1 = std::min(x1, p7[0]);
    x1 = std::min(x1, p8[0]);

    float x2 = std::max(p1[0], p2[0]);
    x2 = std::max(x2, p3[0]);
    x2 = std::max(x2, p4[0]);
    x2 = std::max(x2, p5[0]);
    x2 = std::max(x2, p6[0]);
    x2 = std::max(x2, p7[0]);
    x2 = std::max(x2, p8[0]);

    float y1 = std::min(p1[1], p2[1]);
    y1 = std::min(y1, p3[1]);
    y1 = std::min(y1, p4[1]);
    y1 = std::min(y1, p5[1]);
    y1 = std::min(y1, p6[1]);
    y1 = std::min(y1, p7[1]);
    y1 = std::min(y1, p8[1]);

    float y2 = std::max(p1[1], p2[1]);
    y2 = std::max(y2, p3[1]);
    y2 = std::max(y2, p4[1]);
    y2 = std::max(y2, p5[1]);
    y2 = std::max(y2, p6[1]);
    y2 = std::max(y2, p7[1]);
    y2 = std::max(y2, p8[1]);

    float z1 = std::min(p1[2], p2[2]);
    z1 = std::min(z1, p3[2]);
    z1 = std::min(z1, p4[2]);
    z1 = std::min(z1, p5[2]);
    z1 = std::min(z1, p6[2]);
    z1 = std::min(z1, p7[2]);
    z1 = std::min(z1, p8[2]);

    float z2 = std::max(p1[2], p2[2]);
    z2 = std::max(z2, p3[2]);
    z2 = std::max(z2, p4[2]);
    z2 = std::max(z2, p5[2]);
    z2 = std::max(z2, p6[2]);
    z2 = std::max(z2, p7[2]);
    z2 = std::max(z2, p8[2]);



    bounds = new BoundingBox(x1, x2, y1, y2, z1, z2);
}


Intersection Cube::GetSamplePoint(const glm::vec3 &isx_normal) {
    int side = qrand() % 6;
    float a = ((float) qrand()) / RAND_MAX - 0.5f;
    float b = ((float) qrand()) / RAND_MAX - 0.5f;

    glm::vec4 PL;

    if (side == 0) {
            PL = glm::vec4(-0.5, a, b, 1);
    } else if (side == 1) {
            PL = glm::vec4(0.5, a, b, 1);
    } else if (side == 2) {
            PL = glm::vec4(a, -0.5, b, 1);
    } else if (side == 3) {
            PL = glm::vec4(a, 0.5, b, 1);
    } else if (side == 4) {
            PL = glm::vec4(a, b, -0.5, 1);
    } else {
            PL = glm::vec4(a, b, 0.5, 1);
    }

    glm::vec4 normalL = GetCubeNormal(PL);
    Intersection result;
    result.point = glm::vec3(transform.T() * PL);
    result.normal = glm::normalize(glm::vec3(transform.invTransT() * normalL));
    result.texture_color = material->base_color;
    if(fequal(glm::abs(normalL[0]), 1.f)) {
        result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 1, 0, 0)));
        result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 0, 1, 0)));
    } else if (fequal(glm::abs(normalL[1]), 1.f)){
        result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(1, 0, 0, 0)));
        result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 0, 1, 0)));
    } else {
        result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(1, 0, 0, 0)));
        result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 1, 0, 0)));
    }
    result.object_hit = this;
    return result;
}

void Cube::ComputeArea()
{
    glm::vec4 center = transform.T() * glm::vec4(0, 0, 0, 1);
    float a = glm::distance(transform.T() * glm::vec4(0.5, 0, 0, 1), center);
    float b = glm::distance(transform.T() * glm::vec4(0, 0.5, 0, 1), center);
    float c = glm::distance(transform.T() * glm::vec4(0, 0, 0.5, 1), center);

    area = 4 * 2 * (a * b + a * c + b * c);
}

glm::vec4 Cube::GetCubeNormal(const glm::vec4& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 3; i++){
        if(glm::abs(P[i]) > val){
            idx = i;
            val = glm::abs(P[i]);
        }
    }
    glm::vec4 N(0,0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}

glm::vec3 Cube::ComputeNormal(const glm::vec3 &P)
{return glm::vec3(0);}

Intersection Cube::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            if(r_loc.origin[i] < -0.5f || r_loc.origin[i] > 0.5f){
                return result;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (-0.5f - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (0.5f - r_loc.origin[i])/r_loc.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    float t_final = -1;
    if(t_n < t_f)
    {
        if(t_n >= 0)
        {
            t_final = t_n;
        }
        else if(t_f >= 0)
        {
            t_final = t_f;
        }
    }
    if(t_final >= 0)
    {
        //Lastly, transform the point found in object space by T
        glm::vec4 P = glm::vec4(r_loc.origin + t_final*r_loc.direction, 1);
        glm::vec4 normal = GetCubeNormal(P);
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(glm::vec3(normal), 0)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);

        if(fequal(glm::abs(normal[0]), 1.f)) {
            result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 1, 0, 0)));
            result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 0, 1, 0)));
        } else if (fequal(glm::abs(normal[1]), 1.f)){
            result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(1, 0, 0, 0)));
            result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 0, 1, 0)));
        } else {
            result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(1, 0, 0, 0)));
            result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 1, 0, 0)));
        }

        return result;
    }
    else{
        return result;
    }
}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point)
{
    glm::vec3 abs = glm::min(glm::abs(point), 0.5f);
    glm::vec2 UV;//Always offset lower-left corner
    if(abs.x > abs.y && abs.x > abs.z)
    {
        UV = glm::vec2(point.z + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.x < 0)
        {
            UV += glm::vec2(0, 0.333f);
        }
        else
        {
            UV += glm::vec2(0, 0.667f);
        }
    }
    else if(abs.y > abs.x && abs.y > abs.z)
    {
        UV = glm::vec2(point.x + 0.5f, point.z + 0.5f)/3.0f;
        //Left face
        if(point.y < 0)
        {
            UV += glm::vec2(0.333f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.333f, 0.667f);
        }
    }
    else
    {
        UV = glm::vec2(point.x + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.z < 0)
        {
            UV += glm::vec2(0.667f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.667f, 0.667f);
        }
    }
    return UV;
}

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}
