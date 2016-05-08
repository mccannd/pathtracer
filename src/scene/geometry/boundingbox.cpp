#include "boundingbox.h"

static const int BOX_IDX_COUNT = 24;
static const int BOX_VERT_COUNT = 8;

BoundingBox::BoundingBox(float xmin, float xmax,
                         float ymin, float ymax,
                         float zmin, float zmax)
{
    x1 = xmin;
    x2 = xmax;
    y1 = ymin;
    y2 = ymax;
    z1 = zmin;
    z2 = zmax;
    midpoint = glm::vec3(0.5 * (x1 + x2),
                         0.5 * (y1 + y2),
                         0.5 * (z1 + z2));
    pMax = glm::vec3(x2, y2, z2);
    pMin = glm::vec3(x1, y1, z1);
    float width = x2 - x1;
    float height = y2 - y1;
    float length = z2 - z1;
    surface_area = 2*(width*length + height*length +height*width);
    this->create();
}


void BoundingBox::setBounds(float xmin, float xmax,
                            float ymin, float ymax,
                            float zmin, float zmax)
{
    x1 = xmin;
    x2 = xmax;
    y1 = ymin;
    y2 = ymax;
    z1 = zmin;
    z2 = zmax;
    midpoint = glm::vec3(0.5 * (x1 + x2),
                         0.5 * (y1 + y2),
                         0.5 * (z1 + z2));
    float width = x2 - x1;
    float height = y2 - y1;
    float length = z2 - z1;
    surface_area = 2*(width*length + height*length +height*width);
    this->create();
}

void BoundingBox::setBounds(glm::mat4 t)
{
    glm::vec4 p1 = t * glm::vec4(x1, y1, z1, 1);
    glm::vec4 p2 = t * glm::vec4(x2, y1, z1, 1);
    glm::vec4 p3 = t * glm::vec4(x1, y2, z1, 1);
    glm::vec4 p4 = t * glm::vec4(x1, y1, z2, 1);
    glm::vec4 p5 = t * glm::vec4(x2, y2, z1, 1);
    glm::vec4 p6 = t * glm::vec4(x1, y2, z2, 1);
    glm::vec4 p7 = t * glm::vec4(x2, y1, z2, 1);
    glm::vec4 p8 = t * glm::vec4(x2, y2, z2, 1);


    x1 = std::min(p1[0], p2[0]);
    x1 = std::min(x1, p3[0]);
    x1 = std::min(x1, p4[0]);
    x1 = std::min(x1, p5[0]);
    x1 = std::min(x1, p6[0]);
    x1 = std::min(x1, p7[0]);
    x1 = std::min(x1, p8[0]);

    x2 = std::max(p1[0], p2[0]);
    x2 = std::max(x2, p3[0]);
    x2 = std::max(x2, p4[0]);
    x2 = std::max(x2, p5[0]);
    x2 = std::max(x2, p6[0]);
    x2 = std::max(x2, p7[0]);
    x2 = std::max(x2, p8[0]);

    y1 = std::min(p1[1], p2[1]);
    y1 = std::min(y1, p3[1]);
    y1 = std::min(y1, p4[1]);
    y1 = std::min(y1, p5[1]);
    y1 = std::min(y1, p6[1]);
    y1 = std::min(y1, p7[1]);
    y1 = std::min(y1, p8[1]);

    y2 = std::max(p1[1], p2[1]);
    y2 = std::max(y2, p3[1]);
    y2 = std::max(y2, p4[1]);
    y2 = std::max(y2, p5[1]);
    y2 = std::max(y2, p6[1]);
    y2 = std::max(y2, p7[1]);
    y2 = std::max(y2, p8[1]);

    z1 = std::min(p1[2], p2[2]);
    z1 = std::min(z1, p3[2]);
    z1 = std::min(z1, p4[2]);
    z1 = std::min(z1, p5[2]);
    z1 = std::min(z1, p6[2]);
    z1 = std::min(z1, p7[2]);
    z1 = std::min(z1, p8[2]);

    z2 = std::max(p1[2], p2[2]);
    z2 = std::max(z2, p3[2]);
    z2 = std::max(z2, p4[2]);
    z2 = std::max(z2, p5[2]);
    z2 = std::max(z2, p6[2]);
    z2 = std::max(z2, p7[2]);
    z2 = std::max(z2, p8[2]);

    midpoint = glm::vec3(0.5 * (x1 + x2),
                         0.5 * (y1 + y2),
                         0.5 * (z1 + z2));

    float width = x2 - x1;
    float height = y2 - y1;
    float length = z2 - z1;
    surface_area = 2*(width*length + height*length +height*width);
    pMax = glm::vec3(x2, y2, z2);
    pMin = glm::vec3(x1, y1, z1);
    this->create();
}

void BoundingBox::create()
{


    GLuint box_idx[BOX_IDX_COUNT];
    glm::vec3 box_vert_pos[BOX_VERT_COUNT];
    glm::vec3 box_vert_col[BOX_VERT_COUNT];

    box_vert_pos[0] = glm::vec3(x1, y1, z1);
    box_vert_pos[1] = glm::vec3(x2, y1, z1);
    box_vert_pos[2] = glm::vec3(x1, y2, z1);
    box_vert_pos[3] = glm::vec3(x1, y1, z2);
    box_vert_pos[4] = glm::vec3(x2, y2, z1);
    box_vert_pos[5] = glm::vec3(x1, y2, z2);
    box_vert_pos[6] = glm::vec3(x2, y1, z2);
    box_vert_pos[7] = glm::vec3(x2, y2, z2);

    for (unsigned int i = 0; i < 8; i++) {
        // cyan for visibility
        box_vert_col[i] = glm::vec3(0, 1, 1);
    }

    box_idx[0] = 0;
    box_idx[1] = 1;
    box_idx[2] = 0;
    box_idx[3] = 2;
    box_idx[4] = 0;
    box_idx[5] = 3;
    box_idx[6] = 1;
    box_idx[7] = 4;
    box_idx[8] = 1;
    box_idx[9] = 6;
    box_idx[10] = 2;
    box_idx[11] = 4;
    box_idx[12] = 2;
    box_idx[13] = 5;
    box_idx[14] = 3;
    box_idx[15] = 5;
    box_idx[16] = 3;
    box_idx[17] = 6;
    box_idx[18] = 4;
    box_idx[19] = 7;
    box_idx[20] = 5;
    box_idx[21] = 7;
    box_idx[22] = 6;
    box_idx[23] = 7;

    count = BOX_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(box_idx, BOX_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(box_vert_pos, BOX_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create(); // no normals, flat shader
    bufNor.bind();

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(box_vert_col, BOX_VERT_COUNT * sizeof(glm::vec3));

}

int BoundingBox::MaximumExtent() const {
        glm::vec3 diag = pMax - pMin;
        if (diag.x > diag.y && diag.x > diag.z)
             return 0;
        else if (diag.y > diag.z)
            return 1;
        else
             return 2;
}

// making a wireframe instead of solid mesh
GLenum BoundingBox::drawMode() { return GL_LINES; }

// returns a box that encloses both boxes
BoundingBox* mergeBoxes(BoundingBox *a, BoundingBox *b)
{
    return new BoundingBox(std::min(a->x1, b->x1),
                           std::max(a->x2, b->x2),
                           std::min(a->y1, b->y1),
                           std::max(a->y2, b->y2),
                           std::min(a->z1, b->z1),
                           std::max(a->z2, b->z2));
}

BoundingBox* mergeBoxes(BoundingBox *a, glm::vec3 &midpoint){
    float xmin = fmin(a->x1, midpoint.x);
    float ymin = fmin(a->y1, midpoint.y);
    float zmin = fmin(a->z1, midpoint.z);
    float xmax = fmax(a->x2, midpoint.x);
    float ymax = fmax(a->y2, midpoint.y);
    float zmax = fmax(a->z2, midpoint.z);
    return new BoundingBox(xmin, xmax, ymin, ymax, zmin, zmax);
}

// returns a box that encloses both boxes
BoundingBox mergeBoxes(BoundingBox& a, BoundingBox& b)
{
    return BoundingBox(std::min(a.x1, b.x1),
                           std::max(a.x2, b.x2),
                           std::min(a.y1, b.y1),
                           std::max(a.y2, b.y2),
                           std::min(a.z1, b.z1),
                           std::max(a.z2, b.z2));
}

BoundingBox mergeBoxes(BoundingBox& a, glm::vec3 &midpoint){
    float xmin = fmin(a.x1, midpoint.x);
    float ymin = fmin(a.y1, midpoint.y);
    float zmin = fmin(a.z1, midpoint.z);
    float xmax = fmax(a.x2, midpoint.x);
    float ymax = fmax(a.y2, midpoint.y);
    float zmax = fmax(a.z2, midpoint.z);
    return BoundingBox(xmin, xmax, ymin, ymax, zmin, zmax);
}

