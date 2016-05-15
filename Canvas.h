#pragma once
#include "FrameBuffer.h"

class Canvas
{
public:
    void set(int w=800,int h=600){fbo_.reset(new FrameBuffer(w,h));init();}
    void update();
    std::unique_ptr<FrameBuffer>  fbo_;

private:
    void init();
};

