#pragma once

#include <cstddef> // max_align_t
#include <vector>
#include "ResAlloc.h"

template<class T,size_t alignment=alignof(std::max_align_t) >
class ResourceMemory{

    using AllocPolicy = ArenaHeap<alignment>;
    using AllocatorMy = ResAlloc<T,alignment,AllocPolicy>;

    AllocPolicy arena_;
    AllocatorMy alloctor_;
    std::vector<T, AllocatorMy>  vec_;
    size_t      extraSize_;
    void*       extra_;

public:

    ResourceMemory(size_t elementNum,size_t eleSize =sizeof(T),size_t extraSize=0)
        :arena_(),
         alloctor_(arena_,AllocPolicy::align_up(eleSize),extraSize),
         vec_(alloctor_),extraSize_(extraSize){
         vec_.reserve(elementNum);
         extra_= (char*) indexOf(elementNum);
    }


    T* indexOf(size_t index){
        return (T*)((char*)vec_.data() + index*alloctor_.elementSize());}

    T* data(){return vec_.data();}

    void* extra(){return extra_;}
    size_t extraSize(){return extraSize_;}
};

using SFloat16Pool = ResourceMemory<float,16>;

