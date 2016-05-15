#pragma once
#include "Matrix.h"
#include "EulerAngles.h"
#include "AxisAngle.h"
#include "Quaternion.h"

class Transform
{
public:
    // The default identity transformation
    Transform(void):
        translate_{ 0.0f, 0.0f, 0.0f, 1.0f },
        scale_{ 1.0f, 1.0f, 1.0f, 1.0f },
        isUniformScale_(true),isIdentity_(true),inverseNeedsUpdate_(true)
        {
            makeIdentity();
        }

    // Implicit conversion
    operator Matrix44 const&() const{return mHMatrix_;}

    void makeIdentity(){
        mRMatrix_.makeIdentity();
        translate_ = { 0.0f, 0.0f, 0.0f, 1.0f };
        scale_ = { 1.0f, 1.0f, 1.0f, 1.0f };
        isIdentity_ = true;
        isUniformScale_ = true;
        updateHMatrix();
    }

    void makeUnitScale(){
        scale_ = { 1.0f, 1.0f, 1.0f, 1.0f };
        isUniformScale_ = true;
        updateHMatrix();
    }

    bool isUniformScale() const{return isUniformScale_;}

    void setTranslation(float x0, float x1, float x2){
        translate_ = { x0, x1, x2, 1.0f };
        isIdentity_ = false;
        updateHMatrix();
    }

    void updateTranslation(float x0, float x1, float x2){
        translate_ = { translate_.x+x0, translate_.y+x1, translate_.z+x2, 1.0f };
        isIdentity_ = false;
        updateHMatrix();
    }

    void setScale(float s0, float s1, float s2){
        scale_ = { s0, s1, s2, 1.0f };
        isIdentity_ = false;
        isUniformScale_ = false;
        updateHMatrix();
    }

    void setUniformScale(float scale){
        scale_ = { scale, scale, scale, 1.0f };
        isIdentity_ = false;
        isUniformScale_ = true;
        updateHMatrix();
    }


    Float3 getTranslation() const{ return Float3{translate_.x,translate_.y,translate_.z};}
    Float3 getScale() const {return Float3{scale_.x,scale_.y,scale_.z};}

    void setRotation(const Matrix44& rotate){
        mRMatrix_   = rotate;
        isIdentity_ = false;
        updateHMatrix();
    }

    Float4& rotateAxis(int inx){ return mRMatrix_.axis(inx);}

    Matrix44 const& getRotation(void) const{return mRMatrix_;}

    // The quaternion is unit length.
    void setRotation(const Quaternion& q);
    void getRotation(Quaternion& q) const;

    // The Euler angles are in radians.
    void setRotation(const EulerAngles<float>& eulerAngles);
    void getRotation(EulerAngles<float>& eulerAngles) const;

    void setRotation(const AxisAngle4& aixa);
    // Get the homogeneous matrix.
    Matrix44 const& getHMatrix() const{return mHMatrix_;}

    Matrix44 const& getHInverse();
    inline void updateHMatrix();

private:

    Matrix44  mHMatrix_;
    Matrix44  invHMatrix_;

    Matrix44   mRMatrix_;   // R
    Float4     translate_;  // T
    Float4     scale_;      // S
    bool isUniformScale_;
    bool isIdentity_;
    mutable bool inverseNeedsUpdate_;
};


inline void Transform::updateHMatrix(void){
    if (isIdentity_)
    {
        mHMatrix_.makeIdentity();
    }
    else
    {
        mHMatrix_(0, 0) = mRMatrix_(0, 0) * scale_.m[0];
        mHMatrix_(0, 1) = mRMatrix_(0, 1) * scale_.m[1];
        mHMatrix_(0, 2) = mRMatrix_(0, 2) * scale_.m[2];
        mHMatrix_(1, 0) = mRMatrix_(1, 0) * scale_.m[0];
        mHMatrix_(1, 1) = mRMatrix_(1, 1) * scale_.m[1];
        mHMatrix_(1, 2) = mRMatrix_(1, 2) * scale_.m[2];
        mHMatrix_(2, 0) = mRMatrix_(2, 0) * scale_.m[0];
        mHMatrix_(2, 1) = mRMatrix_(2, 1) * scale_.m[1];
        mHMatrix_(2, 2) = mRMatrix_(2, 2) * scale_.m[2];

        mHMatrix_(0, 3) = translate_.x;
        mHMatrix_(1, 3) = translate_.y;
        mHMatrix_(2, 3) = translate_.z;
    }
    inverseNeedsUpdate_ = true;
}


// Compute M*V.
inline Float4 operator*(const Transform& M,const Float4& V){
    return M.getHMatrix() * V;
}

inline Matrix44 operator*(Matrix44 const& A,Transform const& B){
    return A * B.getHMatrix();
}

inline Matrix44 operator*(Transform const& A,Matrix44 const& B){
    return B * A.getHMatrix();
}


