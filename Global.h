#pragma once
#include <vector>

class FrameBuffer;
class IApplication;

namespace Global{
using IApps = std::vector<std::unique_ptr<IApplication>> ;
FrameBuffer*  screenBuf(void);
IApps*   apps(void);

}
