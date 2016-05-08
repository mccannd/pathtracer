#include <scene/materials/bxdfs/lambertBxDF.h>

glm::vec3 LambertBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return diffuse_color/PI;
}
glm::vec3 LambertBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}


// ONly works with disc lights
glm::vec3 LambertBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    // Sample_f via Malley's Method
    glm::vec3 pt = ConcentricSampleDisc(rand1, rand2);

    // Obtain the z value via pythagorean
    // r^2 = x^2 + y^2 + z^2;
    float z2 = 1.f - pt[0] * pt[0] - pt[1] * pt[1];
    if (z2 <= 0.f) {
        pt[2] = 0.f;
    } else {
        pt[2] = glm::sqrt(z2);
    }
    wi_ret = pt;

    // Return the PDF and ESE
    pdf_ret = PDF(wo, wi_ret);
    return EvaluateScatteredEnergy(wo, wi_ret);
}

float LambertBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const {
    return glm::abs(glm::dot(wo, glm::vec3(0,0,1))) / PI;
    //return glm::abs(glm::dot(wi, glm::vec3(0, 0, 1))) / PI;
}

glm::vec3 ConcentricSampleDisc(float u1, float u2)
{
    float sx = 2 * u1 - 1.0f;
    float sy = 2 * u2 - 1.0f;
    float r, theta;

    if (sx == 0.0 && sy == 0.0)
    {
        return glm::vec3(0,0,0);
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
    return glm::vec3(dx, dy, 0);
}
