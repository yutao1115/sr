#include "Transform.h"
#include "Rotation.h"

void Transform::setRotation(const Quaternion& q){
    Rotation::convert(q,mRMatrix_);
    isIdentity_ = false;
    updateHMatrix();
}

void Transform::getRotation(Quaternion& q) const{
    Rotation::convert(mRMatrix_,q);
}


// The Euler angles are in radians.
void Transform::setRotation(const EulerAngles<float>& eulerAngles){
    Rotation::convert(eulerAngles,mRMatrix_);
    isIdentity_ = false;
    updateHMatrix();
}

void Transform::getRotation(EulerAngles<float>& eulerAngles) const{
    Rotation::convert(mRMatrix_,eulerAngles);
}

void Transform::setRotation(const AxisAngle4& aixa){
    Rotation::convert(aixa,mRMatrix_);
    isIdentity_ = false;
    updateHMatrix();
}


const Matrix44& Transform::getHInverse() {
    if (inverseNeedsUpdate_)
    {
        if (isIdentity_)
        {
            invHMatrix_.makeIdentity();
        }
        else
        {
            if (isUniformScale_)
            {
                float invScale = 1.0f / scale_.x;
                invHMatrix_.axis(0) = Float4{invScale * mRMatrix_(0, 0),
                                             invScale * mRMatrix_(1, 0),
                                             invScale * mRMatrix_(2, 0),0};

                invHMatrix_.axis(1) = Float4{invScale * mRMatrix_(0, 1),
                                             invScale * mRMatrix_(1, 1),
                                             invScale * mRMatrix_(2, 1),0};

                invHMatrix_.axis(2) = Float4{invScale * mRMatrix_(0, 2),
                                             invScale * mRMatrix_(1, 2),
                                             invScale * mRMatrix_(2, 2),0};
            }
            else
            {
                // Replace 3 reciprocals by 6 multiplies and 1 reciprocal.
                float s01 = scale_.m[0] * scale_.m[1];
                float s02 = scale_.m[0] * scale_.m[2];
                float s12 = scale_.m[1] * scale_.m[2];
                float invs012 = 1.0f / (s01 * scale_.m[2]);
                float invS0 = s12 * invs012;
                float invS1 = s02 * invs012;
                float invS2 = s01 * invs012;
                invHMatrix_.axis(0) = Float4{invS0 * mRMatrix_(0, 0),
                                             invS0 * mRMatrix_(1, 0),
                                             invS0 * mRMatrix_(2, 0),0};

                invHMatrix_.axis(1) = Float4{invS1* mRMatrix_(0, 1),
                                             invS1* mRMatrix_(1, 1),
                                             invS1* mRMatrix_(2, 1),0};

                invHMatrix_.axis(2) = Float4{invS2 * mRMatrix_(0, 2),
                                             invS2 * mRMatrix_(1, 2),
                                             invS2 * mRMatrix_(2, 2),0};
            }

            invHMatrix_(0, 3) = -(
                invHMatrix_(0, 0) * translate_.x +
                invHMatrix_(0, 1) * translate_.y +
                invHMatrix_(0, 2) * translate_.z
                );

            invHMatrix_(1, 3) = -(
                invHMatrix_(1, 0) * translate_.x +
                invHMatrix_(1, 1) * translate_.y +
                invHMatrix_(1, 2) * translate_.z
                );

            invHMatrix_(2, 3) = -(
                invHMatrix_(2, 0) * translate_.x +
                invHMatrix_(2, 1) * translate_.y +
                invHMatrix_(2, 2) * translate_.z
                );

            // The last row of mHMatrix is always (0,0,0,1) for an affine
            // transformation, so it is set once in the constructor.  It is
            // not necessary to reset it here.

        }

        inverseNeedsUpdate_ = false;
    }

    return invHMatrix_;
}


