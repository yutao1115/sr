#pragma once

#include "FrameBuffer.h"
#include "ViewFrustum.h"
#include "Transform.h"
#include "CameraRig.h"
#include "Mesh.h"
#include "ResMemory.h"
#include "VerticesAssembly.h"
#include "Algo.h"


class PipeLine{
public:


    struct Uniforms{
        Matrix44     wpvMatrix_;
        Matrix44     viewPort_;
        //Matrix44     worldMatrix_;
        //Matrix44     viewMatrix_;
        //Matrix44     projectionMatrix_;
        //Matrix44     pvMatrix_;
        //Matrix44     invWorldMatrix_;
    };

    struct Registers{
        Registers(int regsN)
            :regs_(1,sizeof(Uniforms),regsN*sizeof(float)),
             head_((float*)regs_.extra()),
             regNum_(regsN){}

        void set(int inx,float v){
            head_[inx]=v;
        }
        float get(int inx){
            return head_[inx];
        }
        void set(int inx,float* value,int numOfFloat){
            memcpy(head_+inx,value,sizeof(float)*numOfFloat);
        }

        void get(int inx,int numOfFloat,float* result){
            memcpy(result,head_+inx,sizeof(float)*numOfFloat);
        }

        SFloat16Pool regs_;
        float*       head_;
        const int    regNum_;
    };



    using VertexShader =
     std::function<void(Mesh*,const Uniforms*,Registers&)>;

    //using PixelShader  =
    //    std::function<void(const Uniforms*,Registers&,VerticesAssembly&,const Trapezoid*,FrameBuffer&)>;

    PipeLine(FrameBuffer& fbo,int registerNum);
    void attachMesh(Mesh*p){meshs_.push_back(p);}
    void setViewPort(int w,int h,int srcx=0,int srcy=0);
    void outlet(void);

    void setRasterizeAlgo(RasAlgo a) { algo_ = a; }
    //cull state
    enum FrontMode {FT_CCW,FT_CW};
    enum CullMode  {CT_FRONT,CT_BACK};
    struct CullState{
        int frontMode = FT_CCW;
        int cullMode  = CT_BACK;
    };
    CullState cullState_;
    bool cullStateEnabled_ = true;
    inline void setCullState(FrontMode fr,CullMode cm);


    /* 输入参数是剪裁空间坐标
       透视除法后视线与三角形夹角变为0度,平行
	*/
    bool backFaceCull(const Float4& clipP0,const Float4& clipP1,const Float4& clipP2){
        Float3 p0p1 = Float3{clipP0.x,clipP0.y,clipP0.z} / clipP0.w - Float3{clipP1.x,clipP1.y,clipP1.z} / clipP1.w;
		Float3 p1p2 = Float3{clipP1.x,clipP1.y,clipP1.z} / clipP1.w - Float3{clipP2.x,clipP2.y,clipP2.z} / clipP2.w;
        p0p1.z = 0;
		p1p2.z = 0;
		Float3 normal = p0p1.cross(p1p2);
        return normal.z  * cullSignAdjust_ <=0;
    }

    /* Vector 三重积
     result = (E-P1) DOT (P2-P1) CROSS (P3-P1)
     result > 0 front face
            <=0 back  face
	*/
    bool backFaceCullInCameraSpace(const Float4& p1,const Float4& p2,const Float4& p3){
        Float3 pp1{p1.x,p1.y,p1.z},pp2{p2.x,p2.y,p2.z},pp3{p3.x,p3.y,p3.z};
        Float3 vNormal=(pp2-pp1).cross(pp3-pp1);
        return vNormal.dot(pp1) * cullSignAdjust_ <= 0;
    }

    void setVertexShader(const VertexShader& vs){vShader_=vs;}
    void scanlineAlgo(const RasTrapezoid* trap);

    CameraRig& getCamera(void){return cam_;}
    Transform& getTransform(void){return worldTransform_;}

private:

    void drawPolypoint();
    void drawPolylineDisjoint();
    void drawPolylineContiguous();
    void drawTriMesh();

    void drawLine(int x1,int y1,int x2,int y2,uint32_t c,uint32_t depth=0);
    int  clipTriangleInClipSpace(void);
    bool clipLine(int &x1,int &y1,int &x2, int &y2);
    void rasterizeTri(int i1,int i2,int i3);
    void renderTrapezoid(const RasTrapezoid* trap);
    void trapezoidization(ClonedVertex*f1,ClonedVertex*f2,ClonedVertex*f3);

    void fillPoint(int x,int y,uint32_t color,uint32_t depth=0){
        fbo_.set(x,y,color,depth);
    }


    FrameBuffer&       fbo_;
    VertexShader       vShader_;
    //PixelShader        pShader_;
    Registers          registers_;
    Uniforms*          uniforms_;
    Transform          worldTransform_;
    ViewFrustum        viewfrustum_;
    CameraRig          cam_;
    std::vector<Mesh*> meshs_;
    int                cullSignAdjust_=-1;
    bool               reverseCullFace_=false;
    Mesh*              curMesh_;

    //static void defalutInterpolat(void*,void*,void*,float){}
    typedef void (PipeLine::*DrawFunction)();
    DrawFunction drawFunction_[Mesh::PrimitiveT::TRISTRIP+1];

    int  algo_=RasAlgo::WIREFRAME_ALGO;
    typedef void (PipeLine::*RasterAlgo)(const RasTrapezoid* trap);
    RasterAlgo rasterAlgo_[RasAlgo::NUM_ALGO];
};


inline void PipeLine::setCullState(FrontMode fr,CullMode cm){
    if(cullStateEnabled_){
        cullState_.frontMode = fr;cullState_.cullMode=cm;
        if ( fr == FT_CCW) {
            if (cm == CT_BACK) cullSignAdjust_= -1;
            else               cullSignAdjust_= +1;
        }
        else {
            if (cm == CT_BACK) cullSignAdjust_= +1;
            else               cullSignAdjust_= -1;
        }
        if (reverseCullFace_) cullSignAdjust_=-cullSignAdjust_;
    }
    else cullSignAdjust_ = 0;
}
 

