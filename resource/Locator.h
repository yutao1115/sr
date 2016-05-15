#pragma once
#include <string>
#include <stdint.h>
struct Locator{
    Locator(const std::string& loc,uint32_t flag = 0)
    :location_(loc),flag_(flag){
    }
    bool isShared() const {return flag_ != NonShared;}

    static const uint32_t NonShared = 0xFFFFFFFF;
    std::string location_;
    uint32_t    flag_;
};
