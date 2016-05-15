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
    assert(index < mesh.vertexCnt_);
	MeshAssignDetail::push(std::make_index_sequence<sizeof...(Ts)>(),
						mesh.vertexBuffer_->indexOf(index),
						mesh.va_->attributes_,args...);
}

struct MeshVertexAppend{
    MeshVertexAppend(Mesh& mesh):mesh_(mesh){};
    Mesh& mesh_;
    size_t index = 0;
    template <typename... Ts>
    void append(Ts ...args){
        meshAssign(mesh_,index++,std::move(args)...);
    }
};













