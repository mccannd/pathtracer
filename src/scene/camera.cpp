#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{
    look = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp),
    enableDOF(0),
    lensRadius(0),
    focalDistance(0)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H),
    enableDOF(c.enableDOF),
    focalDistance(c.focalDistance),
    lensRadius(c.lensRadius)
{}

void Camera::CopyAttributes(const Camera &c)
{
    fovy = c.fovy;
    near_clip = c.near_clip;
    far_clip = c.far_clip;
    eye = c.eye;
    ref = c.ref;
    look = c.look;
    up = c.up;
    right = c.right;
    width = c.width;
    height = c.height;
    aspect = c.aspect;
    V = c.V;
    H = c.H;
    enableDOF = c.enableDOF;
    focalDistance = c.focalDistance;
    lensRadius = c.lensRadius;
}

void Camera::RecomputeAttributes()
{
    look = glm::normalize(ref - eye);
    right = glm::normalize(glm::cross(look, world_up));
    up = glm::cross(right, look);

    float tan_fovy = tan(fovy/2 * DEG2RAD);
    float len = glm::length(ref - eye);
    aspect = width/(float) height;
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(fovy * DEG2RAD, width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    deg *= DEG2RAD;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    deg *= DEG2RAD;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), deg, right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}

Ray Camera::Raycast(const glm::vec2 &pt)
{
    return Raycast(pt.x, pt.y);
}

Ray Camera::Raycast(float x, float y)
{
    float ndc_x = (2*x/width - 1);
    float ndc_y = (1 - 2*y/height);
    return RaycastNDC(ndc_x, ndc_y);
}

Ray Camera::RaycastNDC(float ndc_x, float ndc_y)
{
    glm::vec3 P = ref + ndc_x*H + ndc_y*V;
    Ray result(eye, P - eye);
    if(enableDOF == 1){
        //modify ray for depth of field
        if(lensRadius > 0.f) {
            //sample point on lens
            float lensU = ((float) rand()) / RAND_MAX;
            float lensV = ((float) rand()) / RAND_MAX;
            glm::vec2 sample = ConcentricSampleDisc(lensU, lensV);
            sample *= lensRadius;
            //compute point on plane of focus
            float ft = focalDistance / result.direction.z;
            glm::vec3 Pfocus = result.origin + (result.direction * ft);
            //update ray for effect of lens
            result.origin += glm::vec3(sample.x, sample.y, 0.f);
            result.direction = glm::normalize(Pfocus - result.origin);
        }
    }
    return result;
}

glm::vec2 Camera::ConcentricSampleDisc(float u1, float u2)
{
    float sx = 2 * u1 - 1.0f;
    float sy = 2 * u2 - 1.0f;
    float r, theta;

    if (sx == 0.0 && sy == 0.0)
    {
        return glm::vec2(0,0);
    }
    if (sx >= -sy)
    {
        if (sx > sy)
        {
            // Handle first region of disk
            r = sx;
            if (sy > 0.0) theta = sy/r;
            else          theta = 8.0f + sy/r;
        }
        else
        {
            // Handle second region of disk
            r = sy;
            theta = 2.0f - sx/r;
        }
    }
    else
    {
        if (sx <= sy)
        {
            // Handle third region of disk
            r = -sx;
            theta = 4.0f - sy/r;
        }
        else
        {
            // Handle fourth region of disk
            r = -sy;
            theta = 6.0f + sx/r;
        }
    }
    theta *= PI / 4.f;
    float dx = r * cosf(theta);
    float dy = r * sinf(theta);
    return glm::vec2(dx, dy);
}


void Camera::create()
{
    std::vector<glm::vec3> pos;
    std::vector<glm::vec3> col;
    std::vector<GLuint> idx;

    //0: Eye position
    pos.push_back(eye);
    //1 - 4: Near clip
        //Lower-left
        Ray r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * near_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * near_clip);
    //5 - 8: Far clip
        //Lower-left
        r = this->RaycastNDC(-1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Lower-right
        r = this->RaycastNDC(1,-1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-right
        r = this->RaycastNDC(1,1);
        pos.push_back(eye + r.direction * far_clip);
        //Upper-left
        r = this->RaycastNDC(-1,1);
        pos.push_back(eye + r.direction * far_clip);

    for(int i = 0; i < 9; i++){
        col.push_back(glm::vec3(1,1,1));
    }

    //Frustum lines
    idx.push_back(1);idx.push_back(5);
    idx.push_back(2);idx.push_back(6);
    idx.push_back(3);idx.push_back(7);
    idx.push_back(4);idx.push_back(8);
    //Near clip
    idx.push_back(1);idx.push_back(2);
    idx.push_back(2);idx.push_back(3);
    idx.push_back(3);idx.push_back(4);
    idx.push_back(4);idx.push_back(1);
    //Far clip
    idx.push_back(5);idx.push_back(6);
    idx.push_back(6);idx.push_back(7);
    idx.push_back(7);idx.push_back(8);
    idx.push_back(8);idx.push_back(5);

    //Camera axis
    pos.push_back(eye); col.push_back(glm::vec3(0,0,1)); idx.push_back(9);
    pos.push_back(eye + look); col.push_back(glm::vec3(0,0,1));idx.push_back(10);
    pos.push_back(eye); col.push_back(glm::vec3(1,0,0));idx.push_back(11);
    pos.push_back(eye + right); col.push_back(glm::vec3(1,0,0));idx.push_back(12);
    pos.push_back(eye); col.push_back(glm::vec3(0,1,0));idx.push_back(13);
    pos.push_back(eye + up); col.push_back(glm::vec3(0,1,0));idx.push_back(14);

    count = idx.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx.data(), count * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(pos.data(), pos.size() * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(col.data(), col.size() * sizeof(glm::vec3));
}

GLenum Camera::drawMode(){return GL_LINES;}
