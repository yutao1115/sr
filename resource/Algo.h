#pragma once

#include "VertexAttr.h"
#include "Vectors.h"
#include "FrameBuffer.h"
#include "RasTrapezoid.h"
#include <functional>

enum RasAlgo{SCANLINE_ALGO,TILE_ALGO,WIREFRAME_ALGO,NUM_ALGO};



using Interpolat     = std::function<void(float* result,float*v1,float*v2,float ratio)>;
using LineInit       = std::function<void(float* result,float*v1,float*v2,float ratio)>;
using PerspectiveDiv = std::function<void(float*,float invw)>;
using StepAdd        = std::function<void(float* result,float*step)>;
using PixelColor     = std::function<void(ClonedVertex& v,float,uint32_t*)>;


struct IBase{
    Interpolat     interpolat_;
    PerspectiveDiv perspectiveDiv_;
};
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
 
        lhs.leftChannels   = (float*)(l_+1);
        rhs.leftChannels   = (float*)(r_+1);
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

            float* zbufLine   = f.zBufferRow(y);
            uint32_t* bufLIne = f.bufferRow(y);
            
            for (; iterWidth > 0; ++iterx, iterWidth--) {
                if (iterWidth >= 0 && iterWidth < f.width()) {
                    float invW = result.invW;
                    if (invW >= zbufLine[iterx]) 
                    {
                        /*
                         其他通道 由 invW 恢复到正常值 1.0f/result.invW
                        */                    
                        algo_.pixelColor_(result,1.0f/result.invW,bufLIne+iterx);
                        /*
                         更新Z缓冲 优化: 读取zbufLine[iterx] 后执行一些代码再更新
                        */
                        zbufLine[iterx] = invW;
                    }
                }
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
        // left start point 
        /*
          都已经是 invW = 1/Z , 直接线性插值
        */
        float k = (y - t_.lEdgeP1->coord.y) / (t_.lEdgeP2->coord.y - t_.lEdgeP1->coord.y);
        lhs.coord = lerp(t_.lEdgeP1->coord,t_.lEdgeP2->coord,k);
        lhs.invW  = lerp(t_.lEdgeP1->invW,t_.lEdgeP2->invW,k);
        lerp_(lhs.leftChannels,t_.lEdgeP1->leftChannels,t_.lEdgeP2->leftChannels,k);
        
        // right end  point
              k = (y - t_.rEdgeP1->coord.y) / (t_.rEdgeP2->coord.y - t_.rEdgeP1->coord.y);
        rhs.coord = lerp(t_.rEdgeP1->coord,t_.rEdgeP2->coord,k);
        rhs.invW = lerp(t_.rEdgeP1->invW,t_.rEdgeP2->invW,k);
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
 
