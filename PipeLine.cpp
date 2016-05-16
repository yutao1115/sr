#include "PipeLine.h"
#include "Mesh.h"
using VASemantic = VertexAttr::VASemantic;

// 光栅化梯形
struct Trapezoid{
    Float4* lEdgeP1 ,lEdgeP2;
    Float4* rEdgeP1 ,rEdgeP2;
    float top, bottom;
};

// 用于剪裁后的顶点数据
struct alignas(16) AssemblyVertices{
    static constexpr int NUM    = 5;
    Float4 new1[VASemantic::NUM_SEMANTICS];

    ClonedVertex data[NUM];
    int         order[NUM];
    int           num;

    Float4* indexOf(int n){assert(n>=0 && n<NUM); return &(data[n].coord);}
    Float4* orderOf(int n){assert(n>=0 && n<NUM); return &(data[order[n]].coord);}
    void setEmpty(void){num=0;for(int i=0;i<NUM;++i)order[i]=i;}
    void clone(Float4*inptr){data[num].coord = *inptr; data[num].leftChannels=(float*)(inptr+1);++num;}
    void newVertex1(int srcInx,uint32_t len){
        auto& src = data[order[srcInx]];
        new1[0] = src.coord;
        memcpy(new1+1,src.leftChannels,len - sizeof(Float4));
        data[3].coord = src.coord;
        data[3].leftChannels = (float*)(new1+1);
    }

    void sort(int i1,int i2,int i3){order[0]=i1,order[1]=i2,order[2]=i3;}

    void intersectionPoint(unsigned channels,int result,int inx1,int inx2 ,float znear){
        Float4* r = orderOf(result);Float4* p1=orderOf(inx1);Float4* p2=orderOf(inx2);
        float diffw = p2->w - p1->w;
        if(diffw<LowEpsilon) return ;// avoid div by zero
        float ratio = (znear - p1->w)/diffw;
        *r = lerp(*p1,*p2,ratio);
        float* fr  = data[order[result]].leftChannels;
        float* fp1 = data[order[inx1]].leftChannels;
        float* fp2 = data[order[inx2]].leftChannels;
        for(unsigned i=4;i<channels;++i){
            *fr = lerp(*fp1,*fp2,ratio);
            ++fr;++fp1;++fp2;
        }
    }

};

thread_local AssemblyVertices assemblyV;

/////////////////////////////////////////////////
// viewport transform                          //
//-----------------------------------------------
// 1. scale                  // 2. translate   //
//---------------------------//------------------
// W/2       0       0       //   minX + W/2   //
//  0       -H/2     0       //   minY + H/2   //
//  0        0    maxZ-minZ  //     minZ       //
//-----------------------------------------------
// W/2       0       0         minX+W/2        //
//  0       -H/2     0         minY+H/2        //
//  0        0    maxZ-minZ      minZ          //
//  0        0       0            1            //
//-----------------------------------------------
void PipeLine::setViewPort(int w,int h,int srcx/*0*/,int srcy/*0*/){
    float halfw = w *0.5f-0.5f;
    float halfh = h *0.5f-0.5f;
    uniforms_->viewPort_=Matrix44{
        halfw,  0  ,  0  , srcx+halfw,
        0,     -halfh, 0  , srcy+halfh,
        0,      0  ,  1  ,    0     ,
        0,      0  ,  0  ,    1      };
}


PipeLine::PipeLine(FrameBuffer& fbo,int regsN)
    :fbo_(fbo),registers_(regsN),
     uniforms_((Uniforms*)registers_.regs_.data()),
     worldTransform_(),
     viewfrustum_(),
     cam_(viewfrustum_,0.1,0.01)
{
    setCullState(FT_CCW,CT_BACK);
    viewfrustum_.setFrustum(90.0f,fbo.width(),fbo.height(),5,500);

    //viewfrustum_.setFrustum(-400,400,-300,300,50,500);
    setViewPort(fbo.width(),fbo.height(),0,0);
    uniforms_->wpvMatrix_ = viewfrustum_.getProjectionViewMatrix();
    using PT = Mesh::PrimitiveT;

    // FIXME
    //drawFunction_[PT::POLYPOINT] = &PipeLine::drawPolypoint;
    //drawFunction_[PT::POLYSEGMENT_DISJOINT] = &PipeLine::drawPolylineDisjoint;
    //drawFunction_[PT::POLYSEGMENT_CONTIGUOUS] = &PipeLine::drawPolylineContiguous;
    //drawFunction_[PT::TRISTRIP]=

    drawFunction_[PT::TRIMESH] = &PipeLine::drawTriMesh;
}

bool PipeLine::clipLine(int &x1,int &y1,int &x2, int &y2) {

    int min_clip_x = 0 , max_clip_x = fbo_.width();
    int min_clip_y = 0 , max_clip_y = fbo_.height();
    static constexpr uint8_t  CLIP_CODE_C = 0x00;
    static constexpr uint8_t  CLIP_CODE_N = 0x08;
    static constexpr uint8_t  CLIP_CODE_S = 0x04;
    static constexpr uint8_t  CLIP_CODE_E = 0x02;
    static constexpr uint8_t  CLIP_CODE_W = 0x01;

    static constexpr uint8_t  CLIP_CODE_NE= 0x0a;
    static constexpr uint8_t  CLIP_CODE_SE= 0x06;
    static constexpr uint8_t  CLIP_CODE_NW= 0x09;
    static constexpr uint8_t  CLIP_CODE_SW= 0x05;

    int xc1=x1, yc1=y1, xc2=x2, yc2=y2;
    int p1_code=0, p2_code=0;

    // determine codes for p1 and p2
    if (y1 < min_clip_y) p1_code|=CLIP_CODE_N;
    else if (y1 > max_clip_y) p1_code|=CLIP_CODE_S;

    if (x1 < min_clip_x) p1_code|=CLIP_CODE_W;
    else if (x1 > max_clip_x) p1_code|=CLIP_CODE_E;

    if (y2 < min_clip_y) p2_code|=CLIP_CODE_N;
    else if (y2 > max_clip_y) p2_code|=CLIP_CODE_S;

    if (x2 < min_clip_x) p2_code|=CLIP_CODE_W;
    else if (x2 > max_clip_x) p2_code|=CLIP_CODE_E;

    // try and trivially reject
    if ((p1_code & p2_code)) return false;

    // test for totally visible, if so leave points untouched
    if (p1_code==0 && p2_code==0) return true;

    // determine end clip point for p1
    switch(p1_code)
    {
    case CLIP_CODE_C: break;

    case CLIP_CODE_N:
    {
        yc1 = min_clip_y;
        xc1 = x1 + 0.5f +float((min_clip_y-y1)*(x2-x1))/float(y2-y1);
    } break;
    case CLIP_CODE_S:
    {
        yc1 = max_clip_y;
        xc1 = x1 + 0.5f +float((max_clip_y-y1)*(x2-x1))/float(y2-y1);
    } break;

    case CLIP_CODE_W:
    {
        xc1 = min_clip_x;
        yc1 = y1 + 0.5f +float((min_clip_x-x1)*(y2-y1))/float(x2-x1);
    } break;

    case CLIP_CODE_E:
    {
        xc1 = max_clip_x;
        yc1 = y1 + 0.5f +float((max_clip_x-x1)*(y2-y1))/float(x2-x1);
    } break;

    // these cases are more complex, must compute 2 intersections
    case CLIP_CODE_NE:
    {
        // north hline intersection
        yc1 = min_clip_y;
        xc1 = x1 + 0.5f +float((min_clip_y-y1)*(x2-x1))/float(y2-y1);
        // test if intersection is valid, of so then done, else compute next
        if (xc1 < min_clip_x || xc1 > max_clip_x)
        {
            // east vline intersection
            xc1 = max_clip_x;
            yc1 = y1 + 0.5f +float((max_clip_x-x1)*(y2-y1))/float(x2-x1);
        } // end if
    } break;

    case CLIP_CODE_SE:
    {
        // south hline intersection
        yc1 = max_clip_y;
        xc1 = x1 + 0.5f +float(max_clip_y-y1)*(x2-x1)/(y2-y1);
        // test if intersection is valid, of so then done, else compute next
        if (xc1 < min_clip_x || xc1 > max_clip_x)
        {
            // east vline intersection
            xc1 = max_clip_x;
            yc1 = y1 + 0.5f +float(max_clip_x-x1)*(y2-y1)/(x2-x1);
        } // end if

    } break;

    case CLIP_CODE_NW:
    {
        // north hline intersection
        yc1 = min_clip_y;
        xc1 = x1 + 0.5f +float(min_clip_y-y1)*(x2-x1)/(y2-y1);
        // test if intersection is valid, of so then done, else compute next
        if (xc1 < min_clip_x || xc1 > max_clip_x)
        {
            xc1 = min_clip_x;
            yc1 = y1 + 0.5f +float(min_clip_x-x1)*(y2-y1)/(x2-x1);
        } // end if

    } break;

    case CLIP_CODE_SW:
    {
        // south hline intersection
        yc1 = max_clip_y;
        xc1 = x1 + 0.5f +float(max_clip_y-y1)*(x2-x1)/(y2-y1);
        // test if intersection is valid, of so then done, else compute next
        if (xc1 < min_clip_x || xc1 > max_clip_x)
        {
            xc1 = min_clip_x;
            yc1 = y1 + 0.5f +float(min_clip_x-x1)*(y2-y1)/(x2-x1);
        } // end if

    } break;

    default:break;
    } // end switch

    // determine clip point for p2
    switch(p2_code)
    {
    case CLIP_CODE_C: break;

    case CLIP_CODE_N:
    {
        yc2 = min_clip_y;
        xc2 = x2 + float(min_clip_y-y2)*(x1-x2)/(y1-y2);
    } break;

    case CLIP_CODE_S:
    {
        yc2 = max_clip_y;
        xc2 = x2 + float(max_clip_y-y2)*(x1-x2)/(y1-y2);
    } break;

    case CLIP_CODE_W:
    {
        xc2 = min_clip_x;
        yc2 = y2 + float(min_clip_x-x2)*(y1-y2)/(x1-x2);
    } break;

    case CLIP_CODE_E:
    {
        xc2 = max_clip_x;
        yc2 = y2 + float(max_clip_x-x2)*(y1-y2)/(x1-x2);
    } break;

    // these cases are more complex, must compute 2 intersections
    case CLIP_CODE_NE:
    {
        // north hline intersection
        yc2 = min_clip_y;
        xc2 = x2 + 0.5f +float(min_clip_y-y2)*(x1-x2)/(y1-y2);
        // test if intersection is valid, of so then done, else compute next
        if (xc2 < min_clip_x || xc2 > max_clip_x)
        {
            // east vline intersection
            xc2 = max_clip_x;
            yc2 = y2 + 0.5f +float(max_clip_x-x2)*(y1-y2)/(x1-x2);
        } // end if

    } break;

    case CLIP_CODE_SE:
    {
        // south hline intersection
        yc2 = max_clip_y;
        xc2 = x2 + 0.5f +float(max_clip_y-y2)*(x1-x2)/(y1-y2);
        // test if intersection is valid, of so then done, else compute next
        if (xc2 < min_clip_x || xc2 > max_clip_x)
        {
            // east vline intersection
            xc2 = max_clip_x;
            yc2 = y2 + 0.5f +float(max_clip_x-x2)*(y1-y2)/(x1-x2);
        } // end if

    } break;

    case CLIP_CODE_NW:
    {
        // north hline intersection
        yc2 = min_clip_y;
        xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);
        // test if intersection is valid, of so then done, else compute next
        if (xc2 < min_clip_x || xc2 > max_clip_x)
        {
            xc2 = min_clip_x;
            yc2 = y2 + 0.5f +float(min_clip_x-x2)*(y1-y2)/(x1-x2);
        } // end if

    } break;

    case CLIP_CODE_SW:
    {
        // south hline intersection
        yc2 = max_clip_y;
        xc2 = x2 + 0.5f +float(max_clip_y-y2)*(x1-x2)/(y1-y2);
        // test if intersection is valid, of so then done, else compute next
        if (xc2 < min_clip_x || xc2 > max_clip_x)
        {
            xc2 = min_clip_x;
            yc2 = y2 + 0.5f +float(min_clip_x-x2)*(y1-y2)/(x1-x2);
        } // end if

    } break;
    default:break;
    } // end switch

    // do bounds check
    if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
        (yc1 < min_clip_y) || (yc1 > max_clip_y) ||
        (xc2 < min_clip_x) || (xc2 > max_clip_x) ||
        (yc2 < min_clip_y) || (yc2 > max_clip_y) )
    {
        return false;
    } // end if

    // store vars back
    x1 = xc1;
    y1 = yc1;
    x2 = xc2;
    y2 = yc2;
    return true;
}


void PipeLine::drawLine(int x1,int y1,int x2,int y2,uint32_t c,uint32_t depth){

    if( clipLine(x1,y1,x2,y2) == false)
        return;

    int x, y, rem = 0;
    if (x1 == x2 && y1 == y2) {
        fillPoint( x1, y1, c,depth);
    }else if (x1 == x2) {
        int inc = (y1 <= y2)? 1 : -1;
        for (y = y1; y != y2; y += inc) fillPoint( x1, y, c,depth);
        fillPoint( x2, y2, c,depth);
    }else if (y1 == y2) {
        int inc = (x1 <= x2)? 1 : -1;
        for (x = x1; x != x2; x += inc) fillPoint( x, y1, c,depth);
        fillPoint( x2, y2, c,depth);
    }else {
        // Bresenham line
        int dx = (x1 < x2)? x2 - x1 : x1 - x2;
        int dy = (y1 < y2)? y2 - y1 : y1 - y2;
        if (dx >= dy) {
            // x axis 每次加1 y + dy 如果大于 dx需要矫正
            if (x2 < x1) std::swap(x1,x2),std::swap(y1,y2);
            int delta = (y2 >= y1)? 1 : -1;
            for (x = x1, y = y1; x <= x2; x++/*x 每次加1 */) {
                rem += dy;
                if (rem >= dx) {
                    rem -= dx;
                    y += delta;
                }
                fillPoint(x,y,c,depth);
            }
        }else {
            if (y2 < y1) std::swap(x1,x2),std::swap(y1,y2);
            int delta = (x2 >= x1)? 1 : -1;
            for (x = x1, y = y1; y <= y2; y++) {
                rem += dx;
                if (rem >= dy) {
                    rem -= dy;
                    x += delta;
                }
                fillPoint(x, y,c,depth);
            }
        }
        fillPoint(x2,y2,c,depth);
    }
}


static inline bool equalf(float x,float y)
{
    return (x-y) < LowEpsilon;
}

/*
void trapezoid(VertexTransform& vt,Float4*f1,Float4*f2,Float4*f3){
    if ( equalf( f1->y , f2->y ) ){
        // 平顶三角  triangle down
        // 2 ____1   1___2
        //   \  |    |  /
        //    \ |    | /
        //     \|    |/
        //      3    3
        if ( f1->x > f2->x ) std::swap(f1,f2);
        //     1___2
        //     |  /
        //     | /
        //     |/
        //     3
        Trapezoid trap{f1,f3,f2,f3,f1->y,f3->y};
        if(trap.top >= trap.bottom) return;
        // FIXME
    }
    else if ( equalf( f2->y , f3->y) ){
        // triangle up
        //      1   1
        //     /|   |\
        //    / |   | \
        // 2 /  |3  |3 \2
        // ----    ----
        if( f2->x > f3->x) std::swap(f2,f3);
        //      1
        //     /|
        //    / |
        // 2 /  |3
        // ----
        Trapezoid trap{f1,f2,f1,f3,f1->y,f3->y};
        if(trap.top >= trap.bottom) return;
        // FIXME
    }
    else{
        //      1
        //     /|
        //    / |
        // 2 /__|4
        //   \  |
        //    \ |
        //     \|
        //     3
        Float4* f4 = assemblyV.indexOf(4);
        assemblyV.memcpy(f2,4,vt.stride_);
        Trapezoid trap1{f1,f2,f1,&f4,f1->y,f2->y};
        Trapezoid trap2{f2,f3,&f4,f3,f2->y,f3->y};

    }


}
*/

void PipeLine::rasterizeTri(VertexTransform& vt,Float4*f1,Float4*f2,Float4*f3){
    // keep this
    //      1
    //     /|
    //    / |
    // 2 /  |
    //   \  |
    //    \ |
    //     \|
    // 3
    if( f1->y > f2->y ) std::swap(f1,f2);
    if( f1->y > f3->y ) std::swap(f1,f3);
    if( f2->y > f3->y ) std::swap(f2,f3);
    // not triangle
    if ((int)f1->y == (int)f2->y && (int)f1->y == (int)f3->y) return;
    if ((int)f1->x == (int)f2->x && (int)f1->x == (int)f3->x) return;


    if(wireframeStateEnabled_){
        drawLine(f1->x,f1->y,f2->x,f2->y,0);
        drawLine(f2->x,f2->y,f3->x,f3->y,0);
        drawLine(f3->x,f3->y,f1->x,f1->y,0);
        //DRAWLINE(0,0,800,600,0);
        //drawLine(0,600,800,0,0);
        //drawLine(0,300,800,300,0);
        //drawLine(400,0,400,600,0);
    }

}

void PipeLine::drawTriMesh(VertexTransform& vt){
    //FIXME visual culling

    VertexTransform::TriangleIterator allTriangles(vt);
    for(auto& i:allTriangles ){
        Float4* f1=i.f1_;
        Float4* f2=i.f2_;
        Float4* f3=i.f3_;

        //back face cull
        if( cullStateEnabled_ && backFaceCull(*f1,*f2,*f3) )
            continue;

        assemblyV.setEmpty();
        assemblyV.clone(f1);
        assemblyV.clone(f2);
        assemblyV.clone(f3);

        // viewport clip
        int ret = clipTriangleInClipSpace(vt);
        if(ret == 3){
            Float4* f0 = assemblyV.orderOf(0);
            Float4* f1 = assemblyV.orderOf(1);
            Float4* f2 = assemblyV.orderOf(2);
            clipCoordToScreenCoord(*f0);
            clipCoordToScreenCoord(*f1);
            clipCoordToScreenCoord(*f2);
            rasterizeTri(vt,f0,f1,f2);
        }
        else if(ret == 4){
            Float4* f0 = assemblyV.orderOf(0);
            Float4* f1 = assemblyV.orderOf(1);
            Float4* f2 = assemblyV.orderOf(2);
            Float4* f3 = assemblyV.indexOf(3);

            clipCoordToScreenCoord(*f0);
            clipCoordToScreenCoord(*f1);
            clipCoordToScreenCoord(*f2);
            clipCoordToScreenCoord(*f3);

            rasterizeTri(vt,f0,f1,f2);
            rasterizeTri(vt,f3,f0,f2);
        }
        else // ret == 0
            continue;
    }
}




int PipeLine::clipTriangleInClipSpace(VertexTransform& vt){
    static constexpr uint32_t GREATE_THAN_B = 1;
    static constexpr uint32_t LESS_THAN_B   = 2;
    static constexpr uint32_t BETWEEN_B     = 4;

#define FRUSTUM_TEST(v,inx,code) do{ \
        if(v.m[inx] > v.w)       code = GREATE_THAN_B;\
        else if(v.m[inx] < -v.w) code = LESS_THAN_B;\
        else                     code = BETWEEN_B;\
}while(0);

#define FRUSTUM_ZPLAN_TEST(v,znear,zfar,code) do{ \
        if(v.w > zfar)       code = GREATE_THAN_B;\
        else if(v.w < znear) code = LESS_THAN_B;\
        else                  code = BETWEEN_B;\
}while(0);


    uint8_t vCode[3]={0,0,0};
    const Float4& v1 = *assemblyV.indexOf(0);
    const Float4& v2 = *assemblyV.indexOf(1);
    const Float4& v3 = *assemblyV.indexOf(2);

    // x all out of frustum
    FRUSTUM_TEST(v1,0,vCode[0]);FRUSTUM_TEST(v2,0,vCode[1]);FRUSTUM_TEST(v3,0,vCode[2]);
    if( (vCode[0] == vCode[1]&&vCode[1] == vCode[2] && vCode[0]== GREATE_THAN_B) ||
        (vCode[0] == vCode[1]&&vCode[1] == vCode[2] && vCode[0]== LESS_THAN_B)){
        return 0;
    }

    //y all out of frustum
    FRUSTUM_TEST(v1,1,vCode[0]);FRUSTUM_TEST(v2,1,vCode[1]);FRUSTUM_TEST(v3,1,vCode[2]);
    if( (vCode[0] == vCode[1]&&vCode[1] == vCode[2]&&vCode[0] == GREATE_THAN_B) ||
        (vCode[0] == vCode[1]&&vCode[1] == vCode[2]&&vCode[0] == LESS_THAN_B)){
        return 0;
    }


    //z clip corss over Z near plane
    // x intersect coordinate  Znear Zfar in ClipSpace == EyeSpace
    // 1. v0 < Znear  AND    v1,v2 > Znear
    // Zeye = Wclip  ratio = ( Znear - v0->w ) / (v1->w - V0->w);
    // Xclip = ratio*v0->w
    //float ratio = (Znear)
    float znear=viewfrustum_.getZMin();
    float zfar =viewfrustum_.getZMax();
    FRUSTUM_ZPLAN_TEST(v1,znear,zfar,vCode[0]);
    FRUSTUM_ZPLAN_TEST(v2,znear,zfar,vCode[1]);
    FRUSTUM_ZPLAN_TEST(v3,znear,zfar,vCode[2]);
    if( (vCode[0] == vCode[1]&&vCode[1] == vCode[2]&&vCode[2] == GREATE_THAN_B) ||
        (vCode[0] == vCode[1]&&vCode[1] == vCode[2]&&vCode[2] == LESS_THAN_B)){
        return 0;
    }

    // all in frustum
    if(vCode[0] == vCode[1]&&vCode[1] == vCode[2]&&vCode[1] == BETWEEN_B){
        return 3;
    }
    else if((vCode[0] | vCode[1] | vCode[2]) & LESS_THAN_B){

        // 处于近平面与远平面之间顶点个数
        int inNum =
            (vCode[0]&BETWEEN_B ? 1 : 0) +
            (vCode[1]&BETWEEN_B ? 1 : 0) +
            (vCode[2]&BETWEEN_B ? 1 : 0) ;

        if(inNum == 1){
            // 1 sort
            if (vCode[1] == BETWEEN_B) {
                /*     1      in
                //     /\
                //    /  \
                //   /____\   znear
                //  /      \
                // 2        0 out
                */
                assemblyV.sort(1,2,0);
            }
            if (vCode[2] == BETWEEN_B) {
                /*     2      in
                //     /\
                //    /  \
                //   /____\   znear
                //  /      \
                // 0        1 out
                */
                assemblyV.sort(2,0,1);
            }
            // else v0 between Znear Zfar
            /*     0      in
            //     /\
            //    /  \
            //   /____\   znear
            //  /      \
            // 1        2 out
            */
            //2 clip
            unsigned channels = vt.vAttr_->channels();
            // 通过插值求交点保存在 index1 中 更新index1
            assemblyV.intersectionPoint(channels,1,0,1,znear);
            // 通过插值求交点保存在 index2 中 更新index2
            assemblyV.intersectionPoint(channels,2,0,2,znear);
            return 3;
        }
        else if(inNum == 2){
            //1 sort outside vetex,keep v0 is in
            if (vCode[1] == LESS_THAN_B) {
            /*
            //0__________2  out
            // \        /
            //  \/_____/   znear
            //   \    /
            //     1       in
            */
                assemblyV.sort(1,2,0);
            }
            if (vCode[2] == LESS_THAN_B) {
            /*
            //1__________0  out
            // \        /
            //  \/_____/   znear
            //   \    /
            //     2       in
            */
                assemblyV.sort(2,0,1);
            }
            //else
            /*
            //2__________1  out
            // \        /
            //  \/_____/   znear
            //   \    /
            //     0       in
            */

            // add new Vertex
            assemblyV.newVertex1(0,vt.vAttr_->bytes());
            //2 clip
            unsigned channels = vt.vAttr_->channels();
            // 通过插值求交点保存在 index1 中 更新index1
            assemblyV.intersectionPoint(channels,0,0,1,znear);
            // 通过插值求交点保存在 index2 中 更新index2
            assemblyV.intersectionPoint(channels,3,3,2,znear);
            return 4;
        }
    }

    return 0;
}


void PipeLine::outlet(void){

    uniforms_->wpvMatrix_ = viewfrustum_.getProjectionViewMatrix() * worldTransform_.getHMatrix() ;

    static VertexTransform  primitive;
    // each mesh
    for(auto m : meshs_){
        // each vertex
        primitive.vAttr_    = m->va_.get();
        primitive.stride_   = m->stride();
        primitive.inxSize_  = m->vi_ ? m->vi_->inxSize() : 0;
        primitive.inxCnt_   = m->vi_ ? m->vi_->numOfIndices : 0;
        primitive.inx_      = m->getIndexPtr();
        primitive.inBuf_    = m->getVertexPtr(0);
        primitive.outBuf_   = m->getCacheVerPtr(0);
        primitive.vertexCnt_= m->vertexCnt_;

        vShader_(primitive,uniforms_,registers_);
        (this->*drawFunction_[m->primType_])(primitive);
    }
}


