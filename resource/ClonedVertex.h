#pragma once
#include "Matrix.h"

struct alignas(16) ClonedVertex{
    Float4 coord;
    float* leftChannels;
    float  invW;
    int    state = INVALID;

    bool toScreen(const Matrix44& m){
        if(state == CLIP) {
            // clip coordinate TO NDC
            invW = 1.0f/coord.w;
            coord.x = coord.x * invW;
            coord.y = coord.y * invW;
            coord.z = coord.z * invW;

            // NDC to Screen
            float w = coord.w;
            coord.w = 1.0f;
            coord = m * coord;
            coord.w = w;
            state   = SCREEN;
            return true;
        }
        return false;
    }

    static constexpr int CLIP     =0;
    static constexpr int SCREEN   =2;
    static constexpr int INVALID  =3;
};

