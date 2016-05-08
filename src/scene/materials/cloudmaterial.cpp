#include "scene/materials/cloudmaterial.h"





/**
 * @brief cloudmaterial::cloudmaterial
 * This is literally a decoy just so that it can be read in as a material in the xml file
 * which will then build the voxel buffer.
 */



cloudmaterial::cloudmaterial()
{

}

//Given an intersection with some Geometry, evaluate the scattered energy at isx given a world-space wo and wi for all BxDFs we contain that match the input flags
glm::vec3 cloudmaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDF *bxdf_rand, BxDFType flags) const {
    // Trace Ray inside of the cloud
//    Intersection inside = intersection_engine


//    // Convert to Normal Space
//    glm::mat3 T = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
//    T = glm::transpose(T);
//    glm::vec3 wi = T * wiW;
//    glm::vec3 wo = T * woW;

    // Ray Marching
    return glm::vec3();

}

//Given an intersection with some Geometry, generate a world-space wi then evaluate the scattered energy along the world-space wo.
glm::vec3 cloudmaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDF *bxdf_rand, BxDFType flags) const {
//    // Convert to Normal Space
//    glm::mat3 T = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
//    T = glm::transpose(T);
//    glm::vec3 wi = T * wiW;
//    glm::vec3 wo = T * woW;

        return glm::vec3();
}
