#pragma once

#include "Vectors.h"
#include <initializer_list>

#include <assert.h>

template<typename VEC>
struct AxisAngle{
    using VECTYPE = typename VEC::type;
    using AXIS    = VEC;
    AxisAngle(std::initializer_list<VECTYPE> args,VECTYPE angle):angle_(angle){
        assert(args.size() <= VEC::ELEMSIZE);
        memcpy(&axis_,args.begin(),args.size()*VEC::ELEMSIZE);
        //axis_.normalize();
    }
    AxisAngle(const AXIS& vec,VECTYPE angle):axis_(vec),angle_(angle){} 
    
    void set(std::initializer_list<VECTYPE> args,VECTYPE angle){
        assert(args.size() <= VEC::ELEMSIZE);
        memcpy(&axis_,args.begin(),args.size()*VEC::ELEMSIZE);
        //axis_.normalize();
        angle_ = angle;
    }
    void set(const AXIS& vec,VECTYPE angle){ axis_=vec;angle_=angle;}
    
    void makeZero(){ axis_ = VEC(); angle_ = 0;}
    AxisAngle& normalize(){axis_.normalize();return *this;}

    VEC                axis_;
    VECTYPE            angle_;
};

using AxisAngle2 = AxisAngle<Float2>;
using AxisAngle3 = AxisAngle<Float3>;
using AxisAngle4 = AxisAngle<Float4>;


