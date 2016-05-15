#pragma once
#include "Vectors.h"

template <typename VEC>
class Sphere
{
public:
    // Construction and destruction.  The default constructor sets the center
    // to (0,...,0) and the radius to 1.
    Sphere():radius_(1.0f){center_.makeZero();}
    Sphere(VEC const& inCenter, float r):center_(inCenter),radius_(r){}

    // Public member access.
    VEC center_;
    float radius_;

public:
    // Comparisons to support sorted containers.
    bool operator==(const Sphere& hypersphere) const{
        return center_ == hypersphere.center_ && radius_ == hypersphere.radius_;
    }
    
    bool operator!=(const Sphere& hypersphere) const{
        return !operator==(hypersphere);
    }
    
    bool operator< (const Sphere& hypersphere) const{
        if (center_ < hypersphere.center_)
            return true;

        if (center_ > hypersphere.center_)
            return false;
  
        return radius_ < hypersphere.radius_;
    }
    
    bool operator<=(const Sphere& hypersphere) const{
        return operator<=(hypersphere) || operator==(hypersphere);
    }
    
    bool operator> (const Sphere& hypersphere) const{
        return !operator<=(hypersphere);
    }
    
    bool operator>=(const Sphere& hypersphere) const{
        return !operator<(hypersphere);
    }
};
 
using Circle2 = Sphere<Float2>;
using Sphere3 = Sphere<Float3>;
using Sphere4 = Sphere<Float4>;
    
