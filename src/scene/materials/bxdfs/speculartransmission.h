#pragma once
#include <scene/materials/bxdfs/bxdf.h>
#include <scene/materials/fresnel.h>


class speculartransmission : public BxDF
{
public:
    // speculartransmission();

    //Constructors/Destructors

//    speculartransmission(const glm::vec3 &color) : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)), transmission_color(color)
//    {}

    //from book pg 444
    speculartransmission(const glm::vec3 &t, float ei, float et) : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)) {
        T = t;
        etai = ei;
        etat = et;
        fresnel = new FresnelDielectric(ei, et);
    }

    //Functions
    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const;
    virtual glm::vec3 EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const;
    //This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
    //It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const;

    //Member variables
    float PDF(const glm::vec3 &wo, const glm::vec3 &wi) const;
    FresnelDielectric* fresnel;
private:
    glm::vec3 T;
    float etai, etat;

};


