#include <scene/materials/bxdfs/bxdf.h>


//This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
//It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
glm::vec3 BxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    //TODO
    wi_ret = glm::vec3(0);
    pdf_ret = 0.0f;
    return EvaluateScatteredEnergy(wo, wi_ret);
}

glm::vec3 BxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const
{
    //TODO
    return glm::vec3(0);
}

float BxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return glm::abs(glm::dot(wi, glm::vec3(0, 0, 1.f))) / PI;
}
