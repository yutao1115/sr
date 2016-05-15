// vs.cpp : 定义控制台应用程序的入口点。
//

#include "resource/MeshBuilder.h"
#include "math/Vectors.h"
#include "resource/MeshAssign.h"
#include <forward_list>
#include <tuple>
#include <iostream>
using namespace std;
using VASemantic = VertexAttr::VASemantic;

struct xxx{ float x, y, z; };
struct yyy { float x, y, z; float length() { return x + y + z; } };
int main()
{
    xxx a;
    yyy* pf = (yyy*)&a;
    cout<<pf->length();
    return 0;

    //std::forward_list<Mesh> dfs;
    //dfs.emplace_front(); dfs.front().id_.type_ = 9; cout << hex << &dfs.front() << endl;
    //dfs.emplace_front(); dfs.front().id_.type_ = 10; cout << hex << &dfs.front() << endl;
    //dfs.remove_if([](Mesh& m) {Id id; id.type_ = 10; return m.id_ == id; });
    //cout << hex << &dfs.front() << endl;
    //return 0;

    MeshBuilder builder;
    
    builder.addAttr(VASemantic::COLOR, sizeof(double));
    builder.addAttr(VASemantic::POSITION, sizeof(Float4));
    builder.addAttr(VASemantic::PSIZE, sizeof(double));
    builder.addAttr(VASemantic::FOG, 27);
    cout << alignof(Mesh);
    auto mesh1 = builder.createMesh(5);
    auto mesh2 = builder.createMesh(4);
    //mesh2.reset();
    cout << " float4:" << sizeof(Float4)
        << " float3:" << sizeof(Float3)
        << " attr size:" << mesh1->va_->size_
        << " attr occupied:" << mesh1->va_->bytes()
        << " Float4 is pod" << std::is_pod<Float4>::value
        << endl;
    cout << (sizeof(Float3) + sizeof(Float4) + sizeof(short)) << endl;
//    ptrdiff_t s = (ptrdiff_t)mesh->vertexBuffer_->indexOf(0);
//    ptrdiff_t e = (ptrdiff_t)mesh->vertexBuffer_->indexOf(1);
    // EXPECT_TRUE(s % 16 == 0);
    // EXPECT_TRUE(e % 16 == 0);
//    cout << e - s << endl;   
    cout << "mesh ptr:" << hex << mesh1->vertexBuffer_.get() << endl;

    //builder.redefine();
    //builder.addAttr(VASemantic::POSITION, sizeof(Float4));
    //builder.addAttr(VASemantic::COLOR, sizeof(Float3));



    
    return 0;
}

