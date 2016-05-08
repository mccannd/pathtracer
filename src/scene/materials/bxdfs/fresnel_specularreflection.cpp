#include "fresnel_specularreflection.h"

glm::vec3 fresnel_specularreflection::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const{
    return glm::vec3(0);
}

glm::vec3 fresnel_specularreflection::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const{
    return glm::vec3(0);
}

//This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
//It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
glm::vec3 fresnel_specularreflection::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const{
    //compute perfect specular reflection direction
    wi_ret = glm::reflect(wo, glm::vec3(0, 0, 1));
    pdf_ret = 1.0f;
    glm::vec3 fresnel_term = fresnel->Evaluate(CosTheta(wo));
    float abscos = AbsCosTheta(wi_ret);
    glm::vec3 output = fresnel_term * R / abscos;
    return output;
   // return glm::vec3(0,0,1);
}

//Member variables
float fresnel_specularreflection::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const{
    glm::vec3 reflected_ray = glm::reflect(-wo, glm::vec3(0, 0, 1));
    if (glm::all(glm::equal(reflected_ray, wi))) {
        return 1.0f;
    } else {
        return 0.0f;
    }
}
