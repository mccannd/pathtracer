#include "speculartransmission.h"

////constructor
//speculartransmission::speculartransmission(const glm::vec3 &color) : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), transmission_color(color)
//{}

//Functions
glm::vec3 speculartransmission::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const {
    return glm::vec3(0);
}

glm::vec3 speculartransmission::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const{
    return glm::vec3(0);
}


//This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
//It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
glm::vec3 speculartransmission::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const{

    //figure out which n is incident and which is transmitted pg 445
    bool entering = CosTheta(wo) > 0;
    float ei = etai;
    float et = etat;
    if(!entering) {
        float tmp = ei;
        ei = et;
        et = tmp;
    }
    //compute transmitted ray direction pg 445
    float sini2 = SinTheta2(wo);
    float eta = ei / et;
    float sint2 = eta * eta * sini2;
    //handle total internal reflection
    if(sint2 >= 1.0f) return glm::vec3(0.f);
    float cost = sqrtf(fmax(0.f, 1.f - sint2));
    if (entering) cost = -cost;
    float sintOverSini = eta;
    wi_ret = glm::vec3(sintOverSini * -wo.x, sintOverSini * -wo.y, cost);
    pdf_ret = 1.0;
    glm::vec3 F = fresnel->Evaluate(CosTheta(wo));
    float abscos = AbsCosTheta(wi_ret);
    float eta_ratio = (et*et)/(ei*ei);
    glm::vec3 vec = glm::vec3(1.0)-F;
    glm::vec3 t_term = T / abscos;
    glm::vec3 output = eta_ratio * vec * t_term;
    return output;
}

float speculartransmission::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const {
    return 1.0f;
}
