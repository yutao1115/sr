#include "PipeLine.h"
#include "Mesh.h"
#include "AlgoManager.h"
using VASemantic = VertexAttr::VASemantic;

thread_local VerticesAssembly assemblyV;

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
        halfw,  0  ,  0   , srcx+halfw,
        0,     -halfh,0   , srcy+halfh,
        0,      0  ,  1   ,    0     ,
        0,      0  ,  0   ,    1      };
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
    rasterAlgo_[RasAlgo::SCANLINE_ALGO] = &PipeLine::scanlineAlgo;
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
        int dx = (x1 < x2)? (x2 - x1) : (x1 - x2);
        int dy = (y1 < y2)? (y2 - y1) : (y1 - y2);
        if (dx >= dy) {
            /* x axis 每次加1 y + dy 如果大于 dx需要矫正
			*/
			if (x2 < x1) { std::swap(x1, x2);std::swap(y1, y2); }
            int delta = (y2 >= y1)? 1 : -1;
            for (x = x1, y = y1; x <= x2; x++/* x 每次加1 */) {
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


void PipeLine::scanlineAlgo(const RasTrapezoid* trap){
    IScanline*  i = AlgoManager<IScanline>::instance().get(curMesh_);
    IBase* l = AlgoManager<IBase>::instance().get(curMesh_);
    assert(i != nullptr);
    ScanlineAlgo{ *trap,fbo_,*i,l->interpolat_};
}

void PipeLine::renderTrapezoid(const RasTrapezoid* trap){
    //pShader_(uniforms_,registers_,assemblyV,trap,fbo_);
    (this->*rasterAlgo_[algo_])(trap);
}

void PipeLine::trapezoidization(ClonedVertex*f1,ClonedVertex*f2,ClonedVertex*f3){

    if ( equal( f1->coord.y , f2->coord.y ) ){
        /*
        // triangle down
        // 2 ____1   1___2
        //   \  |    |  /
        //    \ |    | /
        //     \|    |/
        //      3    3
        */
        if ( f1->coord.x > f2->coord.x ) std::swap(f1,f2);
        /*
        //     1___2
        //     |  /
        //     | /
        //     |/
        //     3
        */
        RasTrapezoid trap{f1,f3,f2,f3,f1->coord.y,f3->coord.y};
        if(trap.top >= trap.bottom) return;
        renderTrapezoid(&trap);
    }
    else if ( equal( f2->coord.y , f3->coord.y) ){
        /*
        // triangle up
        //      1   1
        //     /|   |\
        //    / |   | \
        // 2 /  |3  |3 \2
        //  ----    ----
        */
        if( f2->coord.x > f3->coord.x) std::swap(f2,f3);
        /*
        //      1
        //     /|
        //    / |
        // 2 /  |3
        //  ----
        */
        RasTrapezoid trap{f1,f2,f1,f3,f1->coord.y,f3->coord.y};
        if(trap.top >= trap.bottom) return;
        renderTrapezoid(&trap);
    }
    else{
        auto k = (f3->coord.y - f1->coord.y) / (f2->coord.y - f1->coord.y);
        auto x3 = f1->coord.x + (f2->coord.x - f1->coord.x) * k;
        if(x3 <= f3->coord.x){
        /*
        //      1
        //     /|
        //    / |
        // 2 /__|
        //   \  |
        //    \ |
        //     \|
        //     3
        */
        RasTrapezoid trap1{f1,f2,f1,f3,f1->coord.y,f2->coord.y};
        renderTrapezoid(&trap1);
        RasTrapezoid trap2{f2,f3,f1,f3,f2->coord.y,f3->coord.y};
        renderTrapezoid(&trap2);
        }
        else{
        /*
        // 1
        // |\
        // | \
        // |__\ 2
        // |  /
        // | /
        // |/
        // 3
        */
        RasTrapezoid trap1{f1,f3,f1,f2,f1->coord.y,f2->coord.y};
        renderTrapezoid(&trap1);
        RasTrapezoid trap2{f1,f3,f2,f3,f2->coord.y,f3->coord.y};
        renderTrapezoid(&trap2);
        }
    }
}

void PipeLine::rasterizeTri(int i1,int i2,int i3){

    ClonedVertex* c1 = assemblyV.orderV(i1);
    ClonedVertex* c2 = assemblyV.orderV(i2);
    ClonedVertex* c3 = assemblyV.orderV(i3);
    

    /*
    // sort , like this
    //      1
    //     /|
    //    / |
    // 2 /  |
    //   \  |
    //    \ |
    //     \|
    // 3
    */
    if( c1->coord.y > c2->coord.y ) std::swap(c1,c2);
    if( c1->coord.y > c3->coord.y ) std::swap(c1,c3);
    if( c2->coord.y > c3->coord.y ) std::swap(c2,c3);
    // not triangle
    if ((int)c1->coord.y == (int)c2->coord.y && (int)c1->coord.y == (int)c3->coord.y) return;
    if ((int)c1->coord.x == (int)c2->coord.x && (int)c1->coord.x == (int)c3->coord.x) return;

    if(algo_ == WIREFRAME_ALGO){
        drawLine(c1->coord.x,c1->coord.y,c2->coord.x,c2->coord.y,0);
        drawLine(c2->coord.x,c2->coord.y,c3->coord.x,c3->coord.y,0);
        drawLine(c3->coord.x,c3->coord.y,c1->coord.x,c1->coord.y,0);
    }
    else{
        trapezoidization(c1,c2,c3);
    }

}

void PipeLine::drawTriMesh(void){
    //FIXME visual culling

    VertexIterator::TriangleIterator allTriangles(curMesh_->triangleIterator());
    for(auto& i:allTriangles ){
        Float4* f1=i.f1_;
        Float4* f2=i.f2_;
        Float4* f3=i.f3_;

        //back face cull
        if( cullStateEnabled_ && backFaceCull(*f1,*f2,*f3) )
            continue;

        assemblyV.reset();
        assemblyV.clone(f1,0);
        assemblyV.clone(f2,1);
        assemblyV.clone(f3,2);

        // clip in clipspace lerp 
        /*
           剪裁空间内插值求交点 不需要透视矫正
        */
        int ret = clipTriangleInClipSpace();
        if(ret == 3){
            // Clip space TO screen space
            assemblyV.toScreen(uniforms_->viewPort_,assemblyV.NOTRICLIP);
            rasterizeTri(0,1,2);
        }
        else if(ret == 4){
            // Clip space TO screen space
            assemblyV.toScreen(uniforms_->viewPort_,assemblyV.TRICLIPED);
            rasterizeTri(0,1,2);
            rasterizeTri(3,0,2);
        }
        else // ret == 0
            continue;
    }
}



#define FRUSTUM_TEST(v,inx,code) do{ \
        if(v.m[inx] > v.w)       code = GREATE_THAN_B;\
        else if(v.m[inx] < -v.w) code = LESS_THAN_B;\
        else                       code = BETWEEN_B;\
}while(0);

#define FRUSTUM_ZPLAN_TEST(v,znear,zfar,code) do{ \
        if(v.w > zfar)       code = GREATE_THAN_B;\
        else if(v.w < znear) code = LESS_THAN_B;\
        else                  code = BETWEEN_B;\
}while(0);

int PipeLine::clipTriangleInClipSpace(void){
    static constexpr uint32_t GREATE_THAN_B = 1;
    static constexpr uint32_t LESS_THAN_B   = 2;
    static constexpr uint32_t BETWEEN_B     = 4;




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

        /* 处于近平面与远平面之间顶点个数  
		*/
		int inNum 
			= (vCode[0]&BETWEEN_B ? 1 : 0) +
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
            /* 通过插值求交点保存在 index1 中 更新index1 
			*/
            assemblyV.intersectionPoint(1,1,0,znear);
            /* 通过插值求交点保存在 index2 中 更新index2 
			*/
            assemblyV.intersectionPoint(2,2,0,znear);
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
            assemblyV.newVertex(0);
            //2 clip
            /* 通过插值求交点保存在 index1 中 更新index1
            */
            assemblyV.intersectionPoint(0,0,1,znear);
            /* 通过插值求交点保存在 index3 中 更新index3
            */
            assemblyV.intersectionPoint(3,3,2,znear);
            return 4;
        }
    }

    return 0;
}


void PipeLine::outlet(void){
    // Transforming a view space point (vx, vy, vz, 1) into clip space looks like this:
    //|sx  0  0  0||vx|   |sx * vx     |
    //| 0 sy  0  0||vy| = |sx * vy     |
    //| 0  0 sz tz||vz|   |sz * vz + tz|
    //| 0  0 -1  0|| 1|   |-vz         |
    // The corresponding NCD values are computed as follows:
    //|nx|   | (sx * vx)      / -vz |
    //|nx| = | (sy * vy)      / -vz |
    //|nz|   | (sz * vz + tz) / -vz |
    uniforms_->wpvMatrix_ = viewfrustum_.getProjectionViewMatrix() * worldTransform_.getHMatrix() ;
    // each mesh
    for(auto m : meshs_){
        // each vertex
        curMesh_ = m;
        assemblyV.init(m);
        vShader_(m,uniforms_,registers_);
        (this->*drawFunction_[m->primType_])();
    }
}


