#pragma once

#include "Algo.h"



class Mesh;

/* cliped vertices */
class alignas(16) VerticesAssembly{
public:
    static constexpr int NUM          = 4;
    static constexpr int TRIANGLECLIP = 3;
    static constexpr bool NOTRICLIP   = false;
    static constexpr bool TRICLIPED   = true;

 
private:
    using VASemantic = VertexAttr::VASemantic;
    Float4 tri[4][VASemantic::NUM_SEMANTICS];
    ClonedVertex data[NUM];
    int         order[NUM];
    int           num;
    VertexAttr*   attr;
    bool          hasOtherChannels;
    IBase*        base;
    
 public:
    Float4* indexOf(int n){assert(n>=0 && n<NUM); return &(data[n].coord);}
    Float4* orderOf(int n){assert(n>=0 && n<NUM); return &(data[order[n]].coord);}
    ClonedVertex* orderV(int n){assert(n>=0&&n<NUM); return &data[order[n]];}

    void init(Mesh*vt);
    
    void reset(void){
        num=0;
        for(int i=0;i<NUM;++i){
            order[i]=i;
            data[i].state=ClonedVertex::INVALID;
        }
    }
    
    void clone(Float4*inptr,int inx){
        Float4* d = tri[inx];
        ClonedVertex& c = data[inx];
        c.coord = *inptr; *d=*inptr;
        if(hasOtherChannels){
            memcpy(d+1,inptr+1,attr->bytes() - sizeof(Float4));
            c.leftChannels= (float*)(d+1);
        }
        c.state=ClonedVertex::CLIP;
        ++num;
    }

    void newVertex(int srcInx){
        clone(tri[order[srcInx]],3);
    }

    void sort(int i1,int i2,int i3){order[0]=i1,order[1]=i2,order[2]=i3;}

    void intersectionPoint(int result,int st,int ed,float znear){
        Float4* p1=orderOf(st);Float4* p2=orderOf(ed);
        float diffw = p2->w - p1->w;
        if(fabs(diffw)<LowEpsilon) return ;// avoid div by zero
        float ratio = (znear - p1->w) /diffw;
        intersectionPoint(data[order[result]],data[order[st]],data[order[ed]],ratio);
    }

    void intersectionPoint(ClonedVertex& result,ClonedVertex& st,ClonedVertex& ed,float ratio){
        result.coord = lerp(st.coord,ed.coord,ratio);
        if(hasOtherChannels)
            base->interpolat_(result.leftChannels,st.leftChannels,ed.leftChannels,ratio);
        
    }
    
    void toScreen(int i,const Matrix44& m){
        ClonedVertex&j = data[i];
        if(j.toScreen(m) && base->perspectiveDiv_) 
            base->perspectiveDiv_(j.leftChannels,j.invW);
    }
    
    void toScreen(const Matrix44& m,bool hasClip){
        toScreen(0,m);
        toScreen(1,m);
        toScreen(2,m);
        if(hasClip) toScreen(3,m);
    }
};

