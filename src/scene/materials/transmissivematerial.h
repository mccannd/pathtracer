#include <scene/materials/material.h>

class transmissivematerial : public Material
{
public:
    transmissivematerial();
    transmissivematerial(const glm::vec3 &color);

//functions
    //Given an intersection with some Geometry, evaluate the scattered energy at isx given a world-space wo and wi for all BxDFs we contain that match the input flags
    virtual glm::vec3 EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDF *bxdf_rand, BxDFType flags = BSDF_ALL) const;

    //Given an intersection with some Geometry, generate a world-space wi then evaluate the scattered energy along the world-space wo.
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDF *bxdf_rand, BxDFType flags = BSDF_ALL) const;

//Member variables
    //relies on XML files inputting reflective BxDF first and transmissive BxDF second!

    float ni;
    float nt;

};


