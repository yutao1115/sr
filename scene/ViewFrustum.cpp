#include "scene/ViewFrustum.h"


ViewFrustum::ViewFrustum(bool isPerspective)
    :isPerspective_(isPerspective)
{
    setFrame({ 0.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f });

    if (isPerspective_)
    {
        setFrustum(90.0f, 1.0f, 1.0f, 1.0f, 100.0f);
    }
    else
    {
        setFrustum(-1.0f,1.0f,-1.0f,1.0f,0.0f, 1.0f);
    }
}



void ViewFrustum::setAxis(const Float4& xVector,
    const Float4& yVector, const Float4& zVector)
{
    zVector_ = zVector;
    yVector_ = yVector;
    xVector_ = xVector;
    zVector_.normalize();
    yVector_.normalize();
    xVector_.normalize();
    constexpr float epsilon = 0.01f;
    float det = yVector_.cross(xVector_).dot(zVector_);
    if (fabs(1.0f - det) > epsilon)
    {
        // The input vectors do not appear to form an orthonormal set.
        gramSchmidtOrthlize(3, &zVector_);
    }
    onFrameChange();
}

bool ViewFrustum::getFrustum(float& upFovDegrees, float& aspectRatio, float& dMin, float& dMax) const
{
    if (frustum_[VF_XMIN] == -frustum_[VF_XMAX]
        && frustum_[VF_YMIN] == -frustum_[VF_YMAX])
    {
        float tmp = frustum_[VF_YMAX] / frustum_[VF_ZMIN];
        upFovDegrees = 2.0f * atan(tmp) * (float)_RadToDeg;
        aspectRatio = frustum_[VF_XMAX] / frustum_[VF_YMAX];
        dMin = frustum_[VF_ZMIN];
        dMax = frustum_[VF_ZMAX];
        return true;
    }
    return false;
}

void ViewFrustum::onFrameChange()
{
    viewMatrix_(0, 0) = xVector_.x;
    viewMatrix_(0, 1) = xVector_.y;
    viewMatrix_(0, 2) = xVector_.z;
    viewMatrix_(0, 3) = - position_.dot(xVector_);
    viewMatrix_(1, 0) = yVector_.x;
    viewMatrix_(1, 1) = yVector_.y;
    viewMatrix_(1, 2) = yVector_.z;
    viewMatrix_(1, 3) = - position_.dot(yVector_);
    viewMatrix_(2, 0) = zVector_.x;
    viewMatrix_(2, 1) = zVector_.y;
    viewMatrix_(2, 2) = zVector_.z;
    viewMatrix_(2, 3) = - position_.dot(zVector_);
    viewMatrix_(3, 0) = 0.0f;
    viewMatrix_(3, 1) = 0.0f;
    viewMatrix_(3, 2) = 0.0f;
    viewMatrix_(3, 3) = 1.0f;

    updatePVMatrix();
}

void ViewFrustum::onFrustumChange()
{
    // Map (x,y,z) into [-1,1]x[-1,1]x[-1,1].
    float dMin = frustum_[VF_ZMIN];
    float dMax = frustum_[VF_ZMAX];
    float uMin = frustum_[VF_YMIN];
    float uMax = frustum_[VF_YMAX];
    float rMin = frustum_[VF_XMIN];
    float rMax = frustum_[VF_XMAX];
    float invDDiff = 1.0f / (dMax - dMin);
    float invUDiff = 1.0f / (uMax - uMin);
    float invRDiff = 1.0f / (rMax - rMin);

    if (isPerspective_)
    {
        projectionMatrix_(0, 0) = 2.0f*dMin*invRDiff;
        projectionMatrix_(0, 1) = 0.0f;
        projectionMatrix_(0, 2) = (rMin + rMax)*invRDiff;
        projectionMatrix_(0, 3) = 0.0f;
        projectionMatrix_(1, 0) = 0.0f;
        projectionMatrix_(1, 1) = 2.0f*dMin*invUDiff;
        projectionMatrix_(1, 2) = (uMin + uMax)*invUDiff;
        projectionMatrix_(1, 3) = 0.0f;
        projectionMatrix_(2, 0) = 0.0f;
        projectionMatrix_(2, 1) = 0.0f;
        projectionMatrix_(2, 2) = -(dMin+dMax)*invDDiff;
        projectionMatrix_(2, 3) = -2*dMin*dMax*invDDiff;
        projectionMatrix_(3, 0) = 0.0f;
        projectionMatrix_(3, 1) = 0.0f;
        projectionMatrix_(3, 2) = -1.0f;
        projectionMatrix_(3, 3) = 0.0f;
    }
    else
    {
        projectionMatrix_(0, 0) = 2.0f*invRDiff;
        projectionMatrix_(0, 1) = 0.0f;
        projectionMatrix_(0, 2) = 0.0f;
        projectionMatrix_(0, 3) = -(rMin + rMax)*invRDiff;
        projectionMatrix_(1, 0) = 0.0f;
        projectionMatrix_(1, 1) = 2.0f*invUDiff;
        projectionMatrix_(1, 2) = 0.0f;
        projectionMatrix_(1, 3) = -(uMin + uMax)*invUDiff;
        projectionMatrix_(2, 0) = 0.0f;
        projectionMatrix_(2, 1) = 0.0f;
        projectionMatrix_(2, 2) = -2*invDDiff;
        projectionMatrix_(2, 3) = -(dMin+dMax)*invDDiff;
        projectionMatrix_(3, 0) = 0.0f;
        projectionMatrix_(3, 1) = 0.0f;
        projectionMatrix_(3, 2) = 0.0f;
        projectionMatrix_(3, 3) = 1.0f;
    }

    updatePVMatrix();
}




