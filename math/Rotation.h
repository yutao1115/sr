#pragma once

#include "Matrix.h"
#include "AxisAngle.h"
#include "EulerAngles.h"
#include "Quaternion.h"

namespace Rotation{
    template<typename V>
    void convert(const MatrixNN<V>& r, EulerAngles<typename V::type>& e){
        // cosy*cosr+sinr*siny*sinp  -cosy*sinr+cosr*siny*sinp   siny*cosp|
        // sinr*cosp             cosp*cosr                         -sinp  |
        //-siny*cosr+cosy*sinp*sinr   siny*sinr+cosy*sinp*cosr   cosy*cosp|
        static_assert(V::N == 3 || V::N == 4, "Dimension must be 3 or 4.");
        using Real = typename V::type;
        if (r(1,2) < (Real)1) {
            if (r(1,2) > (Real)-1) {
                e.yaw_   = atan2(r(0,2),r(2,2));
                e.pitch_ = asin(-r(1, 2));
                e.roll_  = atan2(r(1,0),r(1,1));
            }
            else {
                e.yaw_   = -atan2(-r(0,1),r(0,0));
                e.pitch_ = (Real)PiHalf;
                e.roll_  = (Real)0;
            }
        }
        else {
            e.yaw_   = atan2(-r(0,1),r(0,0));
            e.pitch_ = -(Real)PiHalf;
            e.roll_  = (Real)0;
        }
    }
    template<typename V>
    void convert(const EulerAngles<typename V::type>& e,MatrixNN<V>& r){
        // ---------------------------------------------------------------------------------------------------------------------
        // yaw            |   pitch         |  roll           |                      result                                    |
        // ---------------|-----------------|-----------------|-----------------------------------------------------------------
        // cosy   0  siny |   1   0    0    |  cosr   -sinr  0| cosy*cosr+sinr*siny*sinp  -cosy*sinr+cosr*siny*sinp   siny*cosp|
        // 0      1   0   |   0  cosp -sinp |  sinr    cosr  0|        sinr*cosp             cosp*cosr                  -sinp  |
        // -siny  0  cosy |   0  sinp  cosp |   0       0    1|-siny*cosr+cosy*sinp*sinr   siny*sinr+cosy*sinp*cosr   cosy*cosp|
        // ---------------|-----------------|-----------------|-----------------------------------------------------------------
       static_assert(V::N == 3 || V::N == 4, "Dimension must be 3 or 4.");
       using Real = typename V::type;
       auto siny = (Real)sin(e.yaw_);
       auto cosy = (Real)cos(e.yaw_);
       auto sinp = (Real)sin(e.pitch_);
       auto cosp = (Real)cos(e.pitch_);
       auto sinr = (Real)sin(e.roll_);
       auto cosr = (Real)cos(e.roll_);
       r.makeIdentity();
       r(0,0) = cosy*cosr+sinr*siny*sinp ;
       r(0,1) = -cosy*sinr+cosr*siny*sinp;
       r(0,2) = siny*cosp;
       r(1,0) = sinr*cosp;
       r(1,1) = cosp*cosr;
       r(1,2) = -sinp;
       r(2,0) = -siny*cosr+cosy*sinp*sinr;
       r(2,1) =  siny*sinr+cosy*sinp*cosr;
       r(2,2) =  cosy*cosp;
       r.cols[0].normalize();
       r.cols[1].normalize();
       r.cols[2].normalize();
    }
    template<typename V>
    void convert(const MatrixNN<V>& r,Quaternion& q){
        static_assert(V::N == 3 || V::N == 4, "Dimension must be 3 or 4.");
        using Real = typename V::type;
        Real r22 = r(2, 2);
        if (r22 <= (Real)0){  // x^2 + y^2 >= z^2 + w^2
            Real dif10 = r(1, 1) - r(0, 0);
            Real omr22 = (Real)1 - r22;
            if (dif10 <= (Real)0){  // x^2 >= y^2
                Real fourXSqr = omr22 - dif10;
                Real inv4x = ((Real)0.5) / sqrt(fourXSqr);
                q.x = fourXSqr*inv4x;
                q.y = (r(0, 1) + r(1, 0))*inv4x;
                q.z = (r(0, 2) + r(2, 0))*inv4x;
                q.w = (r(2, 1) - r(1, 2))*inv4x;
            }
            else  {// y^2 >= x^2
                Real fourYSqr = omr22 + dif10;
                Real inv4y = ((Real)0.5) / sqrt(fourYSqr);
                q.x = (r(0, 1) + r(1, 0))*inv4y;
                q.y = fourYSqr*inv4y;
                q.z = (r(1, 2) + r(2, 1))*inv4y;
                q.w = (r(0, 2) - r(2, 0))*inv4y;
            }
        }
        else{  // z^2 + w^2 >= x^2 + y^2
            Real sum10 = r(1, 1) + r(0, 0);
            Real opr22 = (Real)1 + r22;
            if (sum10 <= (Real)0)  // z^2 >= w^2
            {
                Real fourZSqr = opr22 - sum10;
                Real inv4z = ((Real)0.5) / sqrt(fourZSqr);
                q.x = (r(0, 2) + r(2, 0))*inv4z;
                q.y = (r(1, 2) + r(2, 1))*inv4z;
                q.z = fourZSqr*inv4z;
                q.w = (r(1, 0) - r(0, 1))*inv4z;
            }
            else  // w^2 >= z^2
            {
                Real fourWSqr = opr22 + sum10;
                Real inv4w = ((Real)0.5) / sqrt(fourWSqr);
                q.x = (r(2, 1) - r(1, 2))*inv4w;
                q.y = (r(0, 2) - r(2, 0))*inv4w;
                q.z = (r(1, 0) - r(0, 1))*inv4w;
                q.w = fourWSqr*inv4w;
            }
        }
    }

    template<typename ColT>
    void convert(const Quaternion& q,MatrixNN<ColT>& r){
        static_assert(ColT::N ==3 || ColT::N == 4,"matrix must be 3X3 or 4X4");
        using Real = typename ColT::type;

        r.makeIdentity();

        Real twoX = ((Real)2)*q.x;
        Real twoY = ((Real)2)*q.y;
        Real twoZ = ((Real)2)*q.z;
        Real twoXX = twoX*q.x;
        Real twoXY = twoX*q.y;
        Real twoXZ = twoX*q.z;
        Real twoXW = twoX*q.w;
        Real twoYY = twoY*q.y;
        Real twoYZ = twoY*q.z;
        Real twoYW = twoY*q.w;
        Real twoZZ = twoZ*q.z;
        Real twoZW = twoZ*q.w;

        r(0, 0) = (Real)1 - twoYY - twoZZ;
        r(0, 1) = twoXY - twoZW;
        r(0, 2) = twoXZ + twoYW;
        r(1, 0) = twoXY + twoZW;
        r(1, 1) = (Real)1 - twoXX - twoZZ;
        r(1, 2) = twoYZ - twoXW;
        r(2, 0) = twoXZ - twoYW;
        r(2, 1) = twoYZ + twoXW;
        r(2, 2) = (Real)1 - twoXX - twoYY;
    }

    template<typename Real>
    void convert(const EulerAngles<Real>& e , Quaternion& q){
        Real sp, sr, sy;
        Real cp, cr, cy;
        sinCos(&sp, &cp, e.pitch_ * 0.5f);
        sinCos(&sr, &cr, e.roll_  * 0.5f);
        sinCos(&sy, &cy, e.yaw_   * 0.5f);

        q.w =  cy*cp*cr + sy*sp*sr;
        q.x =  cy*sp*cr + sy*cp*sr;
        q.y = -cy*sp*sr + sy*cp*cr;
        q.z = -sy*sp*cr + cy*cp*sr;
    }

    template<typename Real>
    void convert(const Quaternion& q,EulerAngles<Real>& e){
        // cosy*cosr+sinr*siny*sinp  -cosy*sinr+cosr*siny*sinp   siny*cosp|
        // sinr*cosp                    cosp*cosr                  -sinp  |
        //-siny*cosr+cosy*sinp*sinr   siny*sinr+cosy*sinp*cosr   cosy*cosp|

        // 1-2yy-2zz    2xy-2zw    2xz+2yw
        // 2xy+2zw     1-2xx-2zz   2yz-2xw
        // 2xz-2yw      2yz+2xw   1-2xx-2yy
        Real sinp = (Real)-2 * (q.y*q.z - q.w*q.x);
        if( sinp < (Real)1 ){
            if( sinp > (Real)-1 ){
                e.pitch_= asin(sinp);
                e.yaw_  = atan2(q.x*q.z + q.w*q.y, (Real)0.5 - q.x*q.x - q.y*q.y);
                e.roll_ = atan2(q.x*q.y + q.w*q.z, (Real)0.5 - q.x*q.x - q.z*q.z);
            }
            else{
                e.yaw_  = -atan2(-q.x*q.y+q.z*q.w,(Real)0.5 - q.y*q.y - q.z*q.z);
                e.pitch_= (Real)PiHalf;
                e.roll_ = (Real)0;
            }
        }
        else {
            e.yaw_   = atan2(-q.x*q.y+q.z*q.w,(Real)0.5 - q.y*q.y - q.z*q.z);
            e.pitch_ = -(Real)PiHalf;
            e.roll_  = (Real)0;
        }
    }
    template<typename ColT>
    void convert(const MatrixNN<ColT>& r,AxisAngle<ColT>& a){
        static_assert(ColT::N ==3 || ColT::N == 4,"matrix must be 3X3 or 4X4");
        using Real = typename ColT::type;
        Real trace = r(0, 0) + r(1, 1) + r(2, 2);
        Real half = (Real)0.5;
        Real cs = half*(trace - (Real)1);
        cs = std::max(std::min(cs, (Real)1), (Real)-1);
        a.angle_ = acos(cs);  // The angle is in [0,pi].
        a.axis_.makeZero();

        if (a.angle_ > (Real)0) {
            if (a.angle_ < (Real)Pi) {
                // The angle is in (0,pi).
                a.axis_.x = r(2, 1) - r(1, 2);
                a.axis_.y = r(0, 2) - r(2, 0);
                a.axis_.z = r(1, 0) - r(0, 1);
                a.axis_.normalize();
            }
            else {
                // The angle is pi, in which case R is symmetric and
                // R+I = 2*(I+S^2) = 2*U*U^T, where U = (u0,u1,u2) is the
                // unit-length direction of the rotation axis.  Determine the
                // largest diagonal entry of R+I and normalize the
                // corresponding row to produce U.  It does not matter the
                // sign on u[d] for chosen diagonal d, because R(U,pi) = R(-U,pi).
                Real one = (Real)1;
                if (r(0, 0) >= r(1, 1)) {
                    if (r(0, 0) >= r(2, 2)) {
                        // r00 is maximum diagonal term
                        a.axis_.x = r(0, 0) + one;
                        a.axis_.y = half*(r(0, 1) + r(1, 0));
                        a.axis_.z = half*(r(0, 2) + r(2, 0));
                    }
                    else {
                        // r22 is maximum diagonal term
                        a.axis_.x = half*(r(2, 0) + r(0, 2));
                        a.axis_.y = half*(r(2, 1) + r(1, 2));
                        a.axis_.z = r(2, 2) + one;
                    }
                }
                else {
                    if (r(1, 1) >= r(2, 2)) {
                        // r11 is maximum diagonal term
                        a.axis_.x = half*(r(1, 0) + r(0, 1));
                        a.axis_.y = r(1, 1) + one;
                        a.axis_.z = half*(r(1, 2) + r(2, 1));
                    }
                    else {
                        // r22 is maximum diagonal term
                        a.axis_.x = half*(r(2, 0) + r(0, 2));
                        a.axis_.y = half*(r(2, 1) + r(1, 2));
                        a.axis_.z = r(2, 2) + one;
                    }
                }
                a.axis_.normalize();
            }
        }
        else {
            // The angle is 0 and the matrix is the identity.  Any axis will
            // work, so choose the Unit(0) axis.
            a.axis_.x = (Real)1;
        }

    }

    template<typename ColT,typename MaT>
    void convert(const AxisAngle<ColT>& a,MatrixNN<MaT>& r){
        static_assert(MaT::N == 3 || MaT::N  == 4,"matrix must be 3X3 or 4X4");
        static_assert(ColT::N ==3 || ColT::N == 4,"axisAngle must be 3X3 or 4X4");
        using Real = typename ColT::type;
        r.makeIdentity();

        Real cs = cos(a.angle_);
        Real sn = sin(a.angle_);
        Real oneMinusCos = ((Real)1) - cs;
        Real x0sqr = a.axis_.x * a.axis_.x;
        Real x1sqr = a.axis_.y * a.axis_.y;
        Real x2sqr = a.axis_.z * a.axis_.z;
        Real x0x1m = a.axis_.x * a.axis_.y * oneMinusCos;
        Real x0x2m = a.axis_.x * a.axis_.z * oneMinusCos;
        Real x1x2m = a.axis_.y * a.axis_.z * oneMinusCos;
        Real x0Sin = a.axis_.x * sn;
        Real x1Sin = a.axis_.y * sn;
        Real x2Sin = a.axis_.z * sn;

        r(0, 0) = x0sqr*oneMinusCos + cs;
        r(0, 1) = x0x1m - x2Sin;
        r(0, 2) = x0x2m + x1Sin;
        r(1, 0) = x0x1m + x2Sin;
        r(1, 1) = x1sqr*oneMinusCos + cs;
        r(1, 2) = x1x2m - x0Sin;
        r(2, 0) = x0x2m - x1Sin;
        r(2, 1) = x1x2m + x0Sin;
        r(2, 2) = x2sqr*oneMinusCos + cs;
    }

    template<typename V>
    void convert(const Quaternion& q,AxisAngle<V>& a){
        static_assert(V::N ==3 || V::N == 4,"Vector must be 3 or 4");
        using Real = typename V::type;
        a.axis_.makeZero();
        Real axis_SqrLen = q.x * q.x + q.y * q.y + q.z * q.z;
        if (axis_SqrLen > (Real)0){
            Real adjust = ((Real)1) / sqrt(axis_SqrLen);
            a.axis_.x = q.x * adjust;
            a.axis_.y = q.y * adjust;
            a.axis_.z = q.z * adjust;
            Real cs = std::max(std::min(q.w, (Real)1), (Real)-1);
            a.angle_ = ((Real)2)*acos(cs);
        }
        else{
            // The angle is 0 (modulo 2*pi). Any axis_ will work, so choose the
            // Unit(0) axis_.
            a.axis_.x = (Real)1;
            a.angle_ = (Real)0;
        }
    }

    template<typename V>
    void convert(const AxisAngle<V>& a,Quaternion& q){
        static_assert(V::N ==3 || V::N == 4,"Vector must be 3 or 4");
        using Real = typename V::type;
        assert (equal(a.axis_.length(),(Real)1));
        Real halfAngle = ((Real)0.5)*a.angle_;
        Real sn = sin(halfAngle);
        q.x = sn*a.axis_.x;
        q.y = sn*a.axis_.y;
        q.z = sn*a.axis_.z;
        q.w = cos(halfAngle);
    }

}
