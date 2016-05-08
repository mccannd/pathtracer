#include "fresnel.h"

//Fresnel code
glm::vec3 Fresnel::FrCond(float cosi, const glm::vec3 &eta, const glm::vec3 &k) const { //referenced PBRT page 435
    glm::vec3 tmp = (eta*eta + k*k) * cosi*cosi;
    glm::vec3 Rparl2 = (tmp - (2.f * eta * cosi) + glm::vec3(1)) / (tmp + (2.f * eta * cosi) + glm::vec3(1));
    glm::vec3 tmp_f = eta*eta + k*k;
    glm::vec3 Rperp2 = (tmp_f - (2.f * eta * cosi) + cosi*cosi) / (tmp_f + (2.f * eta * cosi) + cosi*cosi);
    return (Rparl2 + Rperp2) / 2.f;
}

glm::vec3 Fresnel::FrDiel(float cosi, float cost, const glm::vec3 &etai, const glm::vec3 &etat) const { //PBRT pg 435
    glm::vec3 Rparl = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    glm::vec3 Rperp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    return (Rparl*Rparl + Rperp*Rperp) / 2.f;
}

//Fresnel Conductor
FresnelConductor::FresnelConductor(const glm::vec3 &e, const glm::vec3 &kk) : eta(e), k(kk){ //PBRT page 437
}

glm::vec3 FresnelConductor::Evaluate(float cosi) const { //referenced PBRT pg 437
    return FrCond(fabsf(cosi), eta, k);
}

//Fresnel Dielectric

FresnelDielectric::FresnelDielectric(float ei, float et) {
    eta_i = ei;
    eta_t = et;
}
glm::vec3 FresnelDielectric::Evaluate(float cosi) const { //PBRT pg 438
    cosi = glm::clamp(cosi, -1.f, 1.f);
    bool entering = cosi > 0.;
    //compute indices of refraction for dielectric
    float ei = eta_i, et = eta_t;
    if(!entering) {
        float tmp = ei;
        ei = et;
        et = tmp;
    }
    //compute sint using snell's law
    float sint = ei/et * sqrtf(fmax(0.f, 1.f - cosi*cosi));
    if(sint >= 1.0f) {
        //handle total internal reflection
        return glm::vec3(1.f);
    } else {
        float cost = sqrtf(fmax(0.f, 1.f - sint*sint));
        return FrDiel(fabsf(cosi),cost, glm::vec3(ei), glm::vec3(et));
    }
}

