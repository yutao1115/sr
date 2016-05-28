#pragma once
#include "RasTrapezoid.h"
#include "Mesh.h"
#include "VertexAttr.h"
#include "AlgoManager.h"



/* cliped vertices */
class alignas(16) VerticesAssembly{
public:
    static constexpr int NUM          = 8;
    static constexpr int TRIANGLECLIP = 3;
    static constexpr int TRAPEZOIDCLIP= 4;

 
private:
    using VASemantic = VertexAttr::VASemantic;
    Float4 new1[VASemantic::NUM_SEMANTICS];
    Float4 new2[VASemantic::NUM_SEMANTICS];

    ClonedVertex data[NUM];
    int         order[NUM];
    int           num;
    Mesh* pvt;
    VertexAttr*   attr;
    bool          hasOtherChannels;
    
 public:
    Float4* indexOf(int n){assert(n>=0 && n<NUM); return &(data[n].coord);}
    Float4* orderOf(int n){assert(n>=0 && n<NUM); return &(data[order[n]].coord);}
    ClonedVertex* orderV(int n){assert(n>=0&&n<NUM); return &data[order[n]];}
    void init(Mesh*vt){
        pvt=vt; attr = vt->va_.get(); num=0;
        hasOtherChannels = (attr->channels()>4);
        for(int i=0;i<NUM;++i){order[i]=i;}
    }
    void clone(Float4*inptr,int inx){
        ClonedVertex& c = data[inx];
        c.coord = *inptr;
        c.leftChannels= hasOtherChannels?(float*)(inptr+1):nullptr;
        c.state=ClonedVertex::CLIP;
        ++num;}

    void newVertex1(int srcInx){
        auto& src = data[order[srcInx]];
        new1[0] = src.coord;
        memcpy(new1+1,src.leftChannels,attr->bytes() - sizeof(Float4));
        ClonedVertex& c = data[TRIANGLECLIP];
        c.coord = src.coord;
        c.leftChannels = hasOtherChannels?(float*)(new1+1):nullptr;
        c.state=src.state;
        ++num;
    }

    void newVertex2(ClonedVertex* src){
        new2[0] = src->coord;
        memcpy(new2+1,src->leftChannels,attr->bytes()-sizeof(Float4));
        ClonedVertex& c = data[TRAPEZOIDCLIP];
        c.coord = src->coord;
        c.leftChannels = hasOtherChannels?(float*)(new2+1):nullptr;;
        c.state=src->state;
        ++num;
    }

    void sort(int i1,int i2,int i3){order[0]=i1,order[1]=i2,order[2]=i3;}

    void intersectionPoint(int result,int st,int ed,float znear){
        Float4* p1=orderOf(st);Float4* p2=orderOf(ed);
        float diffw = (p2->w - p1->w);
        if(diffw<LowEpsilon) return ;// avoid div by zero
        float ratio = (znear - p1->w) /diffw;
        intersectionPoint(data[order[result]],data[order[st]],data[order[ed]],ratio);
    }

    void intersectionPoint(ClonedVertex& result,ClonedVertex& st,ClonedVertex& ed,float ratio){
        result.coord = lerp(st.coord,ed.coord,ratio);
        if(hasOtherChannels){
            Interpolat& f = *(AlgoManager<Interpolat>::instance().get(pvt));
            f(result.leftChannels,st.leftChannels,ed.leftChannels,ratio);
        }
    }
};

