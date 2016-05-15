#pragma once
#include "BoundingSphere.h"

class Culler
{
public:
    enum CullingMode{CULL_DYNAMIC,CULL_ALWAYS,CULL_NEVER};
    enum { MAX_PLANE_QUANTITY = 32 };

    Culler():planeCnt_(0){}

    bool pushPlane(const Plane3& plane){
        if (planeCnt_ < MAX_PLANE_QUANTITY){
            plane_[planeCnt_] = plane;
            ++planeCnt_;
            return true;
        }
        return false;
    }

    bool popPlane(){
        if (planeCnt_ > 0){--planeCnt_;return true;}
        return false;
    }

    bool isVisible(const BoundingSphere& sphere){
        if(sphere.getRadius() == 0.0f || planeCnt_<1 )
            return false;
        int index = planeCnt_ - 1;
        for (int i = 0; i < planeCnt_; ++i, --index)
            if (sphere.whichSide(plane_[index]) < 0)
                return false;
        return true;
    }

     // FIXME equal plane not push
    void pushFrustumPlanes(const Matrix44& v){
        Matrix44 vf = v.transpose();
        //warn:
        //the plane’s normal vector is not a unit vector
        //left    a=m14+m11   b=m24+m21 c=m34+m31 d=m44+m41   col4+col1
        //right   a=m14-m11   b=m24-m21 c=m34-m31 d=m44-m41   col4-col1
        //bottom  a=m14+m12   b=m24+m22 c=m34+m32 d=m44+m42   col4+col2
        //top     a=m14-m12   b=m24-m22 c=m34-m32 d=m44-m42   col4-col2
        //near    a=m14+m13   b=m24+m23 c=m34+m33 d=m44+m43   col4+col3
        //far     a=m14-m13   b=m24-m23 c=m34-m33 d=m44-m43   col4-col3
        // Compute L = Length(n0,n1,n2) and set the plane to (n0,n1,n2,c)/L.
        //left
        normalizedPush(Float4{vf.col(3)+vf.col(0)});
        normalizedPush(Float4{vf.col(3)-vf.col(0)});
        normalizedPush(Float4{vf.col(3)+vf.col(1)});
        normalizedPush(Float4{vf.col(3)-vf.col(1)});
        normalizedPush(Float4{vf.col(3)+vf.col(2)});
        normalizedPush(Float4{vf.col(3)-vf.col(2)});
    }
private:
    bool normalizedPush(Float4&& v){
        float invLength = 1.0f / Float3{v.x,v.y,v.z}.length();
        return pushPlane(Plane3{ Float3{v.x*invLength,v.y*invLength,v.z*invLength},v.w*invLength});
    }
private:
    int planeCnt_;
    Plane3 plane_[MAX_PLANE_QUANTITY];
};
