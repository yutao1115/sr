#pragma once

#include "PipeLine.h"
#include "IApplication.h"
#include "Global.h"
#include "FrameBuffer.h"

class Poor : public IApplication
{
public:
    Poor():IApplication(Global::screenBuf()){}

	bool createWorld(void) override;
	void renderWorld(void) override;

};
