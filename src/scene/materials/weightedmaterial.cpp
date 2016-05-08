#include <scene/materials/weightedmaterial.h>

WeightedMaterial::WeightedMaterial() : Material(){}
WeightedMaterial::WeightedMaterial(const glm::vec3 &color) : Material(color){}

BxDF* WeightedMaterial::GetRandomBxDF() {
    // Random Sample
    float r1 = qrand() / (float) RAND_MAX;
    float interval = 0.f;

    for(int i = 0; i < bxdfs.size(); i++) {
        if (r1 >= interval && r1 <= interval + bxdf_weights.at(i)){
            return bxdfs.at(i);
        }
        interval += bxdf_weights.at(i);
    }

}

// RELIES ON NICE XML FILES
glm::vec3 WeightedMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDF *bxdf_rand, BxDFType flags) const
{
    // Convert to Normal Space
    glm::mat3 T = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
    T = glm::transpose(T);
    glm::vec3 wi = T * wiW;
    glm::vec3 wo = T * woW;

//    // Random Sample
//    float r1 = qrand() / (float) RAND_MAX;
//    float interval = 0.f;

//    if (currBxDF == NULL) {
//        SetBxDF();
//    }

//    glm::vec3 ESE;
//    for(int i = 0; i < bxdfs.size(); i++) {
//        if (r1 >= interval && r1 <= interval + bxdf_weights.at(i)){
//            ESE = isx.texture_color * base_color *
//                    bxdfs.at(i)->EvaluateScatteredEnergy(T * wo, T * wi, pdf_ret);
//            break;
//        }
//        interval += bxdf_weights.at(i);
//    }

    // Convert back to world space
    // But since not returning, it's fine
    return isx.texture_color * base_color * bxdf_rand->EvaluateScatteredEnergy(T * wo, T * wi);
}

glm::vec3 WeightedMaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDF *bxdf_rand,  BxDFType flags) const
{
    // Convert to Normal Space
    glm::mat3 T = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
    T = glm::transpose(T);
    wiW_ret = T * wiW_ret;
    glm::vec3 wo = T * woW;

//    // Random Sample
//    float r1 = qrand() / (float) RAND_MAX;
//    float interval = 0.f;

//    // Random
    float r2 = qrand() / (float) RAND_MAX;
    float r3 = qrand() / (float) RAND_MAX;

    glm::vec3 SAESE;
//    for(int i = 0; i < bxdfs.size(); i++) {
//        if (r1 >= interval && r1 <= interval + bxdf_weights.at(i)){
//            SAESE = isx.texture_color * base_color * bxdfs.at(i)->SampleAndEvaluateScatteredEnergy(wo, wiW_ret, r2, 23, pdf_ret);
//            break;
//        }
//        interval += bxdf_weights.at(i);
//    }

    SAESE = isx.texture_color * base_color * bxdf_rand->SampleAndEvaluateScatteredEnergy(wo, wiW_ret, r2, r3, pdf_ret);

    // Convert back to world space
    // Inverse of a rotation matrix is the same as its transpose
    T = glm::transpose(T);
    wiW_ret = T * wiW_ret;


    return SAESE;
}
