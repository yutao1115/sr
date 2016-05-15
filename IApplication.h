#pragma once
#include <vector>
#include <memory>
#include "FrameBuffer.h"

class IApplication
{
public:
    IApplication(const IApplication&) = delete;
    IApplication& operator=( const IApplication& ) = delete;
    IApplication(FrameBuffer* fbo = nullptr):fbo_(fbo){}
	virtual ~IApplication(){}
	virtual bool createWorld(void)  = 0;
	virtual void renderWorld(void)  = 0;
    FrameBuffer* fbo_;
};

using  IApps = std::vector<std::unique_ptr<IApplication>> ;
