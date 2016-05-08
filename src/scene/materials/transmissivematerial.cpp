#include "transmissivematerial.h"

transmissivematerial::transmissivematerial() : Material()
{
//    spec_reflect = bxdfs.at(0);
//    spec_transmit = bxdfs.at(1);
}

transmissivematerial::transmissivematerial(const glm::vec3 &color) : Material(color)
{
//    spec_reflect = bxdfs.at(0);
//    spec_transmit = bxdfs.at(1);
}

//Given an intersection with some Geometry, evaluate the scattered energy at isx given a world-space wo and wi for all BxDFs we contain that match the input flags
glm::vec3 transmissivematerial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDF *bxdf_rand, BxDFType flags) const {
    return glm::vec3(0);
}

//Given an intersection with some Geometry, generate a world-space wi then evaluate the scattered energy along the world-space wo.
glm::vec3 transmissivematerial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDF *bxdf_rand, BxDFType flags) const{
    //spec_reflect = bxdfs.at(0);
    //spec_transmit = bxdfs.at(1);
    BxDF * spec_reflect = bxdfs.at(1);
    BxDF * spec_transmit = bxdfs.at(0);
    // Convert to Normal Space
    glm::mat3 T = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
    T = glm::transpose(T);
    wiW_ret = T * wiW_ret;
    glm::vec3 wo = T * woW;

    glm::vec3 SAESE;

    //compute fresnel term based on wo, wi
        //F = [0,1]
    glm::vec3 N = glm::vec3(0, 0, 1);
    float costhetaO = glm::abs(glm::dot(wo, N));
    glm::vec3 direction = glm::vec3(-glm::refract(-wo,N, (nt/ni)));
    float costhetaI = glm::abs(glm::dot(direction, N));
    float rl = (nt * costhetaI - ni * costhetaO) / (nt * costhetaI + ni * costhetaO);
    float rp = (ni * costhetaI - nt * costhetaO) / (ni * costhetaI + nt * costhetaO);
    float fr = (rl * rl + rp * rp) / 2.f;
    fr = glm::clamp(fr, 0.f, 1.f);


    //choose to evaluate spec_reflect or spec_transmit based on unif rand variable R
        //if R <= F, sample spec_reflect
        //else, sample spec_transmit
    float rand = qrand() / (float) RAND_MAX;
    if(rand <= fr) {
        SAESE = isx.texture_color * base_color * spec_reflect->SampleAndEvaluateScatteredEnergy(-wo,wiW_ret,0,0,pdf_ret);
    } else {
        SAESE = isx.texture_color * base_color * spec_transmit->SampleAndEvaluateScatteredEnergy(-wo,wiW_ret,0,0,pdf_ret);
    }


    T = glm::transpose(T);
    wiW_ret = T * -wiW_ret;

    return SAESE;
}


