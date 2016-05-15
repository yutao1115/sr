#pragma once

#include "Sphere.h"
#include "Plane.h"

class BoundingSphere
{
public:
    BoundingSphere():s_(){}
    void setCenter(float x,float y,float z){s_.center_ = Float3{x,y,z};}
    void setRadius(float radius){s_.radius_=(radius >= 0.0f ? radius : 0.0f);}
    Float4 getCenter() const{return Float4{s_.center_.x,s_.center_.y,s_.center_.z,1.0f};}
    float getRadius() const {return s_.radius_;}
    // +1 on the positive side,-1 when the negative side,
    // 0 transversely cut by the plane
    template<typename PLANE>
    int whichSide(const PLANE& plane) const{
        float signedDistance = plane.distanceToPoint(s_.center_);
        if (signedDistance <= -s_.radius_) return -1;
        if (signedDistance >= +s_.radius_)  return +1;
        return 0;
    }
    // Test for intersection of the two stationary spheres.
    bool testIntersection(const BoundingSphere &sphere) const
    { return testIntersection(sphere.s_);}

    bool testIntersection(const Sphere3 &sphere) const{
        Float4 diffV {s_.center_ - sphere.center_,0};
        // distance square to compare
        return diffV.dot(diffV) <= (s_.radius_ + sphere.radius_)*(s_.radius_ + sphere.radius_);
    }
private:
    // (center, radius) = (c0, c1, c2, r)
    Sphere3 s_;
};
