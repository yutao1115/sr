#pragma once
#include <initializer_list>
#include "Common.h"
#include <assert.h>

template<typename Real>
struct EulerAngles{
    Real   yaw_;
    Real   pitch_;
    Real   roll_;

    EulerAngles():yaw_(0),pitch_(0),roll_(0){}
    EulerAngles(Real y,Real p,Real r):yaw_(y),pitch_(p),roll_(r){}
    void makeIdentity(){yaw_=pitch_=roll_=0;}
    void canonize(){
        // First, wrap pitch in range -pi ... pi
        pitch_ = wrapPi(pitch_);
        // Now, check for "the back side" of the matrix, pitch outside
        // the canonical range of -pi/2 ... pi/2
        if (pitch_ < -PiHalf) {
            pitch_ = -Pi - pitch_;
            yaw_  += Pi;
            roll_ += Pi;
        }
        else if (pitch_ > PiHalf) {
            pitch_ = Pi - pitch_;
            yaw_  += Pi;
            roll_ += Pi;
        }
        // OK, now check for the gimbel lock case (within a slight
        // tolerance)
        if (fabs(pitch_) > PiHalf - 1e-4) {
            // We are in gimbel lock.  Assign all rotation
            // about the vertical axis to heading
            yaw_  += roll_;
            roll_  = 0;
        } else {
            // Not in gimbel lock.  Wrap the bank angle in
            // canonical range
            roll_ = wrapPi(roll_);
        }
        // Wrap heading in canonical range
        yaw_ = wrapPi(yaw_);
    }
};
using EulerAnglesF = EulerAngles<float>;

