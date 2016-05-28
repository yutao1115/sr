#pragma once

#include "Vectors.h"
#include "ChebyshevRatio.h"
#include <assert.h>

// x,y,z,w   q = x*i + y*j + z*k + w
typedef Float4 Quaternion;
inline float angleQuat(const Quaternion&normalizedA , const Quaternion&normalizedB){
    assert(equal(normalizedA.length(),1.0f));
    assert(equal(normalizedB.length(),1.0f));
    return acos(normalizedB.dot(normalizedA));
}

inline Quaternion makeIdentityQuat(){
    return Quaternion{0,0,0,1};
}

inline float angleQuat(const Quaternion& q){
    float cs = max2(min2(q.w, 1.0f), -1.0f);
    return acos(cs) * 2.0f;
}

inline Quaternion createQuat(Float3 axis,float radianV){
    return Quaternion{axis.x,axis.y,axis.z,radianV};
}

inline bool equalQuat(const Quaternion& a , const Quaternion& b){
    return  equal( a.dot(b) , 1.0f);
}

inline Float3 axisQuat(const Quaternion& q){
    // w = cos(theta/2)
    // sin^2(x) + cos^2(x) = 1
    float sinThetaOver2Sq = 1.0f - q.w*q.w;
    if (sinThetaOver2Sq > 0.0f) {
        float oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);
        return Float3{q.x * oneOverSinThetaOver2, q.y * oneOverSinThetaOver2,
                q.z * oneOverSinThetaOver2};
    }
    return Float3{1.0f,0,0};
}

inline Quaternion operator*(const Quaternion& q , const Quaternion& p) {
    // Multiplication of quaternions.  This operation is not generally
    // commutative; that is, q0*q1 and q1*q0 are not usually the same value.
    // (x0*i + y0*j + z0*k + w0)*(x1*i + y1*j + z1*k + w1) =
    // i*(+x0*w1 + y0*z1 - z0*y1 + w0*x1) +
    // j*(-x0*z1 + y0*w1 + z0*x1 + w0*y1) +
    // k*(+x0*y1 - y0*x1 + z0*w1 + w0*z1) +
    // 1*(-x0*x1 - y0*y1 - z0*z1 + w0*w1)
    return Quaternion {
        +q.x*p.w + q.y*p.z - q.z*p.y + q.w*p.x,
        -q.x*p.z + q.y*p.w + q.z*p.x + q.w*p.y,
        +q.x*p.y - q.y*p.x + q.z*p.w + q.w*p.z,
        -q.x*p.x - q.y*p.y - q.z*p.z + q.w*p.w};
}
// The conjugate of q = (x,y,z,w) is conj(q) = (-x,-y,-z,w).
inline Quaternion conjugate(Quaternion const& q){
    return Quaternion{-q.x, -q.y, -q.z, +q.w};
}

// For a nonzero quaternion q = (x,y,z,w), inv(q) = (-x,-y,-z,w)/|q|^2, where
// |q| is the length of the quaternion.  When q is zero, the function returns
// zero, which is considered to be an improbable case.
inline Quaternion inverse(Quaternion const& q) {
    float sqrLen = q.dot(q);
    if (sqrLen > 0) {
        float invSqrLen = 1.0f / sqrLen;
        return conjugate(q)*invSqrLen;
    }
    else {
        return Quaternion{0,0,0,0};
    }
}


inline Float3 rotate(Quaternion const&q,Float3 const& v){
    Float4 u = q * Float4(v.x,v.y,v.z,0) * conjugate(q);
    return Float3(u.x,u.y,u.z);
}

// Rotate a vector using quaternion multiplication.  The input quaternion must
// be unit length.
inline Float4 rotate(Quaternion const& q, Float4 const& v){
    Float4 u = q * v * conjugate(q);
    // Zero-out the w-component in remove numerical round-off error.
    u.w = 0;
    return u;
}


inline Quaternion lerpQuat(const Quaternion& q1,const Quaternion&q2,float ratio){
    return (q1 + (q2- q1) * ratio).normalize();
}

// The spherical linear interpolation (slerp) of unit-length quaternions
// q0 and q1 for t in [0,1] is
//     slerp(t,q0,q1) = [sin(t*theta)*q0 + sin((1-t)*theta)*q1]/sin(theta)
// where theta is the angle between q0 and q1 [cos(theta) = Dot(q0,q1)].
// This function is a parameterization of the great spherical arc between
// q0 and q1 on the unit hypersphere.  Moreover, the parameterization is
// one of normalized arclength--a particle traveling along the arc through
// time t does so with constant speed.
//
// When using slerp in animations involving sequences of quaternions, it is
// typical that the quaternions are preprocessed so that consecutive ones
// form an acute angle A in [0,pi/2].  Other preprocessing can help with
// performance.  See the function comments below.
//
// See SlerpEstimate.{h,inl} for various approximations, including
// SLERP::EstimateRPH that gives good performance and accurate results
// for preprocessed quaternions.

// The angle between q0 and q1 is in [0,pi).  There are no angle restrictions
// restrictions and nothing is precomputed.
inline Quaternion slerp(Quaternion const& q0, Quaternion const& q1,float ratio){
    float cosA = q0.dot(q1);
    float sign;
    if (cosA >= 0) {sign = 1.0f;}
    else {cosA = -cosA; sign = -1.0f;}

    float f0, f1;
    ChebyshevRatio::get(f0,f1,ratio,cosA);
    return q0 * f0 + q1 * (sign * f1);
}

// The angle between q0 and q1 must be in [0,pi/2].  The suffix R is for
// 'Restricted'.  The preprocessing code is
//   Quaternion<Real> q[n];  // assuming initialized
//   for (i0 = 0, i1 = 1; i1 < n; i0 = i1++)
//   {
//       cosA = Dot(q[i0], q[i1]);
//       if (cosA < 0)
//       {
//           q[i1] = -q[i1];  // now Dot(q[i0], q[i]1) >= 0
//       }
//   }
inline Quaternion slerpR(Quaternion const& q0, Quaternion const& q1,float ratio){
    float f0, f1;
    ChebyshevRatio::get(f0,f1,ratio,q0.dot(q1));
    return q0 * f0 + q1 * f1;
}



// The angle between q0 and q1 must be in [0,pi/2].  The suffix R is for
// 'Restricted' and the suffix P is for 'Preprocessed'.  The preprocessing
// code is
//   Quaternion<Real> q[n];  // assuming initialized
//   Real cosA[n-1], omcosA[n-1];  // to be precomputed
//   for (i0 = 0, i1 = 1; i1 < n; i0 = i1++)
//   {
//       cs = Dot(q[i0], q[i1]);
//       if (cosA[i0] < 0)
//       {
//           q[i1] = -q[i1];
//           cs = -cs;
//       }
//       cosA[n-1] = cs;  // for GeneralRP
//       omcosA[i0] = 1 - cs;  // for EstimateRP
//   }
inline Quaternion slerpRP(Quaternion const& q0, Quaternion const& q1,float ratio, float cosA){
    float f0, f1;
    ChebyshevRatio::get(f0, f1, ratio, cosA);
    return q0 * f0 + q1 * f1;
}


