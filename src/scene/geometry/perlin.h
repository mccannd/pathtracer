#pragma once
#include <la.h>

class perlin
{
public:
    perlin();
    static float noise(float x, float y, float z, float n);
    static float pbrt_lerp(float t, float v1, float v2);

};

float Grad(int ix, int iy, int iz, float dx, float dy, float dz);
float NoiseWeight(float t);
