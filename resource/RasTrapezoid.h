#pragma once
#include "ClonedVertex.h"

/* ¹âÕ¤»¯ÌÝÐÎ for raster 
*/
struct RasTrapezoid{
    ClonedVertex* lEdgeP1 ,*lEdgeP2;
    ClonedVertex* rEdgeP1 ,*rEdgeP2;
    float top, bottom;
};
