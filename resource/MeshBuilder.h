#pragma once

#include "Mesh.h"


class MeshBuilder{
public:
    using PrimitiveT = Mesh::PrimitiveT;
    using MeshPtr    = std::shared_ptr<Mesh>;
    using InxType    = IndexAttr::InxType;
    using VASemantic = VertexAttr::VASemantic;
    static constexpr bool DOUBLECACHE = true;

    MeshBuilder():va_(std::make_shared<VertexAttr>()),
                  vi_(std::make_shared<IndexAttr>()){}

    template<class T>
    bool addAttr(VASemantic se){
        return va_->add<T>(se);
    }

    MeshPtr createMesh(uint32_t numOfVertices,
                       uint32_t numOfIndices=0,
                       InxType inType = InxType::Index16,
                       bool     doubleCache = DOUBLECACHE,
                       PrimitiveT t   = PrimitiveT::TRIMESH);


    void redefine(void){
        va_ = std::make_shared<VertexAttr>();
        vi_ = std::make_shared<IndexAttr>();
    }

    void redefine(std::shared_ptr<VertexAttr>& va,
                  std::shared_ptr<IndexAttr>&vi)
    {
        va_ = va; vi_ = vi;
    }

private:
    std::shared_ptr<VertexAttr>  va_;
    std::shared_ptr<IndexAttr>   vi_;
};



