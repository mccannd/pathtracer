#include "voxel.h"
#include "perlin.h"

static const int BOX_IDX_COUNT = 24;
static const int BOX_VERT_COUNT = 8;

Voxel::Voxel()
{

}

Voxel::Voxel(glm::vec3 mini, glm::vec3 maxi) {
    min = mini;
    max = maxi;

    // TODO: Calculate n based on distance between mini and max
    // For now, set to 2x2x2
//    float dist = glm::distance(mini, maxi);

    n = 8;

    // Fill in densities
//    for (int k = 0; k < n; k++) {
//        for(int j = 0; j < n; j++) {
//            for(int i = 0; i < n; i++) {
//                densities.append(perlin::noise(i, j, k, n));
//            }
//        }
//    }

    create();
}


// Index of Density Array
int Voxel::pointToIndex(float x, float y, float z) {
    // Transform to voxel space
    float vx = 0.5f + x;
    float vy = 0.5f + y;
    float vz = 0.5f + z;

    // obtain voxel cooridinates
    int ix = (int) glm::floor(vx * n);
    int iy = (int) glm::floor(vy * n);
    int iz = (int) glm::floor(vz * n);

    return ix + iy * n + iz * n * n;
}

// x, y, z are converted into local space
float Voxel::getDensity(float x, float y, float z) {
    // index into the densities array
    return densities.at(pointToIndex(x, y, z));
}

float Voxel::getDensity(glm::mat4 invT, glm::vec3 p) {
    glm::vec3 p_loc = glm::vec3(invT * glm::vec4(p, 1));
    return perlin::noise(p_loc[0], p_loc[1], p_loc[2], n);
//    return densities.at(pointToIndex(p_loc[0], p_loc[1], p_loc[2]));
}

glm::vec3 Voxel::ComputeNormal(const glm::vec3 &P)
{return glm::vec3(0);}

glm::vec4 Voxel::GetCubeNormal(const glm::vec4& P)
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

void Voxel::create() {
    glm::vec3 box_vert_pos[BOX_VERT_COUNT];
    glm::vec3 box_vert_col[BOX_VERT_COUNT];
    int box_idx[BOX_IDX_COUNT];

    // Top Vertices 0 - 3
    box_vert_pos[0] = max;
    box_vert_pos[1] = glm::vec3(max[0], max[1], min[2]);
    box_vert_pos[2] = glm::vec3(min[0], max[1], min[2]);
    box_vert_pos[3] = glm::vec3(min[0], max[1], max[2]);

    // Bottom Vertices 4 - 7
    box_vert_pos[4] = glm::vec3(max[0], min[1], max[2]);
    box_vert_pos[5] = glm::vec3(max[0], min[1], min[2]);
    box_vert_pos[6] = min;
    box_vert_pos[7] = glm::vec3(min[0], min[1], max[2]);

    for (unsigned int i = 0; i < 8; i++) {
        // cyan for visibility
        box_vert_col[i] = glm::vec3(0, 1, 1);
    }

    // Indices for top (VERT 0 - 3)
    box_idx[0] = 0;
    box_idx[1] = 1;
    box_idx[2] = 1;
    box_idx[3] = 2;
    box_idx[4] = 2;
    box_idx[5] = 3;
    box_idx[6] = 3;
    box_idx[7] = 0;

    // Indices for bottom (VERT 4 - 7)
    box_idx[8] = 4;
    box_idx[9] = 5;
    box_idx[10] = 5;
    box_idx[11] = 6;
    box_idx[12] = 6;
    box_idx[13] = 7;
    box_idx[14] = 7;
    box_idx[15] = 4;

    // Indices for Barrel
    box_idx[16] = 0;
    box_idx[17] = 4;
    box_idx[18] = 1;
    box_idx[19] = 5;
    box_idx[20] = 2;
    box_idx[21] = 6;
    box_idx[22] = 3;
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

GLenum Voxel::drawMode(){
    return GL_LINES;
}
