#include <scene/geometry/square.h>

void SquarePlane::MakeBoundingBox()
{
    glm::mat4 m = this->transform.T();
    glm::vec4 cub_vert_pos[4];

    cub_vert_pos[0] = m * glm::vec4(-0.5f, 0.5f, 0, 1);
    cub_vert_pos[1] = m * glm::vec4(-0.5f, -0.5f, 0, 1);
    cub_vert_pos[2] = m * glm::vec4(0.5f, -0.5f, 0, 1);
    cub_vert_pos[3] = m * glm::vec4(0.5f, 0.5f, 0, 1);

    float minx = cub_vert_pos[0][0];
    float maxx = cub_vert_pos[0][0];
    float miny = cub_vert_pos[0][1];
    float maxy = cub_vert_pos[0][1];
    float minz = cub_vert_pos[0][2];
    float maxz = cub_vert_pos[0][2];

    for (int i = 0; i < 4; i++) {
        minx = std::min(minx, cub_vert_pos[i][0]);
        maxx = std::max(maxx, cub_vert_pos[i][0]);
        miny = std::min(miny, cub_vert_pos[i][1]);
        maxy = std::max(maxy, cub_vert_pos[i][1]);
        minz = std::min(minz, cub_vert_pos[i][2]);
        maxz = std::max(maxz, cub_vert_pos[i][2]);
    }

    bounds = new BoundingBox(minx, maxx, miny, maxy, minz, maxz);

}

Intersection SquarePlane::GetSamplePoint(const glm::vec3 &isx_normal) {
    float x = ((float) qrand()) / RAND_MAX - 0.5f;
    float y = ((float) qrand()) / RAND_MAX - 0.5f;

    glm::vec4 PL = glm::vec4(x, y, 0, 1);
    glm::vec4 normalL = glm::vec4(0, 0, 1, 1);
    glm::vec3 T = glm::vec3(0, 1, 0);
    glm::vec3 B = glm::vec3(1, 0, 0);

    Intersection result;
    result.point = glm::vec3(transform.T() * PL);
    result.normal = glm::normalize(glm::vec3(transform.invTransT() * normalL));
    result.texture_color = material->base_color;
    result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(T, 0)));
    result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(B, 0)));
    result.object_hit = this;
    return result;
}

void SquarePlane::ComputeArea()
{
    glm::vec4 center = transform.T() * glm::vec4(0, 0, 0, 1);
    glm::vec4 a = transform.T() * glm::vec4(0.5, 0, 0, 1);
    glm::vec4 b = transform.T() * glm::vec4(0, 0.5, 0, 1);
    area = 4 * glm::distance(a, center) * glm::distance(b, center);
}

Intersection SquarePlane::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    glm::vec4 P = glm::vec4(t * r_loc.direction + r_loc.origin, 1);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(P)), 0)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.texture_color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);
        result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(0, 1, 0, 0)));
//        result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(1, 0, 0, 0)));
        result.bitangent = glm::normalize(glm::cross(result.tangent, result.normal));
        return result;
    }
    return result;
}


glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);
}

glm::vec3 SquarePlane::ComputeNormal(const glm::vec3 &P)
{
        return glm::vec3(0,0,1);
}

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}
