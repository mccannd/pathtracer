#include <scene/materials/bxdfs/bxdf.h>
#include <scene/materials/fresnel.h>

class fresnel_specularreflection : public BxDF
{
public:
    fresnel_specularreflection(const glm::vec3 &r, float ei, float et, glm::vec3 e, glm::vec3 k, float is_dielectric) : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)) {
        R = r;
        etai = ei;
        etat = et;
        eta = e;
        kk = k;
        if(is_dielectric == 1) {
            fresnel = new FresnelDielectric(etai, etat);
        } else if (is_dielectric == 0) {
            fresnel = new FresnelConductor(eta, kk);
        }
    }

    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const;
    virtual glm::vec3 EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const;
    //This generates an incoming light direction wi based on rand1 and rand2 and returns the result of EvaluateScatteredEnergy based on wi.
    //It "returns" wi by storing it in the supplied reference to wi. Likewise, it "returns" the value of its PDF given wi and wo in the reference to pdf.
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const;

    //Member variables
    float PDF(const glm::vec3 &wo, const glm::vec3 &wi) const;

    Fresnel* fresnel;
private:
    glm::vec3 R;
    float etai;
    float etat;
    glm::vec3 eta;
    glm::vec3 kk;
    
};
