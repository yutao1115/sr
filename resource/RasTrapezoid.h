#pragma once
#include "ClonedVertex.h"

/* ��դ������ for raster 
*/
struct RasTrapezoid{
    ClonedVertex* lEdgeP1 ,*lEdgeP2;
    ClonedVertex* rEdgeP1 ,*rEdgeP2;
    float top, bottom;
};
