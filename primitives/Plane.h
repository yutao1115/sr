#pragma once
#include "Vectors.h"

// The plane is defined by Dot((n0,n1,n2,d),(x0,x1,x2,1)) = 0, where a
// plane normal is N = (n0,n1,n2,0), d is a constant, and X = (x0,x1,x2,1)
// is a point on the plane.  If P = (p0,p1,p2,1) is a point on the plane, then
// Dot(N,X-P) = 0, ==> DOT(N,X) - Dot(N,P) = 0
// ==> nxX+nyY+nzZ -Dot(N,P) = 0
// ==> aX +bY +cZ + d        = 0
// in which case d = -Dot(N,P).  If P0, P1, and P2 are points
// on the plane and are not collinear, then N = Cross(P1-P0,P2-P0) and
// d = -Dot(N,P0).

template <typename VEC>
class Plane
{
public:
    // Construction and destruction.  The default constructor sets the normal
    // to (0,...,0,1) and the constant to zero (plane z = 0).
    Plane():d_(0){normal_.makeUnit();}

    // Specify U and d directly.
    Plane(const VEC& inNormal, float d):normal_(inNormal),d_(d){}

    // U is specified, d = -Dot(U,p) where p is a point on the plane.
    Plane(const VEC& inNormal,const VEC& p):normal_(inNormal),d_(- inNormal.dot(p)){}

    // Public member access.
    VEC normal_;
    float d_;
    // The "positive side" of the plane is the half space to which the
    // plane normal is directed.  The "negative side" is the other half
    // space.  The function returns +1 when P is on the positive side, -1
    // when P is on the negative side, or 0 when P is on the plane.
    int whichSide (const Float4& P) const{
        int signedDistance = distanceToPoint(P);
        return (signedDistance > 0.0f ? (+1) : (signedDistance < 0.0f ? -1 : 0));
    }
    int whichSide (const Float3& P) const{
        int signedDistance = distanceToPoint(Float4{P,1.0f});
        return (signedDistance > 0.0f ? (+1) : (signedDistance < 0.0f ? -1 : 0));
    }
    // Compute d = Dot(N,P)+d where N is the plane normal and d is the plane
    // constant.  This is a signed pseudodistance.  The sign of the return
    // value is consistent with that in the comments for WhichSide(...).
    float distanceToPoint (const Float4& P) const{
        return Float4{normal_.x,normal_.y,normal_.z,d_}.dot(P);
    }

    float distanceToPoint (const Float3& P) const{
        return distanceToPoint(Float4{P,1.0f});
    }

public:
    // Comparisons to support sorted containers.
    inline bool operator==(const Plane& plane) const;
    inline bool operator!=(const Plane& plane) const;
    inline bool operator< (const Plane& plane) const;
    inline bool operator<=(const Plane& plane) const;
    inline bool operator> (const Plane& plane) const;
    inline bool operator>=(const Plane& plane) const;
};

// Template alias for convenience.
using Plane3 = Plane<Float3>;
using Plane4 = Plane<Float4>;

template <typename VEC>
inline bool Plane<VEC>::operator==(const Plane& plane) const
{
    return normal_ == plane.normal_ && d_ == plane.d_;
}

template <typename VEC>
inline bool Plane<VEC>::operator!=(const Plane& plane) const
{
    return !operator==(plane);
}

template <typename VEC>
inline bool Plane<VEC>::operator<(const Plane& plane) const
{
    if (normal_ < plane.normal_)
        return true;

    if (normal_ > plane.normal_)
        return false;

    return d_ < plane.d_;
}

template <typename VEC>
inline bool Plane<VEC>::operator<=(const Plane& plane) const
{
    return operator<(plane) || operator==(plane);
}

template <typename VEC>
inline bool Plane<VEC>::operator>(const Plane& plane) const
{
    return !operator<=(plane);
}

template <typename VEC>
inline bool Plane<VEC>::operator>=(const Plane& plane) const
{
    return !operator<(plane);
}
