#pragma once
#include <functional>

using W_OnInitialize = std::function<bool(void)>;
using W_OnTerminate  = std::function<void(void)>;
using W_OnDisplay    = std::function<void(void)>;
using W_OnResize     = std::function<void(int w, int h)>;
using W_OnIdle       = std::function<void(void)>;
using W_OnKey        = std::function<bool(unsigned char key, int x, int y)>;
using W_OnClickDown  = std::function<bool(int x, int y,unsigned int uiModifiers)>;
using W_OnClickUp    = std::function<bool(int x, int y,unsigned int uiModifiers)>;
using W_OnMotion     = std::function<bool(int x, int y)>;
using W_OnPassiveMotion=std::function<bool(int x,int y)>;


struct WindowInput{
    static  W_OnInitialize    OnInitialize   ;
    static  W_OnTerminate     OnTerminate    ;
    static  W_OnDisplay       OnDisplay      ;
    static  W_OnResize        OnResize       ;
    static  W_OnIdle          OnIdle         ;
    static  W_OnKey           OnKey          ;
    static  W_OnClickDown     OnClickDown    ;
    static  W_OnClickUp       OnClickUp      ;
    static  W_OnMotion        OnMotion       ;
    static  W_OnPassiveMotion OnPassiveMotion;


    static constexpr int MOUSE_UP          = 0;
    static constexpr int MOUSE_DOWN        = 1;
    static constexpr int MODIFIER_CONTROL  = 6;
    static constexpr int MODIFIER_BUTTON   = 7;
    static constexpr int MODIFIER_SHIFT    = 8;
};

