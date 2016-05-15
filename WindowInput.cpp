#include "WindowInput.h"
static auto boolFunc = [](void){return true;};
static auto voidFunc = [](void){};
static auto voidxyFunc=[](int,int){};
static auto keysFunc =[](unsigned char,int,int){return true;};
static auto ctrlkFunc=[](int,int,int){return true;};
static auto clickFunc=[](int,int,unsigned int){return true;};
static auto pmotion  =[](int,int){return true;};


W_OnInitialize    WindowInput::OnInitialize   = boolFunc;
W_OnTerminate     WindowInput::OnTerminate    = voidFunc;
W_OnDisplay       WindowInput::OnDisplay      = voidFunc;
W_OnResize        WindowInput::OnResize       = voidxyFunc;
W_OnIdle          WindowInput::OnIdle         = voidFunc;
W_OnKey           WindowInput::OnKey          = keysFunc;
W_OnClickDown     WindowInput::OnClickDown    = clickFunc;
W_OnClickUp       WindowInput::OnClickUp      = clickFunc;
W_OnMotion        WindowInput::OnMotion       = pmotion;
W_OnPassiveMotion WindowInput::OnPassiveMotion= pmotion;