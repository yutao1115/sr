#pragma once

#include "VertexAttr.h"
#include "Vectors.h"
#include "FrameBuffer.h"
#include "RasTrapezoid.h"
#include <functional>

enum RasAlgo{SCANLINE_ALGO,TILE_ALGO,WIREFRAME_ALGO,NUM_ALGO};



using Interpolat = std::function<void(void* result,void*v1,void*v2,float ratio)>;
using LineInit   = std::function<void(void* result,void*v1,void*v2,float ratio)>;
using StepAdd    = std::function<void(void* result,void*step)>;
using PixelColor = std::function<void(ClonedVertex& v,uint32_t*)>;

struct IScanline{
    LineInit   lineInit_;
    StepAdd    stepAdd_;
    PixelColor pixelColor_;
};

class  alignas(16) ScanlineAlgo{
    static constexpr int SCANLINELEFT = 0;
    static constexpr int SCANLINERIGHT= 1;
    static constexpr int SCANLINESTEP = 2;
    static constexpr int NUM          = 3;
    using VASemantic = VertexAttr::VASemantic;
    Float4 l_[VASemantic::NUM_SEMANTICS];
    Float4 r_[VASemantic::NUM_SEMANTICS];
    Float4 s_[VASemantic::NUM_SEMANTICS];

    ClonedVertex data[NUM];
    const RasTrapezoid& t_;
    IScanline&  algo_;
    Interpolat& lerp_;
    
public: 
    ScanlineAlgo(const RasTrapezoid& t,FrameBuffer& f,IScanline& algo,Interpolat& lerp)
    :t_(t),algo_(algo),lerp_(lerp)
    {
        ClonedVertex& lhs = data[SCANLINELEFT];
        ClonedVertex& rhs = data[SCANLINERIGHT];
        ClonedVertex& result = data[SCANLINELEFT];
        ClonedVertex& step   = data[SCANLINESTEP];
 
        lhs.leftChannels  = (float*)(l_+1);
        rhs.leftChannels = (float*)(r_+1);
        step.leftChannels  = (float*)(s_+1);
        
        int top    = (int)(t.top    + 0.5f);
        int bottom = (int)(t.bottom + 0.5f);
        

        
        /* 按边插值 每次+1 y坐标*/
        for(int y=top;y<bottom;++y){
           if( y>0 && y< f.height() && 
              nextLine((float)y+0.5f,lhs,rhs) )
          {
          	int iterx = (int)(lhs.coord.x + 0.5f);
	        int iterWidth  = (int)(rhs.coord.x + 0.5f) - iterx;
            
            if (iterWidth >= 200)
            {
                printf("width:%d\r\n", iterWidth);
            }

            float* zbufLine   = f.zBufferRow(y);
            uint32_t* bufLIne = f.bufferRow(y);
            
            for (; iterWidth > 0; ++iterx, iterWidth--) {
                if (iterWidth >= 0 && iterWidth < f.width()) {
                    float invW = lhs.invW;
                    //FIXME
                    //if (invW >= zbufLine[iterx]) {
                        zbufLine[iterx] = invW;
                        algo_.pixelColor_(result,bufLIne+iterx);
                    //}
                }
                //addStep();
                result.coord  += step.coord;
                result.invW   += step.invW;
                algo_.stepAdd_(result.leftChannels,step.leftChannels);
                if (iterWidth >= f.width()) break;    
            }
          }
        }
    }
    
private:
    bool nextLine(float y,ClonedVertex& lhs,ClonedVertex& rhs){
        float k = (y - t_.lEdgeP1->coord.y) / (t_.lEdgeP2->coord.y - t_.lEdgeP1->coord.y);
        lhs.coord = lerp(t_.lEdgeP1->coord,t_.lEdgeP2->coord,k);
        lerp_(lhs.leftChannels,t_.lEdgeP1->leftChannels,t_.lEdgeP2->leftChannels,k);
        
              k = (y - t_.rEdgeP1->coord.y) / (t_.rEdgeP2->coord.y - t_.rEdgeP1->coord.y);
        rhs.coord = lerp(t_.rEdgeP1->coord,t_.rEdgeP2->coord,k);
        lerp_(rhs.leftChannels,t_.rEdgeP1->leftChannels,t_.rEdgeP2->leftChannels,k);      

        if(rhs.coord.x < lhs.coord.x) 
            return false;

        //step
        float xdiff    = rhs.coord.x - lhs.coord.x;
        float invWidth = 1.0f/xdiff;
        ClonedVertex& step= data[SCANLINESTEP];
        step.coord = (rhs.coord - lhs.coord) * invWidth;
        step.invW  = (rhs.invW  - lhs.invW)  * invWidth;
        algo_.lineInit_(step.leftChannels,lhs.leftChannels,rhs.leftChannels,invWidth);
         
        return true;
    }
    
};
 
