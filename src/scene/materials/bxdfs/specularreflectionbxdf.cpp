#include <scene/materials/bxdfs/specularreflectionBxDF.h>

glm::vec3 SpecularReflectionBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 reflected_ray = -glm::reflect(-wo, glm::vec3(0, 0, 1));
    if (glm::all(glm::equal(reflected_ray, wi))) {
        return reflection_color;
    } else {
        return glm::vec3(0.0f);
    }
}

glm::vec3 SpecularReflectionBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0.0f);
}

glm::vec3 SpecularReflectionBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    // Reflect -wo off of the normal
    wi_ret = glm::reflect(-wo, glm::vec3(0, 0, 1));
    pdf_ret = 1.0f;
    return reflection_color;
}

float SpecularReflectionBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const {
    glm::vec3 reflected_ray = glm::reflect(-wo, glm::vec3(0, 0, 1));
    if (glm::all(glm::equal(reflected_ray, wi))) {
        return 1.0f;
    } else {
        return 0.0f;
    }
}
