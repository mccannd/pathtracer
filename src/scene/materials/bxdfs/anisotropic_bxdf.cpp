#include "anisotropic_bxdf.h"

glm::vec3 anisotropic_bxdf::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const{
    glm::vec3 wh = glm::normalize(wo + wi);
    glm::vec3 N = glm::vec3(0, 0, 1);
    float costhetaO = glm::abs(glm::dot(wo, N));
    float costhetaI = glm::abs(glm::dot(wi, N));

    if (fequal(costhetaO, 0.f) || fequal(costhetaI, 0.f)) {
        return glm::vec3(0.0f);
    }

    // Calculate Masking term, Shaow term, and the Geometric term
    float M = 2.f * glm::abs(glm::dot(N, wh) * glm::dot(N, wo) / glm::dot(wo, wh));
    float S = 2.f * glm::abs(glm::dot(N, wh) * glm::dot(N, wi) / glm::dot(wo, wh));
    float G = glm::min(1.f, glm::min(M, S));

    // Anisotropic distribution, from PBRT
    float costhetah = AbsCosTheta(wh);
    float d = 1.f - costhetah * costhetah;
    if(d == 0.f) return glm::vec3(0.f);
    float e = (ex * wh.x * wh.x + ey * wh.y * wh.y) / d;
    float Dwh = sqrtf((ex+2.f) * (ey+2.f)) * (1/ (2*PI)) * powf(costhetah, e);

    // Calculate the fresnel term
    float nt = 1.5f; // index of refraction of transmitting media
    float ni = 1.f; // indes of refraction of incedent media
    float rl = (nt * costhetaI - ni * costhetaO) / (nt * costhetaI + ni * costhetaO);
    float rp = (ni * costhetaI - nt * costhetaO) / (ni * costhetaI + nt * costhetaO);
    float F = (rl * rl + rp * rp) / 2.f;

    return color * Dwh * G * F / (4 * costhetaO * costhetaI);
}
glm::vec3 anisotropic_bxdf::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const{

}
//This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
//It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
glm::vec3 anisotropic_bxdf::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const{

    //convert to PBRT ray conventions
    glm::vec3 wo_neg = -wo;

    //sample from first quadrant and remap to hemisphere to sample wh pg 699
    float phi, costheta;
    if(rand1 < .25f) {
        sampleFirstQuadrant(4.f * rand1, rand2, &phi, &costheta);
    } else if (rand1 < .5f) {
        rand1 = 4.f * (.5f - rand1);
        sampleFirstQuadrant(rand1, rand2, &phi, &costheta);
        phi = PI - phi;
    } else if (rand1 < .75f) {
        rand1 = 4.f * (rand1 - .5f);
        sampleFirstQuadrant(rand1, rand2, &phi, &costheta);
        phi += PI;
    } else {
        rand1 = 4.f * (1.f - rand1);
        sampleFirstQuadrant(rand1, rand2, &phi, &costheta);
        phi = 2.f * PI * phi;
    }
    float sintheta = sqrtf(fmax(0.f, 1.f - costheta*costheta));
    glm::vec3 wh = SphericalDirection(sintheta, costheta,phi);
    if(!SameHemisphere(wo,wh)) wh = -wh;
    //compute incident direction by reflection about wh pg 697
    wi_ret = -wo_neg + 2.f * glm::dot(wo_neg, wh) * wh;

    //compute pdf for wi from anisotropic distribution 701
    pdf_ret = PDF(wo_neg, wi_ret);

    //convert back to rest of code ray conventions
    wi_ret = -wi_ret;

    glm::vec3 ESE = EvaluateScatteredEnergy(wo_neg,-wi_ret);
    return ESE;
}

//Member variables
float anisotropic_bxdf::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const{
    glm::vec3 wh = glm::normalize(wo + wi);
    //compute pdf for wi from anisotrpoic distriution page 701
    float costhetah = AbsCosTheta(wh);
    float ds = 1.f - costhetah * costhetah;
    float anisotropic_pdf = 0.f;
    if(ds > 0.f && glm::dot(wo, wh) > 0.f) {
        float e = (ex * wh.x * wh.x + ey * wh.y * wh.y) / ds;
        float d = sqrtf((ex+1.f) * (ey+1.f)) * (1 / (2 * PI)) * powf(costhetah, e);
        anisotropic_pdf = d / (4.f * glm::dot(wo,wh));
    }
   return anisotropic_pdf;
}

//pg 700
 void anisotropic_bxdf::sampleFirstQuadrant(float u1, float u2, float *phi, float *costheta) const{
    if(ex == ey)
        *phi = PI * u1 * .5f;
    else
        *phi = atanf(sqrtf((ex+1.f)/(ey+1.f))) * tanf(PI * u1 * .5f);
    float cosphi = cosf(*phi), sinphi = sinf(*phi);
    *costheta = powf(u2, 1.f/(ex * cosphi * cosphi + ey * sinphi * sinphi + 1));
 }
