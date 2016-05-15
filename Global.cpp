#include "IApplication.h"
#include "FrameBuffer.h"

namespace Global{

FrameBuffer*  screenBuf(void){
    static FrameBuffer c;
    return &c;
}

IApps*   apps(void){
    static IApps apps;
    return &apps;
}

}
