#pragma once
#include <memory>
#include <vector>
#include <stdint.h>
#include <stdio.h>

class FrameBuffer{
public:
    enum DepthValue : unsigned int
    {
        DEPTH_NONE = 0x00000000,
        DEPTH_16   = 0x0000FFFF,
        DEPTH_24   = 0x00FFFFFF,
        DEPTH_32   = 0xFFFFFFFF
    };
    FrameBuffer( const FrameBuffer& ) = delete;
    FrameBuffer& operator=( const FrameBuffer& ) = delete;
    
    FrameBuffer(void):maxDepthValue_(DEPTH_NONE),w_(0),h_(0),
                     ownself_(false),pixels_(nullptr){}
    
    FrameBuffer(int w,int h,DepthValue d=DEPTH_24)
        :maxDepthValue_(d),w_(w),h_(h),ownself_(true),
        pixels_( new uint32_t[w*h]() ) 
    {
        if(d) depthBuffer_.reset(new std::vector<float>(w*h));
    }
    
    ~FrameBuffer(){
        if(ownself_) delete []pixels_;
        pixels_=nullptr;
    }
    
    void init(int w,int h,uint32_t* p=nullptr,DepthValue d=DEPTH_32){
        maxDepthValue_=d,w_=w,h_=h;
        
        if(p) pixels_= p,ownself_=false;
        else  pixels_= new uint32_t[w*h](),ownself_=true;
          
        if(d) depthBuffer_.reset(new std::vector<float>(w*h));
    }
    
    void set(int x,int y,uint32_t color,uint32_t depth){
        if(x>=w_) x = w_ -1; if(y>=h_) y = h_-1;
        uint32_t pos = y*w_+x;
        //printf("x:%u,y:%u,set pos:%u\r\n",x,y,pos);
        pixels_[pos]=color;
        if(depthBuffer_) (*depthBuffer_)[pos]=depth;
    }
    
    void clear(void){
        int y, x, height = h_;
        for (y = 0; y < height; y++) {
            uint32_t *dst = pixels_+y*w_;
            //uint32_t cc = (height - 1 - y) * 230 / (height - 1);
            //cc = (cc << 16) | (cc << 8) | cc;
		
            for (x = w_; x > 0; dst++, x--) dst[0] = (255 << 16) | (255 << 8) | 255;
        }
        if(depthBuffer_){
            float*p =depthBuffer_->data();
            for (y = 0; y < height; y++) {
                float *dst = p+y*w_;
                for (x = w_; x > 0; dst++, x--) dst[0] = 0.0f;
            }
        }
    }

    const uint32_t* pixels(void){return pixels_;}
    int width(void){return w_;}
    int height(void){return h_;}

public:

    Float2Uint(float i){return (unsigned int)(i*(float)maxDepthValue_);}

private:
    DepthValue maxDepthValue_;
    int w_, h_;

    bool         ownself_;
    uint32_t*    pixels_;
    std::shared_ptr<std::vector<float>> depthBuffer_;
};
