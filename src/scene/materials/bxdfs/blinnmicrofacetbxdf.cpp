#include <scene/materials/bxdfs/blinnmicrofacetbxdf.h>

glm::vec3 BlinnMicrofacetBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
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

    // Exponential fallout
    float Dwh = (exponent + 2.f)/(2.f * PI) * powf(glm::dot(wh, N), exponent);

    // Calculate the fresnel term
    float nt = 1.f; // index of refraction of transmitting media
    float ni = 1.f; // indes of refraction of incedent media
    float rl = (nt * costhetaI - ni * costhetaO) / (nt * costhetaI + ni * costhetaO);
    float rp = (ni * costhetaI - nt * costhetaO) / (ni * costhetaI + nt * costhetaO);
    float fr = (rl * rl + rp * rp) / 2.f;

    float F = 1.f; // hardcode fresnel

    return reflection_color * Dwh * G * F / (4 * costhetaO * costhetaI);
}
glm::vec3 BlinnMicrofacetBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}

glm::vec3 BlinnMicrofacetBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    // Calculate the PDF in addition to the new wi
    float costheta = powf(rand1, 1.0f / (exponent + 1.0f));
    float sintheta = sqrtf(fmax(0.f, 1.f - costheta * costheta));
    float phi = rand2 * 2.f * PI;
    glm::vec3 wh = glm::normalize(glm::vec3(sintheta * cosf(phi), sintheta * sinf(phi), costheta));

    // Check if on the not on the same hemisphere
    if (wh[2] * wo[2] < 0.0f) {
        wh = -wh;
    }

    // Calculate new wj
    // Reflection equation basically. Actually is. Literally. I mean it.
//    wi_ret = -wo + 2.f * glm::dot(wo, wh) * wh;
    wi_ret = glm::reflect(-wo, wh);

    // Calculate PDF
    pdf_ret = PDF(wo, wi_ret);

    // Calculate ESE
    glm::vec3 ESE = EvaluateScatteredEnergy(wo, wi_ret);

    return ESE;
}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const {
    glm::vec3 wh = glm::normalize(wo + wi);
    float costheta = glm::abs(wh[2]);

    if (glm::dot(wo, wh) <= 0.f) {
        return 0.f;
    } else {
        float c = (exponent + 1.f) / (2.f * PI);
        return powf(costheta, exponent) * c / (4.f * glm::dot(wo, wh));
    }
}
