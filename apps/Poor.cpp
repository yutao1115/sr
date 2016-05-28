#include "Poor.h"
#include "loop.h"
#include "WindowInput.h"
#include "MeshBuilder.h"
#include "MeshAssign.h"
#include "VertexAttr.h"
#include "PipeLine.h"
#include "Rotation.h"
#include "AlgoManager.h"


using VASemantic = VertexAttr::VASemantic;
static PipeLine *pipeline;

int main()
{

    WindowInput::OnInitialize = [](){
        Global::apps()->emplace_back(new Poor);
        auto& app = Global::apps()->back();
        if ( !app->createWorld() ){
            printf("create world err\r\n");
            return false;
        }
        return true;
    };
    WindowInput::OnDisplay = [](){
        auto& app = Global::apps()->back();
        app->renderWorld();
    };
    WindowInput::OnKey =[](unsigned char key, int x, int y){
        auto& camera = pipeline->getCamera();
        auto& world  = pipeline->getTransform();
        
        switch(key){
        case 'i': camera.up();break;
        case 'k': camera.down();break;
        case 'j': camera.left();break;
        case 'l': camera.right();break;
        case 'w': camera.lookUp();break;
        case 's': camera.lookDown();break;
        case 'a': camera.turnLeft();break;
        case 'd': camera.turnRight();break;
        case 'n': camera.rollClockwise();break;
        case 'm': camera.rollCounterclockwise();break;
        case 'z': camera.forward();break;
        case 'x': camera.backward();break;
        case 'f':
        case 'g':{
            static float roateAngle=0;
            AxisAngle4 a1 {{0.0f, 0.0f, 1.0f},roateAngle};
            AxisAngle4 a2 {{0.0f, 1.0f, 0.0f},roateAngle};
            AxisAngle4 a3 {{1.0f, 0.0f, 0.0f},roateAngle};
            Matrix44 m1;Rotation::convert(a1,m1);
            Matrix44 m2;Rotation::convert(a2,m2);
            Matrix44 m3;Rotation::convert(a3,m3);
            //AxisAngle4 curY {{0,1,0,0},roateAngle};
            //AxisAngle4 curZ {{0,0,1,0},roateAngle};
            world.setRotation(m1*m2*m3);
            if(key=='f')    roateAngle += PiOver180;
            else  roateAngle -= PiOver180;
            if(roateAngle>TwoPi) roateAngle=0;}break;
        case 'c': 
            pipeline->cullStateEnabled_  = !pipeline->cullStateEnabled_  ;
            break;
        default:
            printf("%c\r\n",key);
        }
        return true;
    };
    srloop(800,600,"sr");
    return 0;
}

bool Poor::createWorld(void) {
    MeshBuilder builder;
    builder.addAttr<Float4>(VASemantic::POSITION);
    
    struct RGB{float R;float G;float B;};
    builder.addAttr<RGB>(VASemantic::COLOR0);
    /*
    //World Vertices 114
    static float vertices[]={
        0.000000 ,0.000000  ,2.000000   ,1,
        0.000000 ,0.000000  ,0.000000   ,1,
        0.382683 ,0.000000  ,1.923880   ,1,
        0.353553 ,0.146447  ,1.923880   ,1,
        0.270598 ,0.270598  ,1.923880   ,1,
        0.146447 ,0.353553  ,1.923880   ,1,
        0.000000 ,0.382683  ,1.923880   ,1,
        -0.146447, 0.353553 ,1.923880   ,1,
        -0.270598, 0.270598 ,1.923880   ,1,
        -0.353553, 0.146447 ,1.923880   ,1,
        -0.382683, 0.000000 ,1.923880   ,1,
        -0.353553, -0.146447, 1.923880  ,1,
        -0.270598, -0.270598, 1.923880  ,1,
        -0.146447, -0.353553, 1.923880  ,1,
        0.000000 ,-0.382683 ,1.923880   ,1,
        0.146447 ,-0.353553 ,1.923880   ,1,
        0.270598 ,-0.270598 ,1.923880   ,1,
        0.353553 ,-0.146447 ,1.923880   ,1,
        0.707107 ,0.000000 ,1.707107    ,1,
        0.653282 ,0.270598 ,1.707107    ,1,
        0.500000 ,0.500000 ,1.707107    ,1,
        0.270598 ,0.653282 ,1.707107    ,1,
        0.000000 ,0.707107 ,1.707107    ,1,
        -0.270598, 0.653282, 1.707107   ,1,
        -0.500000, 0.500000, 1.707107   ,1,
        -0.653282, 0.270598, 1.707107   ,1,
        -0.707107, 0.000000, 1.707107   ,1,
        -0.653281, -0.270598, 1.707107  ,1,
        -0.500000, -0.500000, 1.707107  ,1,
        -0.270598, -0.653282, 1.707107  ,1,
        0.000000 ,-0.707107 ,1.707107   ,1,
        0.270598 ,-0.653281 ,1.707107   ,1,
        0.500000 ,-0.500000 ,1.707107   ,1,
        0.653282 ,-0.270598 ,1.707107   ,1,
        0.923880 ,0.000000 ,1.382683    ,1,
        0.853553 ,0.353553 ,1.382683    ,1,
        0.653281 ,0.653282 ,1.382683    ,1,
        0.353553 ,0.853553 ,1.382683    ,1,
        0.000000 ,0.923880 ,1.382683    ,1,
        -0.353553, 0.853553, 1.382683   ,1,
        -0.653282, 0.653281, 1.382683   ,1,
        -0.853553, 0.353553, 1.382683   ,1,
        -0.923880, 0.000000, 1.382683   ,1,
        -0.853553, -0.353553, 1.382683  ,1,
        -0.653281, -0.653282, 1.382683  ,1,
        -0.353553, -0.853553, 1.382683  ,1,
        0.000000 ,-0.923880 ,1.382683   ,1,
        0.353554 ,-0.853553 ,1.382683   ,1,
        0.653282 ,-0.653281 ,1.382683   ,1,
        0.853553 ,-0.353553 ,1.382683   ,1,
        1.000000 ,0.000000 ,1.000000    ,1,
        0.923880 ,0.382683 ,1.000000    ,1,
        0.707107 ,0.707107 ,1.000000    ,1,
        0.382683 ,0.923880 ,1.000000    ,1,
        0.000000 ,1.000000 ,1.000000    ,1,
        -0.382683, 0.923880, 1.000000   ,1,
        -0.707107, 0.707107, 1.000000   ,1,
        -0.923880, 0.382683, 1.000000   ,1,
        -1.000000, 0.000000, 1.000000   ,1,
        -0.923880, -0.382684, 1.000000  ,1,
        -0.707107, -0.707107, 1.000000  ,1,
        -0.382683, -0.923880, 1.000000  ,1,
        0.000000 ,-1.000000 ,1.000000   ,1,
        0.382684 ,-0.923880 ,1.000000   ,1,
        0.707107 ,-0.707107 ,1.000000   ,1,
        0.923880 ,-0.382683 ,1.000000   ,1,
        0.923880 ,0.000000 ,0.617316    ,1,
        0.853553 ,0.353553 ,0.617316    ,1,
        0.653281 ,0.653282 ,0.617316    ,1,
        0.353553 ,0.853553 ,0.617316    ,1,
        0.000000 ,0.923880 ,0.617316    ,1,
        -0.353553, 0.853553, 0.617316   ,1,
        -0.653282, 0.653281, 0.617316   ,1,
        -0.853553, 0.353553, 0.617316   ,1,
        -0.923880, 0.000000, 0.617316   ,1,
        -0.853553, -0.353553, 0.617316  ,1,
        -0.653281, -0.653282, 0.617316  ,1,
        -0.353553, -0.853553, 0.617316  ,1,
        0.000000 ,-0.923880 ,0.617316   ,1,
        0.353554 ,-0.853553 ,0.617316   ,1,
        0.653282 ,-0.653281 ,0.617316   ,1,
        0.853553 ,-0.353553 ,0.617316   ,1,
        0.707107 ,0.000000 ,0.292893    ,1,
        0.653281 ,0.270598 ,0.292893    ,1,
        0.500000 ,0.500000 ,0.292893    ,1,
        0.270598 ,0.653281 ,0.292893    ,1,
        0.000000 ,0.707107 ,0.292893    ,1,
        -0.270598, 0.653281, 0.292893   ,1,
        -0.500000, 0.500000, 0.292893   ,1,
        -0.653281, 0.270598, 0.292893   ,1,
        -0.707107, 0.000000, 0.292893   ,1,
        -0.653281, -0.270598, 0.292893  ,1,
        -0.500000, -0.500000, 0.292893  ,1,
        -0.270598, -0.653281, 0.292893  ,1,
        0.000000 ,-0.707107 ,0.292893   ,1,
        0.270598 ,-0.653281 ,0.292893   ,1,
        0.500000 ,-0.500000 ,0.292893   ,1,
        0.653282 ,-0.270598 ,0.292893   ,1,
        0.382683 ,0.000000 ,0.076120    ,1,
        0.353553 ,0.146447 ,0.076120    ,1,
        0.270598 ,0.270598 ,0.076120    ,1,
        0.146447 ,0.353553 ,0.076120    ,1,
        0.000000 ,0.382683 ,0.076120    ,1,
        -0.146447, 0.353553, 0.076120   ,1,
        -0.270598, 0.270598, 0.076120   ,1,
        -0.353553, 0.146447, 0.076120   ,1,
        -0.382683, 0.000000, 0.076120   ,1,
        -0.353553, -0.146447, 0.076120  ,1,
        -0.270598, -0.270598, 0.076120  ,1,
        -0.146447, -0.353553, 0.076120  ,1,
        0.000000 ,-0.382683 ,0.076120   ,1,
        0.146447 ,-0.353553 ,0.076120   ,1,
        0.270598 ,-0.270598 ,0.076120   ,1,
        0.353553 ,-0.146447 ,0.076120   ,1,
    };
    */
    static MeshBuilder::MeshPtr mesh =  builder.createMesh(8,36);//(8,36)
    AlgoManager<Interpolat>::instance().registerMesh(mesh.get());
    AlgoManager<IScanline>::instance().registerMesh(mesh.get());
    Interpolat* l  = AlgoManager<Interpolat>::instance().get(mesh.get());
    IScanline* is  = AlgoManager<IScanline>::instance().get(mesh.get());
    *l=[](void* result,void*v1,void*v2,float t){
        float* r  = (float*)result; 
        float* p1 = (float*)v1;
        float* p2 = (float*)v2;
        //R
        *r = lerp(*p1, *p2, t); ++r;++p1;++p2;
        //G
        *r = lerp(*p1, *p2, t); ++r;++p1;++p2;
        //B
        *r = lerp(*p1, *p2, t);
    };
    is->lineInit_ = [](void* result,void*v1,void*v2,float t){
        float* r  = (float*)result; 
        float* lhs = (float*)v1;
        float* rhs = (float*)v2;
        *r  = (*rhs  - *lhs)  * t;++r;++rhs;++lhs;
        *r  = (*rhs  - *lhs)  * t;++r;++rhs;++lhs;
        *r  = (*rhs  - *lhs)  * t;
    };
    is->stepAdd_ = [](void* result,void*step){
        float* r  = (float*)result; 
        float* s = (float*)step;
        *r  += *s;++r;++s;
        *r  += *s;++r;++s;
        *r  += *s;
    };
    is->pixelColor_=[](ClonedVertex& v,uint32_t*t){
        int R = (int)(*v.leftChannels * 255.0f);
		int G = (int)(*(v.leftChannels+1) * 255.0f);
		int B = (int)(*(v.leftChannels+2) * 255.0f);
		R = clamp(R, 0, 255);
		G = clamp(G, 0, 255);
		B = clamp(B, 0, 255);
		*t = (R << 16) | (G << 8) | (B);
    };
  
 uint16_t indices[] = {  // Note that we start from 0!
        // 前
        1, 3, 0,
        2, 3, 1,

        // 后
        7, 5, 4,
        7, 6, 5,

        // 下
        4, 1, 0,
        5, 1, 4,

        // 左
        5, 2, 1,
        6, 2, 5,

        // 上
        6, 3, 2,
        7, 3, 6,

        // 右
        7, 0, 3,
        4, 0, 7,
    };
    static float vvv[]={ 
                           1, -1,  1, 1 ,1.0f, 0.2f, 0.2f, //0
                          -1, -1,  1, 1 ,0.2f, 1.0f, 0.2f, //1
                          -1,  1,  1, 1 ,0.2f, 0.2f, 1.0f, //2
                           1,  1,  1, 1 ,1.0f, 0.2f, 1.0f, //3
                           1, -1, -1, 1 ,1.0f, 1.0f, 0.2f, //4
                          -1, -1, -1, 1 ,0.2f, 1.0f, 1.0f, //5
                          -1,  1, -1, 1 ,1.0f, 0.3f, 0.3f, //6
                           1,  1, -1, 1 ,0.2f, 1.0f, 0.3f, //7
                        }; 
     
    MeshVertexSeqIn mva(*mesh);
    mva.streamCopy(vvv,sizeof(vvv));
    memcpy(mesh->getIndexPtr16(),indices,36*sizeof(uint16_t));


    Global::screenBuf()->init(800,600);
    pipeline = new PipeLine(*Global::screenBuf(),4);
    pipeline->setRasterizeAlgo(RasAlgo::SCANLINE_ALGO);
    pipeline->cullStateEnabled_ = false;
    //pipeline->setCullState(PipeLine::FT_CW,PipeLine::CT_BACK);
    //pipeline->getTransform().setTranslation(2.0f, 0.0f, 0.0f);
    //pipeline->getTransform().setRotation( AxisAngle4{{0.0f, 1.0f, 0.0f,0},PiHalf/2.0f});
    //pipeline->getTransform().setScale(1.0f, 2.0f, 1.0f);
    //pipeline->getTransform().setUniformScale(2.0f);
    //pipeline->getTransform().setTranslation(0,0,-20.0f);
    Float4 eye{0,0,20,1};
    Float4 target{0,0,0,1};
    Float4 up{0,1,0,1};
    pipeline->getCamera().lookAt(eye,target,up);
    pipeline->attachMesh(mesh.get());
    pipeline->setVertexShader([]
    (Mesh* m,const PipeLine::Uniforms* u,PipeLine::Registers&)
    {
        VertexIterator::SeqIterator allVertex(m->seqIterator());
        for(auto& i : allVertex){
           *i.dst = u->wpvMatrix_ * (*i.src);
           memcpy(i.dst + 1, i.src + 1, sizeof(RGB));
        }
    });
    return true;
}

void Poor::renderWorld(void) {
    Global::screenBuf()->clear();
    pipeline->outlet();
}


