#include <raytracing/integrator.h>
#include <raytracing/directlightingintegrator.h>
#include <scene/geometry/perlin.h>

Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

glm::vec3 ComponentMult(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}

// Ray r is the ray from the camera
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth) {
    // When the recursion depth reaches a maximum value (assigned in the scene configuration file)
    // RETURN instead of evaluating the color of the ray.

    // Create a copy of r as wi to pass into our lighitn equation
    // It will get overridden, it will be passed in by reference
    Ray wi = Ray(r.origin, r.direction);

    // Keep track of the cumulative BxDF_Color * |dot_wj_N| term
    glm::vec3 Cumulative_BxDFCol_Dot = glm::vec3(1.f, 1.f, 1.f);

    // Cumulative Color
    glm::vec3 LTE = glm::vec3(0.0f);

    // Create variables for the curent BxDF_PDF and BxDF_Color * |dot| terms
    glm::vec3 Current_BxDFCol_Dot;
    float Current_BxDF_PDF = 1.f;

    float throughput = 1.f;

    #define INDIRECT
    #ifdef INDIRECT
    while (depth < max_depth) {
        // Break if the current BxDF PDF is less than 0.
        if (Current_BxDF_PDF < 0.f) {
            break;
        }

        // wi, current_bxdf_pdf, current_bxdfcol_dot gets updated in DirectLightingIntegratorTraceRay
        // Multiply this iteration's color by the previous iterations' cumulative_bxdfcol_dot term to dampen
        // the current iteration's effect.
        LTE += DirectLightingIntegratorTraceRay(wi, depth, Current_BxDF_PDF, Current_BxDFCol_Dot) * Cumulative_BxDFCol_Dot;

        // Update Cumulative_BxDFCol_Dot
        Cumulative_BxDFCol_Dot *= Current_BxDFCol_Dot;

        // Russian Roulette
        // Terminate your loop if a uniform random number you generate is higher than your
        // throughput, but only if you've iterated three or more times.
        float local_throughput = glm::min(1.0f, fmaxf(fmaxf(Cumulative_BxDFCol_Dot[0], Cumulative_BxDFCol_Dot[1]), Cumulative_BxDFCol_Dot[2]));
        throughput *= local_throughput;
        float rand_terminate = qrand() / (float) RAND_MAX;
        if (depth >= 2 && rand_terminate > throughput) {
            break;
        }

        // Increment depth
        depth++;
    }
    #else
        LTE = DirectLightingIntegratorTraceRay(wi, depth, Current_BxDF_PDF, Cumulative_BxDFCol_Dot);
//        LTE = DirectLightingIntegratorTraceRayWithClouds(wi, depth, Current_BxDF_PDF, Cumulative_BxDFCol_Dot);
    #endif

    return ((float) scene->lights.size()) *  LTE;
}

// Treating Ray r as the ray from the camera
glm::vec3 Integrator::DirectLightingIntegratorTraceRay(Ray &r, unsigned int depth, float &current_brdf_PDF, glm::vec3 &current_BxDFCol_Dot) {
    bool is_BVH = true;

    float light_PDF, BxDF_PDF; //probability distribution functions for light and material
    float light_importance, bxdf_importance;
    glm::vec3 light_incoming, LTE_LIGHT, LTE_BxDF;

    r.origin = r.origin + 0.0001f * r.direction; // avoid shadow acne from floats
    Intersection isx;
    if(is_BVH) {
        isx = intersection_engine->IntersectionByBounds(r, intersection_engine->root);
    } else {
       isx = intersection_engine->GetIntersection(r);
    }

    // Sample a random point on a random light
    Geometry* rand_light = scene->lights.at(qrand() % scene->lights.size());
    Intersection sample_isx = rand_light->GetSamplePoint(isx.normal);

    // If object no object is hit then return black
    // If object hit is a light, return its texturecolor * basecolor
    if (isx.object_hit == NULL) {
        current_brdf_PDF = -1; // this ends the trace
        return glm::vec3(0.0f);
    } else if (isx.object_hit->material->is_light_source) {
        current_brdf_PDF = -1; // this ends the trace
        return isx.texture_color * isx.object_hit->material->base_color;

    }

    // RAY CONVENTION
    // All rays are pointing away from the geometry
    // wj = Ray from geometry to the light
    glm::vec3 wj_dir = glm::normalize(sample_isx.point - isx.point);
    Ray wj = Ray(isx.point + wj_dir * 0.0001f, wj_dir);
    // wo is the ray from the object to the camera in the first iteration
    // wo is otherwise the old wj reversed
    glm::vec3 wo = -r.direction;

    // get material properties of intersection
    BxDF* BxDF_RAND = isx.object_hit->material->GetRandomBxDF();


    /**
      * Light Sampling
      **/

    // Direct lighting test
    Intersection obs_isx;
    if(is_BVH) {
        obs_isx = intersection_engine->IntersectionByBounds(Ray(wj.origin + 0.001f * wj.direction, wj.direction), intersection_engine->root);
    } else {
        obs_isx = intersection_engine->GetIntersection(Ray(wj.origin + 0.001f * wj.direction, wj.direction));
    }
    if (obs_isx.object_hit != rand_light) {
        light_incoming = glm::vec3(0.0f); // radiance
        light_PDF = 0.f;
    } else { // hit rand_light
        // Calculate the LightESE
        light_incoming = rand_light->material->EvaluateScatteredEnergy(sample_isx, wo, -wj.direction, BxDF_RAND);

        // light_PDF tells us if the light hit it
        light_PDF = obs_isx.object_hit->RayPDF(obs_isx, wj);

        // if no light hit it return 0
        if (light_PDF <= 0.0001f) {
            light_PDF = 0.f;
            light_incoming = glm::vec3(0.0f);
        }
    }

    // Convert to Normal Space to pass into the PDF function for BxDF
    // Refactor?
    glm::mat3 T = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
    T = glm::transpose(T);
    glm::vec3 pdf_wi = T * wj.direction;
    glm::vec3 pdf_wo = T * wo;

    // Calculate the BxDF_PDF
    BxDF_PDF = BxDF_RAND->PDF(pdf_wo, pdf_wi);

    float dot_wj_nor = glm::abs(glm::dot(wj.direction, isx.normal));

    // Calculate weighting for the materials.
    glm::vec3 BRDF_color = isx.object_hit->material->EvaluateScatteredEnergy(isx, wo, wj.direction, BxDF_RAND);

    // First Half of the Equation
    if (fequal(light_PDF, 0.f)) {
        LTE_LIGHT = glm::vec3(0.0f);

        // Set MIS to 0 mostly because if BxDF_PDF is also 0 then it would be NaN
        // If light_PDF is 0, then MIS should be zero anyway
        light_importance = 0.f;
    } else {
        LTE_LIGHT = BRDF_color * light_incoming * dot_wj_nor / light_PDF;
        light_importance = light_PDF * light_PDF / (light_PDF * light_PDF + BxDF_PDF * BxDF_PDF);
    }

    // Sample a ray from the bxdf and get the color for indirect lighting
    BRDF_color = isx.object_hit->material->SampleAndEvaluateScatteredEnergy(isx, wo, wj.direction, BxDF_PDF, BxDF_RAND);
    if (BxDF_PDF <= 0.f) {
        BRDF_color = glm::vec3(0.f);
    }
    dot_wj_nor = glm::abs(glm::dot(wj.direction, isx.normal));

    if (fequal(BxDF_PDF, 0.0f)) {
        LTE_BxDF = glm::vec3(0.0f);

        // Set MIS to 0 mostly because if BxDF_PDF is also 0 then it would be NaN
        // If BxDF_PDF is 0, then MIS should be zero anyway
        bxdf_importance = 0.f;
    } else {
        LTE_BxDF = BRDF_color * light_incoming * dot_wj_nor / BxDF_PDF;
        bxdf_importance = BxDF_PDF * BxDF_PDF / (light_PDF * light_PDF + BxDF_PDF * BxDF_PDF);
    }


    // Multiple importance sampling for returned luminosity
    glm::vec3 LTE = light_importance * LTE_LIGHT + bxdf_importance * LTE_BxDF;

    // Update references for continuing along path
    r = wj; // next path ray
    current_brdf_PDF = BxDF_PDF; // for roulette weight
    current_BxDFCol_Dot = BRDF_color * dot_wj_nor; // for weighting color of next iterations

    return LTE;
}


// Treating Ray r as the ray from the camera
glm::vec3 Integrator::DirectLightingIntegratorTraceRayWithClouds(Ray &r, unsigned int depth, float &current_brdf_PDF, glm::vec3 &current_BxDFCol_Dot) {
    // Declaring Party
    float light_PDF, BxDF_PDF;
    float light_importance, bxdf_importance;
    glm::vec3 light_incoming, LTE_LIGHT, LTE_BxDF;

    // Get intersection from Ray r
    Intersection isx = intersection_engine->GetIntersection(Ray(r.origin + 0.0001f * r.direction, r.direction));

    // Random Light
    Geometry* rand_light = scene->lights.at(qrand() % scene->lights.size());

    // Random point on light
    Intersection sample_isx = rand_light->GetSamplePoint(isx.normal);

    // Cloud Lighting
    glm::vec3 cloud_colorLS = glm::vec3(0.f);
    QList<Geometry*> skip;
    float cloud_opacityLS = 0.f;
    if (isx.object_hit != NULL && isx.object_hit->material->is_cloud) {
        cloud_colorLS = CloudVisibility(isx, sample_isx, r, cloud_opacityLS, skip);
    }

    // If object no object is hit then return black
    // If object hit is a light, return its texturecolor * basecolor
    if (isx.object_hit == NULL) {
        current_brdf_PDF = -1; // this ends the trace
        return glm::vec3(0.0f);
    } else if (isx.object_hit->material->is_light_source) {
        if (depth > 0) {
            current_brdf_PDF = -1; // this ends the trace
            return glm::vec3(0.f);
        } else {
            current_brdf_PDF = -1; // this ends the trace
            return isx.texture_color * isx.object_hit->material->base_color;
        }
    }

    // RAY CONVENTION
    // All rays are pointing away from the geometry
    // wj = Ray from geometry to the light
    glm::vec3 wj_dir = glm::normalize(sample_isx.point - isx.point);
    Ray wj = Ray(isx.point + wj_dir * 0.0001f, wj_dir);
    // wo is the ray from the object to the camera in the first iteration
    // wo is otherwise the old wj reversed
    glm::vec3 wo = -r.direction;

    // Get a random BxDF which will be passed into all ESE formulas as a pointer to keep the BxDF consistant
    // In a regular material, this is just the first BxDF on the QList as there are no other BxDFs
    // However, in a weighted material, this will help with consistancy
    BxDF* BxDF_RAND = isx.object_hit->material->GetRandomBxDF();


    /**
      * Light Sampling
      **/

    // Shadow Testing
    // Check if wj is obstructed, if it return 0
    Intersection obs_isx = intersection_engine->GetIntersectionSkipThis(Ray(wj.origin + 0.001f * wj.direction, wj.direction), skip);
    if (obs_isx.object_hit != rand_light) {
        light_incoming = glm::vec3(0.0f); // radiance
        light_PDF = 0.f;
    } else { // hit rand_light
        // Calculate the LightESE
        light_incoming = rand_light->material->EvaluateScatteredEnergy(sample_isx, wo, -wj.direction, BxDF_RAND);

        // light_PDF tells us if the light hit it
        light_PDF = obs_isx.object_hit->RayPDF(obs_isx, wj);

        // if no light hit it return 0
        if (light_PDF <= 0.0001f) {
            light_PDF = 0.f;
            light_incoming = glm::vec3(0.0f);
        }
    }

    // Convert to Normal Space to pass into the PDF function for BxDF
    // Refactor?
    glm::mat3 T = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
    T = glm::transpose(T);
    glm::vec3 pdf_wi = T * wj.direction;
    glm::vec3 pdf_wo = T * wo;

    // Calculate the BxDF_PDF
    BxDF_PDF = BxDF_RAND->PDF(pdf_wo, pdf_wi);

    float dot_wj_nor = glm::abs(glm::dot(wj.direction, isx.normal));

    // Calculate weighting for the materials.
    glm::vec3 BRDF_color = isx.object_hit->material->EvaluateScatteredEnergy(isx, wo, wj.direction, BxDF_RAND);

    // First Half of the Equation
    if (fequal(light_PDF, 0.f)) {
        LTE_LIGHT = glm::vec3(0.0f);

        // Set MIS to 0 mostly because if BxDF_PDF is also 0 then it would be NaN
        // If light_PDF is 0, then MIS should be zero anyway
        light_importance = 0.f;
    } else {
        LTE_LIGHT = BRDF_color * light_incoming * dot_wj_nor / light_PDF;
        light_importance = light_PDF * light_PDF / (light_PDF * light_PDF + BxDF_PDF * BxDF_PDF);
    }


    /**
     * BxDF Sampling
     **/
    // Generate the new ray wj
    // Calculate BRDF_color from that ray
    BRDF_color = isx.object_hit->material->SampleAndEvaluateScatteredEnergy(isx, wo, wj.direction, BxDF_PDF, BxDF_RAND);

//    // Cloud Lighting
    glm::vec3 cloud_colorBS = glm::vec3(0.f);
    skip.empty();
    float cloud_opacityBS = 0.f;

    if (isx.object_hit != NULL && isx.object_hit->material->is_cloud) {
        cloud_colorBS = CloudVisibility(isx, sample_isx, r, cloud_opacityBS, skip);
    }

    // Shadow Test
    obs_isx = intersection_engine->GetIntersectionSkipThis(Ray(wj.origin + 0.001f * wj.direction, wj.direction), skip);
    if (obs_isx.object_hit != rand_light) {
        light_incoming = glm::vec3(0.0f); // radiance
        light_PDF = 0.f;
    } else {
        light_incoming = rand_light->material->EvaluateScatteredEnergy(obs_isx, wo, -wj.direction, BxDF_RAND);

         // light_PDF tells us if the light hit it
        light_PDF = obs_isx.object_hit->RayPDF(obs_isx, wj);
        if (light_PDF < 0.0001f) {
            light_PDF = 0.f;
        }
    }

    dot_wj_nor = glm::abs(glm::dot(wj.direction, isx.normal));

    if (fequal(BxDF_PDF, 0.0f)) {
        LTE_BxDF = glm::vec3(0.0f);

        // Set MIS to 0 mostly because if BxDF_PDF is also 0 then it would be NaN
        // If BxDF_PDF is 0, then MIS should be zero anyway
        bxdf_importance = 0.f;
    } else {
        LTE_BxDF = BRDF_color * light_incoming * dot_wj_nor / BxDF_PDF;
        bxdf_importance = BxDF_PDF * BxDF_PDF / (light_PDF * light_PDF + BxDF_PDF * BxDF_PDF);
    }

    /** --------------------------------------
     *  QUIK Change this to see other LTE outcomes
     *  LTE_LIGHT = first half of the equation with the light
     *  LTE_BxDF = second half of the equation with the BxDF
     *  light_importance = MIS for LTE_LIGHT
     *  bxdf_importance = MIS for LTE_BxDF
     * ---------------------------------------
     */
    // Set up actual return
//    glm::vec3 LTE = light_importance * LTE_LIGHT +  bxdf_importance * LTE_BxDF;
    glm::vec3 LTE = cloud_colorLS + (1.f-cloud_opacityLS) * (light_importance * LTE_LIGHT)
                    + cloud_colorBS + (1.f-cloud_opacityBS) * (bxdf_importance * LTE_BxDF);

    // Return for indirect lighting
    r = wj; // isx point is already updated from above
    current_brdf_PDF = BxDF_PDF;
    current_BxDFCol_Dot = BRDF_color * dot_wj_nor;

    // Actual Return
    return LTE;
}

glm::vec3 Integrator::CloudVisibility(Intersection &isx, Intersection sample_isx, Ray r, float &opacity, QList<Geometry*> &skip) {
    glm::vec3 cloud_color = glm::vec3(0.f);
    float visibility = 0.f;
    float density = 0.f;
    float step = 0.05f;

    // Second raycast to get to other side of the cloud
    Intersection cloudEdgeR = intersection_engine->GetIntersection(Ray(isx.point + 0.0001f * r.direction, r.direction));

    // Ray marching
    // Assuming we don't start in a cloud
    // Start and end of a cloud
    glm::vec3 startR = isx.point + 0.0001f * r.direction;
    glm::vec3 endR = cloudEdgeR.point - 0.001f * r.direction;

    // Step through the cloud
    float distR = glm::distance(startR, endR);
    glm::vec3 cloud_pt;
    for (float i = 0.f; i < distR; i += step) {
       float densityL = 0.f;

        // Obtain point of the cloud you're in
        cloud_pt = startR + i * step * r.direction;

        // wc = From cloud to the light
        glm::vec3 wc_dir = glm::normalize(sample_isx.point - cloud_pt);
        Ray wc = Ray(cloud_pt + wc_dir * 0.0001f, wc_dir);

        // Ray March to the light
        Intersection cloudEdgeL = intersection_engine->GetIntersection(wc);
        glm::vec3 endL = cloudEdgeL.point - 0.001f * wc.direction;

        // Convert to Normal Space to pass into the PDF function for BxDF
        // Refactor?
        glm::mat3 Tc = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
        Tc = glm::transpose(Tc);

        float distL = glm::distance(cloud_pt, endL);
        for (float j = 0.f; j < distL; j += step) {
            if (cloudEdgeL.object_hit != isx.object_hit) {
                break;
            }
            glm::vec3 cloud_pt_l = cloud_pt + j * step * wc.direction;
            densityL += isx.object_hit->vox->getDensity(isx.object_hit->transform.invT(), cloud_pt_l);
        }
        densityL /= (distL / step);
        visibility += densityL;

        // Update density through cloud
        density += isx.object_hit->vox->getDensity(isx.object_hit->transform.invT(), cloud_pt);
    }
    visibility /= (distR / step);
    density /= (distR/step);
    visibility = glm::clamp(visibility, 0.f, 1.f);
    if (visibility < 0.f) {
        visibility = 0.f;
    }
    if (visibility > 1.0f) {
        visibility = 1.0f;
    }

    cloud_color = (visibility) * sample_isx.object_hit->material->base_color
                  + (1.f-visibility) * isx.object_hit->material->base_color;
    cloud_color = glm::clamp(cloud_color, glm::vec3(0.f), glm::vec3(1.f));

    // Continue ray after cloud
    isx = intersection_engine->GetIntersection(Ray(cloudEdgeR.point + 0.0001f * r.direction, r.direction));

    if (isx.object_hit != NULL && isx.object_hit->material->is_cloud) {
        // break point here though
        float temp = 0.f;
        cloud_color += CloudVisibility(isx, sample_isx, r, temp, skip); // INCORRECT
        opacity = glm::clamp(opacity + temp, 0.f, 1.f);
    } else {
        opacity = visibility;
        return cloud_color;
    }
}
