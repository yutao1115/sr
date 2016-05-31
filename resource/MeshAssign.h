#pragma once
#include "Mesh.h"
#include "ResourceState.h"
#include <type_traits>
#include <initializer_list>
#include <string.h>
#include <assert.h>


namespace MeshAssignDetail{

using Code       = ResourceState::Code;
using VASemantic = VertexAttr::VASemantic;
using Attrs      = VertexAttr::Container;

template<class T>
void helper(size_t index,void* pushIn,const Attrs &attr,T value){
	assert(attr[index].typeSize <= sizeof(T) );
    if( attr[index].semantic != VASemantic::NO_SEMANTIC)
    {
        memcpy((char*)pushIn + attr[index].offset,
               &value,sizeof(T));
    }
}

template<size_t ...I,typename... Args>
void push(std::index_sequence<I...>,void* pushIn,const Attrs &attr,Args... args){
	std::initializer_list<int>{ (helper(I,pushIn,attr,args),0) ...};
}

}//end of namespace

template <typename... Ts>
inline void meshAssign(Mesh&mesh,size_t index,Ts ...args){
    assert(mesh.resState_ == ResourceState::Code::Setup);
    assert(index < mesh.va_->allCnt_);
	MeshAssignDetail::push(std::make_index_sequence<sizeof...(Ts)>(),
						mesh.vertexBuffer_->indexOf(index),
						mesh.va_->attributes_,args...);
}

struct MeshVertexSeqIn{
    MeshVertexSeqIn(Mesh& mesh) :mesh_(mesh),
        dst_( (char*)mesh_.getVertexPtr(0) ),
        endp_((const char*)mesh_.getVertexPtr(mesh_.va_->allCnt_))
        {}

    Mesh& mesh_;
    size_t index_ = 0;
    char* dst_;
    const char* endp_;

    template <typename... Ts>
    void append(Ts ...args){
        if (dst_ < endp_) {
            meshAssign(mesh_, index_++, std::move(args)...);
            dst_ += mesh_.stride();
        }
    }

    void streamCopy(const float*input,size_t bytes) {
        size_t allCnt = mesh_.va_->allCnt_;
        size_t rest   = bytes;
        size_t stride = mesh_.stride();
        size_t elementBytes = mesh_.va_->bytes();
        const char*inp = (const char*)input;
        while (dst_ < endp_ && index_ < allCnt && rest >= elementBytes ) {
            memcpy(dst_, inp, elementBytes);
            inp  += elementBytes;

            ++index_;
            dst_ += stride;
        }
    }
};













