#include <la.h>
#pragma once


class Fresnel
{
public:
    virtual glm::vec3 Evaluate(float cosi) const = 0;
    glm::vec3 FrCond(float cosi, const glm::vec3 &eta, const glm::vec3 &k) const;
    glm::vec3 FrDiel(float cosi, float cost, const glm::vec3 &etai, const glm::vec3 &etat) const;
};


class FresnelConductor : public Fresnel {
public:
    FresnelConductor(const glm::vec3 &e, const glm::vec3 &kk);
    glm::vec3 Evaluate(float cosi) const;
private:
    glm::vec3 eta, k;
};

class FresnelDielectric: public Fresnel {
public:
    FresnelDielectric(float ei, float et);
    glm::vec3 Evaluate(float cosi) const;
private:
    float eta_i, eta_t;
};


