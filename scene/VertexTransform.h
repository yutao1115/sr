#pragma once

#include <stdint.h>
#include <iterator>
#include "Vectors.h"

namespace VertexIterator{

    class SeqIterator : public std::iterator<std::input_iterator_tag, Float4>
    {
        size_t   stride_;
        Float4*  p_;
        Float4*  end_;
    public:
        SeqIterator() :vertexCnt_(0),stride_(0),p_(nullptr),end_(nullptr) {} 
        
        SeqIterator(void*ptr,uint32_t vertexCnt,size_t stride) :
            stride_(stride),
            p_((Float4*)ptr),
            end_( (Float4*)((char*)p_+vertexCnt*stride_)){}
                      
        SeqIterator(const SeqIterator& rhs) : vt_(rhs.vt_),p_( rhs.p_),end_(rhs.end_){}
        
        SeqIterator& operator++() { 
            if(p_ != nullptr ) {
                p_=(Float4*)((char*)p_+stride_); 
                if(p_==end_) p_=nullptr;
            }         
            return *this; 
        }
            
        SeqIterator operator++(int) {SeqIterator tmp(*this); operator++(); return tmp;}
        
        bool operator==(const SeqIterator& rhs) 
        {
           return p_ == rhs.p_;
        }
        bool operator!=(const SeqIterator& rhs) 
        {
           return p_ != rhs.p_;
        }
        Float4& operator* () {return *p_;}
        Float4* operator->() {return p_;}
        
        SeqIterator begin(){return SeqIterator(*this);}
        SeqIterator end(){return SeqIterator{};}
    };
    
    struct Triangle{
        Float4* f1_;
        Float4* f2_;
        Float4* f3_;
    };
    class TriangleIterator : public std::iterator<std::input_iterator_tag, Triangle>
    {  
        Triangle       tri_;
        uint32_t       index_;
        
        void*          vBufPtr_;
        void*          inxBufPtr_;
        const uint32_t inxCnt_;
        const size_t   inxStride_;
        const size_t   vstride_;
        
        void extract(void){
            if(inxStride_ != 0){
                if(index_ >= inxCnt_) {
                    index_ = 0xFFFFFFFF;
                    return ;
                }
                uint32_t i1,i2,i3;
                if(inxStride_ ==sizeof(uint16_t)){
                    uint16_t* inx16 = (uint16_t*)inxBufPtr_;
                    i1 = *(inx16+index_);
                    i2 = *(inx16+index_+1);
                    i3 = *(inx16+index_+2);
                }
                else{
                    uint32_t* inx32 = (uint32_t*)inxBufPtr_;
                    i1 = *(inx32+index_);
                    i2 = *(inx32+index_+1);
                    i3 = *(inx32+index_+2);
                }
                tri_.f1_ = (Float4*)((uint8_t*)vBufPtr_ + i1*vstride_);
                tri_.f2_ = (Float4*)((uint8_t*)vBufPtr_ + i2*vstride_);
                tri_.f3_ = (Float4*)((uint8_t*)vBufPtr_ + i3*vstride_);
            }
            else{
                if(index_ >= vt_.inxCnt_) {
                    index_ = 0xFFFFFFFF;
                    return ;
                }
                tri_.f1_ = (Float4*)((uint8_t*)vt_.vBufPtr_ + index_*vstride_);
                tri_.f2_ = (Float4*)((uint8_t*)tri_.f1_         + vstride_       );
                tri_.f3_ = (Float4*)((uint8_t*)tri_.f2_         + vstride_       );
            }
        }
    public:
        TriangleIterator() 
        :  vBufPtr_(nullptr),
           inxBufPtr_(nullptr),
           inxCnt_(0),
           inxStride_(0),
           vstride_(0),index_(0xFFFFFFFF){}
        
        TriangleIterator(void* vBufPtr,void* inxBufPtr,const uint32_t inxCnt,const size_t inxStride,const size_t vstride)
        :  vBufPtr_(vBufPtr),
           inxBufPtr_(inxBufPtr),
           inxCnt_(inxCnt),
           inxStride_(inxStride),
           vstride_(vstride),index_(0){extract();}
                      
        TriangleIterator(const TriangleIterator& rhs) 
        :  vBufPtr_(rhs.vBufPtr_),
           inxBufPtr_(rhs.inxBufPtr_),
           inxCnt_(rhs.inxCnt_),
           inxStride_(rhs.inxStride_),
           vstride_(rhs.vstride_),
           tri_(rhs.tri_),
           index_(rhs.index_)
        {}
        
        TriangleIterator& operator++() {
            if(index_ != 0xFFFFFFFF){
                index_ += 3;
                extract();
            }
            return *this; 
        }
            
        TriangleIterator operator++(int) {TriangleIterator tmp(*this); operator++(); return tmp;}
        
        bool operator==(const TriangleIterator& rhs) 
        {
           return index_ == rhs.index_;
        }
        bool operator!=(const TriangleIterator& rhs) 
        {
           return index_ != rhs.index_;
        }
        Triangle& operator* () {return tri_;}
        Triangle* operator->() {return &tri_;}
        
        TriangleIterator begin(){return TriangleIterator(*this);}
        TriangleIterator end(){return TriangleIterator{};}
    };   
     
    
}



