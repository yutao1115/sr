#include "MeshBuilder.h"
#include "ResRegistry.h"
#include "Enum.h"
#include <stdio.h>// sprintf
using PrimitiveT = Mesh::PrimitiveT;
using Code       = ResourceState::Code;
using MeshPtr    = MeshBuilder::MeshPtr;
using InxType    = IndexAttr::InxType;

static unsigned int s_sequence;
static unsigned int next(){
    if(s_sequence==9999999)
        s_sequence = 0;
    return s_sequence++;
}

MeshPtr MeshBuilder::createMesh(uint32_t numOfVertices,
                                uint32_t numOfIndices,/*=0*/
                                InxType inType,/* = InxType::Index16*/
                                bool     doubleCache,//=true,
                                PrimitiveT t   /*= PrimitiveT::TRIMESH*/
                               )
{
    static char name[10]={0};
    sprintf(name,"%07uMB",next());
    Id id = ResRegistry::instance().
        add(Locator(name),ResourceType::Mesh_T);

    auto mesh = Mesh::resolveId(id);
    //mesh->resState_ = Code::Initial;
    mesh->va_        = va_;
    mesh->primType_  = t;
    mesh->va_->allCnt_ = numOfVertices;

     
    if(numOfIndices){
        vi_->indexType = inType;
        vi_->numOfIndices = numOfIndices;
        mesh->vi_    = vi_;
    }


    mesh->vertexBuffer_.reset(new SFloat16Pool(
                               doubleCache?(2*numOfVertices):numOfVertices,
                               va_->bytes(),vi_->bytes()));
    mesh->resState_ = Code::Setup;
    return mesh;
}



