#include <scene/materials/material.h>

class anisotropic_bxdf : public BxDF
{
public:
    anisotropic_bxdf(glm::vec3 c,float x, float y, float e)  : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)){
        ex = x;
        ey = y;
        color = c;
        exponent = e;
        if(ex > 10000.f || std::isnan(ex)) ex = 10000.f;
        if(ey > 10000.f || std::isnan(ey)) ey = 10000.f;
    }

    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const;
    virtual glm::vec3 EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const;
    //This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
    //It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const;

    void sampleFirstQuadrant(float u1, float u2, float *phi, float *costheta) const;
    //Member variables
    float PDF(const glm::vec3 &wo, const glm::vec3 &wi) const;


private:
    float ex;
    float ey;
    glm::vec3 color;
    float exponent;

};


