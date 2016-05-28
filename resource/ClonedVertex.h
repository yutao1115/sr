#pragma once
#include "Matrix.h"

struct alignas(16) ClonedVertex{
    Float4 coord;
    float* leftChannels;
    float  invW;
    int    state = CLIP;

    void toScreen(const Matrix44& m){
        if(state == CLIP) {
            // clip coordinate TO NDC
            float invZ = 1.0f/coord.w;
            coord.x = coord.x * invZ;
            coord.y = coord.y * invZ;
            coord.z = coord.z * invZ;
            invW    = invZ;
            // NDC to Screen
            float w = coord.w;
            coord.w = 1.0f;
            coord = m * coord;
            coord.w = w;
            state   = SCREEN;
        }
    }

    static constexpr int CLIP     =0;
    static constexpr int SCREEN   =2;
    static constexpr int INVALID  =3;
};

