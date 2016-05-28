#include "../MeshBuilder.h"
#include "../../math/Vectors.h"
#include "../MeshAssign.h"
#include <gtest/gtest.h>
#include <tuple>
#include <iostream>
using namespace std;
using VASemantic = VertexAttr::VASemantic;
TEST(MeshBuilderTest,position){
    MeshBuilder builder;
    builder.addAttr<double>(VASemantic::COLOR0);
    builder.addAttr<Float4>(VASemantic::POSITION);
    builder.addAttr<double>(VASemantic::PSIZE);
    builder.addAttr<double>(VASemantic::FOG);
    auto mesh = builder.createMesh(5);
    cout<<" float4:"<<sizeof(Float4)
        <<" float3:"<<sizeof(Float3)
        <<" attr size:"<<mesh->va_->size_
        <<" attr occupied:"<<mesh->va_->bytes()
        <<" Float4 is pod"<<std::is_pod<Float4>::value
        <<endl;
    cout<<(sizeof(Float3)+sizeof(Float4)+sizeof(short))<<endl;
    ptrdiff_t s = (ptrdiff_t)mesh->vertexBuffer_->indexOf(0);
    ptrdiff_t e = (ptrdiff_t)mesh->vertexBuffer_->indexOf(1);
    EXPECT_TRUE( s%16 == 0 );
    EXPECT_TRUE( e%16 == 0 );
    cout<<e-s<<endl;

    builder.redefine();
    builder.addAttr<Float4>(VASemantic::POSITION);
    builder.addAttr<Float3>(VASemantic::COLOR0);


    mesh =  builder.createMesh(4);
    meshAssign(*mesh,0,Float4{1,2,3,4},Float3{6,7,8});
    meshAssign(*mesh,1,Float4{2,2,2,2},Float3{1,1,1});
    using MemoryObj = tuple<Float4,Float3>;

    Float4* v1Position = (tuple_element<0,MemoryObj>::type*)mesh->vertexBuffer_->indexOf(0);
    Float4* v2Position = (tuple_element<0,MemoryObj>::type*)mesh->vertexBuffer_->indexOf(1);
    cout<<"v1:"<<hex<<v1Position<<" v2:"<<v2Position<<endl;
    EXPECT_EQ((*v1Position),(Float4{1,2,3,4}));
    EXPECT_EQ((*v2Position),(Float4{2,2,2,2}));

}

