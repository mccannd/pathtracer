#pragma once
#include <scene/materials/bxdfs/bxdf.h>

class BlinnMicrofacetBxDF : public BxDF
{
public:
//Constructors/Destructors
    BlinnMicrofacetBxDF() : BlinnMicrofacetBxDF(glm::vec3(0.5f), 1.0f)
    {}
    BlinnMicrofacetBxDF(const glm::vec3 &color) : BlinnMicrofacetBxDF(color, 1.0f)
    {}
    BlinnMicrofacetBxDF(const glm::vec3 &color, float exp) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)), reflection_color(color), exponent(exp)
    {}
//Functions
    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const;
    virtual glm::vec3 EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const;

    //This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
    //It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const;

//Member variables
    glm::vec3 reflection_color;
    float exponent;
    float PDF(const glm::vec3 &wo, const glm::vec3 &wi) const;
};
