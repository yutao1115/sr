#pragma once
#include "ResNode.h"
#include "ResMemory.h"
#include "VertexAttr.h"
#include "IndexAttr.h"
#include "VertexIterator.h"
#include <memory>

struct Mesh : public ResNode {
    enum PrimitiveT : unsigned {
        NONE                     = 0xFFFFFFFF,
        POLYPOINT                = 0x00000001,
        POLYSEGMENT_DISJOINT     = 0x00000002,
        POLYSEGMENT_CONTIGUOUS   = 0x00000004,
        TRIMESH                  = 0x00000008,
        TRISTRIP                 = 0x00000010
    };

    unsigned  primType_   = NONE;

    std::shared_ptr<VertexAttr>   va_;
    std::shared_ptr<IndexAttr>    vi_;


    std::unique_ptr<SFloat16Pool> vertexBuffer_ = nullptr;

    float* getVertexPtr(uint32_t index){
        return vertexBuffer_->indexOf(index);
    }

    float* getCacheVerPtr(uint32_t index){
        return getVertexPtr(index + va_->allCnt_);
    }
    void* getIndexPtr(void){
        return (void*)vertexBuffer_->extra();
    }

    uint16_t* getIndexPtr16(void){
        return (uint16_t*)vertexBuffer_->extra();
    }

    uint32_t* getIndexPtr32(void){
        return (uint32_t*)vertexBuffer_->extra();
    }
    
    size_t stride(void){return vertexBuffer_->stride();}
    
    VertexIterator::TriangleIterator triangleIterator(void){
        return VertexIterator::TriangleIterator(getCacheVerPtr(0),getIndexPtr(),
                     vi_?vi_->numOfIndices:0,vi_?vi_->inxSize():0,stride() ); 
    }
    
    VertexIterator::SeqIterator seqIterator(void){
        return VertexIterator::SeqIterator(getVertexPtr(0),getCacheVerPtr(0),va_->allCnt_,stride()); 
    }
    
    static std::shared_ptr<Mesh> resolveId(const Id& id);
};

