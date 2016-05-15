#pragma once

#include <stdint.h>
#include <iterator>
#include "VertexAttr.h"
#include "Vectors.h"

struct VertexTransform{
    VertexAttr*   vAttr_;
    uint32_t      vertexCnt_;
    size_t        stride_;
    unsigned      inxSize_;
    uint32_t      inxCnt_;
    void*         inx_;
    float*        outBuf_;
    float*        inBuf_;
    
    
    class VIterator : public std::iterator<std::input_iterator_tag, Float4>
    {
        VertexTransform& vt_;
        Float4* p_;
        Float4* end_;
    public:
        VIterator(VertexTransform& vt,int) :vt_(vt),p_(nullptr),end_(nullptr) {} 
        
        VIterator(VertexTransform& vt) :vt_(vt),
                      p_((Float4*)vt.outBuf_),
                      end_( (Float4*)((char*)p_+vt.vertexCnt_*vt.stride_)){}
                      
        VIterator(const VIterator& rhs) : vt_(rhs.vt_),p_( rhs.p_),end_(rhs.end_){}
        
        VIterator& operator++() { 
            if(p_ != nullptr ) {
                p_=(Float4*)((char*)p_+vt_.stride_); 
                if(p_==end_) p_=nullptr;
            }         
            return *this; 
        }
            
        VIterator operator++(int) {VIterator tmp(*this); operator++(); return tmp;}
        
        bool operator==(const VIterator& rhs) 
        {
           return p_ == rhs.p_;
        }
        bool operator!=(const VIterator& rhs) 
        {
           return p_ != rhs.p_;
        }
        Float4& operator* () {return *p_;}
        Float4* operator->() {return p_;}
        
        VIterator begin(){return VIterator(*this);}
        VIterator end(){return VIterator{vt_,0};}
    };
    
    struct Triangle{
        Float4* f1_;
        Float4* f2_;
        Float4* f3_;
    };
    class TriangleIterator : public std::iterator<std::input_iterator_tag, Triangle>
    {  
        Triangle tri_;
        VertexTransform& vt_;
        uint32_t index_;
        
        void extract(void){
            if(vt_.inxSize_ != 0){
                if(index_ >= vt_.inxCnt_) {
                    index_ = 0xFFFFFFFF;
                    return ;
                }
                uint32_t i1,i2,i3;
                if(vt_.inxSize_ ==sizeof(uint16_t)){
                    uint16_t* inx16 = (uint16_t*)vt_.inx_;
                    i1 = *(inx16+index_);
                    i2 = *(inx16+index_+1);
                    i3 = *(inx16+index_+2);
                }
                else{
                    uint32_t* inx32 = (uint32_t*)vt_.inx_;
                    i1 = *(inx32+index_);
                    i2 = *(inx32+index_+1);
                    i3 = *(inx32+index_+2);
                }
                tri_.f1_ = (Float4*)((uint8_t*)vt_.outBuf_ + i1*vt_.stride_);
                tri_.f2_ = (Float4*)((uint8_t*)vt_.outBuf_ + i2*vt_.stride_);
                tri_.f3_ = (Float4*)((uint8_t*)vt_.outBuf_ + i3*vt_.stride_);
            }
            else{
                if(index_ >= vt_.inxCnt_) {
                    index_ = 0xFFFFFFFF;
                    return ;
                }
                tri_.f1_ = (Float4*)((uint8_t*)vt_.outBuf_ + index_*vt_.stride_);
                tri_.f2_ = (Float4*)((uint8_t*)tri_.f1_         + vt_.stride_       );
                tri_.f3_ = (Float4*)((uint8_t*)tri_.f2_         + vt_.stride_       );
            }
        }
    public:
        TriangleIterator(VertexTransform& vt,int) :vt_(vt),index_(0xFFFFFFFF){} 
        
        TriangleIterator(VertexTransform& vt) :vt_(vt),index_(0){extract();}
                      
        TriangleIterator(const TriangleIterator& rhs) : vt_(rhs.vt_),index_(rhs.index_){}
        
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
        
        TriangleIterator begin(){return TriangleIterator(vt_);}
        TriangleIterator end(){return TriangleIterator{vt_,0};}
    };   
     
    
};



